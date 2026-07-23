#include "PitchShifter.h"
#include <algorithm>
#include <cmath>

PitchShifter::PitchShifter(PitchMode mode, int sampleRate)
    : mode_(mode), sampleRate_(sampleRate) {
    bufferSize_ = static_cast<size_t>(sampleRate) * 2;
    buffer_.resize(bufferSize_, 0.0f);
    inputBuffer_.resize(FFT_SIZE, 0.0f);
    outputBuffer_.resize(FFT_SIZE * 2, 0.0f);
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
}

float PitchShifter::processIndependent(float input, float pitchShift) {
    inputBuffer_[inputPos_] = input;
    inputPos_ = (inputPos_ + 1) % FFT_SIZE;

    float ratio = std::pow(2.0f, pitchShift / 12.0f);

    float readPos = 0.0f;
    float result = 0.0f;

    for (int i = 0; i < HOP_SIZE; i++) {
        readPos += ratio;
        if (readPos >= FFT_SIZE) readPos -= FFT_SIZE;

        size_t intPos = static_cast<size_t>(readPos) % FFT_SIZE;
        float frac = readPos - static_cast<size_t>(readPos);
        size_t nextPos = (intPos + 1) % FFT_SIZE;

        result = inputBuffer_[intPos] * (1.0f - frac) + inputBuffer_[nextPos] * frac;
    }

    outputBuffer_[outputWritePos_] = result;
    outputWritePos_ = (outputWritePos_ + 1) % (FFT_SIZE * 2);

    float out = outputBuffer_[outputReadPos_];
    outputReadPos_ = (outputReadPos_ + 1) % (FFT_SIZE * 2);

    return out;
}
