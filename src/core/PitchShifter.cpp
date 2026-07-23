#include "PitchShifter.h"
#include <algorithm>
#include <cmath>

PitchShifter::PitchShifter(PitchMode mode, int sampleRate)
    : mode_(mode), sampleRate_(sampleRate) {
    bufferSize_ = static_cast<size_t>(sampleRate) * 2;
    buffer_.resize(bufferSize_, 0.0f);
}

float PitchShifter::process(float input, float pitchShift, float speedShift) {
    if (mode_ == PitchMode::RESAMPLE) {
        return processResample(input, pitchShift, speedShift);
    }
    return input;
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
}
