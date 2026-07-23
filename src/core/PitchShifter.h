#pragma once
#include <vector>
#include <cmath>

enum class PitchMode {
    RESAMPLE,
    INDEPENDENT
};

class PitchShifter {
public:
    explicit PitchShifter(PitchMode mode, int sampleRate = 48000);

    float process(float input, float pitchShift, float speedShift);
    void reset();

private:
    PitchMode mode_;
    int sampleRate_;

    std::vector<float> buffer_;
    size_t bufferSize_;
    size_t writePos_ = 0;

    float processResample(float input, float pitchShift, float speedShift);
    float processIndependent(float input, float pitchShift);

    static constexpr int FFT_SIZE = 2048;
    static constexpr int HOP_SIZE = 512;
    std::vector<float> inputBuffer_;
    size_t inputPos_ = 0;
    std::vector<float> outputBuffer_;
    size_t outputReadPos_ = 0;
    size_t outputWritePos_ = 0;
};
