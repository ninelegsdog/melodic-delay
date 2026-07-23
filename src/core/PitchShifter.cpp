#include "PitchShifter.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include "kiss_fft.h"
#include "kiss_fftr.h"

PitchShifter::PitchShifter(PitchMode mode, int sampleRate)
    : mode_(mode), sampleRate_(sampleRate) {
    bufferSize_ = static_cast<size_t>(sampleRate) * 2;
    buffer_.resize(bufferSize_, 0.0f);

    // Phase vocoder initialization
    inputBuffer_.resize(FFT_SIZE, 0.0f);
    outputBuffer_.resize(FFT_SIZE * 2, 0.0f);

    // Hann window
    window_.resize(WINDOW_SIZE);
    for (int i = 0; i < WINDOW_SIZE; i++) {
        window_[i] = 0.5f * (1.0f - std::cos(2.0f * M_PI * i / (WINDOW_SIZE - 1)));
    }

    overlapBuffer_.resize(FFT_SIZE, 0.0f);

    // Phase tracking
    phaseAccumulator_.resize(FFT_SIZE / 2 + 1, 0.0f);
    lastPhase_.resize(FFT_SIZE / 2 + 1, 0.0f);
    expectedPhase_.resize(FFT_SIZE / 2 + 1, 0.0f);
    magnitude_.resize(FFT_SIZE / 2 + 1, 0.0f);
    phase_.resize(FFT_SIZE / 2 + 1, 0.0f);

    // FFT configs
    fftCfg_ = kiss_fft_alloc(FFT_SIZE, 0, nullptr, nullptr);
    ifftCfg_ = kiss_fft_alloc(FFT_SIZE, 1, nullptr, nullptr);
}

PitchShifter::~PitchShifter() {
    if (fftCfg_) kiss_fft_free(fftCfg_);
    if (ifftCfg_) kiss_fft_free(ifftCfg_);
}

float PitchShifter::process(float input, float pitchShift, float speedShift) {
    if (mode_ == PitchMode::RESAMPLE) {
        return processResample(input, pitchShift, speedShift);
    }
    return processIndependent(input, pitchShift);
}

float PitchShifter::processResample(float input, float pitchShift, float speedShift) {
    buffer_[writePos_] = input;
    writePos_ = (writePos_ + 1) % bufferSize_;

    float delay = 1.0f / speedShift;
    float readFrom = static_cast<float>(writePos_) - delay;
    if (readFrom < 0.0f) {
        readFrom += static_cast<float>(bufferSize_);
    }

    size_t intPos = static_cast<size_t>(readFrom) % bufferSize_;
    float frac = readFrom - static_cast<float>(intPos);
    size_t nextPos = (intPos + 1) % bufferSize_;

    return buffer_[intPos] * (1.0f - frac) + buffer_[nextPos] * frac;
}

void PitchShifter::reset() {
    std::fill(buffer_.begin(), buffer_.end(), 0.0f);
    writePos_ = 0;
    std::fill(inputBuffer_.begin(), inputBuffer_.end(), 0.0f);
    inputPos_ = 0;
    std::fill(outputBuffer_.begin(), outputBuffer_.end(), 0.0f);
    outputReadPos_ = 0;
    outputWritePos_ = 0;
    std::fill(overlapBuffer_.begin(), overlapBuffer_.end(), 0.0f);
    std::fill(phaseAccumulator_.begin(), phaseAccumulator_.end(), 0.0f);
    std::fill(lastPhase_.begin(), lastPhase_.end(), 0.0f);
    std::fill(expectedPhase_.begin(), expectedPhase_.end(), 0.0f);
}

float PitchShifter::processIndependent(float input, float pitchShift) {
    inputBuffer_[inputPos_] = input;
    inputPos_ = (inputPos_ + 1) % FFT_SIZE;

    // Check if we have a full hop to process
    if (inputPos_ % HOP_SIZE != 0) {
        // Just return from overlap buffer
        float out = outputBuffer_[outputReadPos_];
        outputReadPos_ = (outputReadPos_ + 1) % (FFT_SIZE * 2);
        return out;
    }

    // Prepare windowed input
    float windowedInput[FFT_SIZE];
    for (int i = 0; i < FFT_SIZE; i++) {
        size_t idx = (inputPos_ - FFT_SIZE + i + FFT_SIZE) % FFT_SIZE;
        windowedInput[i] = inputBuffer_[idx] * window_[i];
    }

    // FFT
    kiss_fft_cpx timeData[FFT_SIZE];
    kiss_fft_cpx freqData[FFT_SIZE / 2 + 1];

    for (int i = 0; i < FFT_SIZE; i++) {
        timeData[i].r = windowedInput[i];
        timeData[i].i = 0.0f;
    }

    kiss_fft(static_cast<kiss_fft_cfg>(fftCfg_), timeData, freqData);

    // Extract magnitude and phase
    for (int i = 0; i <= FFT_SIZE / 2; i++) {
        magnitude_[i] = std::sqrt(freqData[i].r * freqData[i].r + freqData[i].i * freqData[i].i);
        phase_[i] = std::atan2(freqData[i].i, freqData[i].r);
    }

    // Phase difference
    float ratio = std::pow(2.0f, pitchShift / 12.0f);

    // Process spectrum for pitch shifting
    kiss_fft_cpx outputFreqData[FFT_SIZE / 2 + 1];
    memset(outputFreqData, 0, sizeof(outputFreqData));

    for (int i = 0; i <= FFT_SIZE / 2; i++) {
        // Phase deviation
        float phaseDev = phase_[i] - lastPhase_[i];
        lastPhase_[i] = phase_[i];

        // Remove expected phase advance
        phaseDev -= expectedPhase_[i];
        expectedPhase_[i] = static_cast<float>(i) * 2.0f * M_PI * HOP_SIZE / FFT_SIZE;

        // Wrap phase deviation to [-pi, pi]
        while (phaseDev > M_PI) phaseDev -= 2.0f * M_PI;
        while (phaseDev < -M_PI) phaseDev += 2.0f * M_PI;

        // True frequency
        float trueFreq = static_cast<float>(i) * 2.0f * M_PI / FFT_SIZE + phaseDev / HOP_SIZE;

        // Map to new frequency bin
        int newBin = static_cast<int>(std::round(i * ratio));
        if (newBin <= FFT_SIZE / 2) {
            float mag = magnitude_[i];
            float phase = phaseAccumulator_[newBin];
            outputFreqData[newBin].r += mag * std::cos(phase);
            outputFreqData[newBin].i += mag * std::sin(phase);
            phaseAccumulator_[newBin] += trueFreq * HOP_SIZE * ratio;
        }
    }

    // IFFT
    kiss_fft_cpx outputTimeData[FFT_SIZE];
    kiss_fft(static_cast<kiss_fft_cfg>(ifftCfg_), outputFreqData, outputTimeData);

    // Extract real part and apply window
    float timeOutput[FFT_SIZE];
    for (int i = 0; i < FFT_SIZE; i++) {
        timeOutput[i] = outputTimeData[i].r / FFT_SIZE * window_[i];
    }

    // Overlap-add
    for (int i = 0; i < FFT_SIZE; i++) {
        outputBuffer_[(outputWritePos_ + i) % (FFT_SIZE * 2)] += timeOutput[i];
    }

    outputWritePos_ = (outputWritePos_ + HOP_SIZE) % (FFT_SIZE * 2);

    float out = outputBuffer_[outputReadPos_];
    outputReadPos_ = (outputReadPos_ + 1) % (FFT_SIZE * 2);

    return out;
}
