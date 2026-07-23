#pragma once
#include <vector>
#include <cmath>
#include <complex>

enum class PitchMode {
    RESAMPLE,
    INDEPENDENT
};

class PitchShifter {
public:
    explicit PitchShifter(PitchMode mode, int sampleRate = 48000);
    ~PitchShifter();

    float process(float input, float pitchShift, float speedShift);
    void reset();

private:
    PitchMode mode_;
    int sampleRate_;

    // Resample mode
    std::vector<float> buffer_;
    size_t bufferSize_;
    size_t writePos_ = 0;

    float processResample(float input, float pitchShift, float speedShift);

    // Phase vocoder mode (independent)
    static constexpr int FFT_SIZE = 2048;
    static constexpr int HOP_SIZE = 512;
    static constexpr int WINDOW_SIZE = FFT_SIZE;

    std::vector<float> inputBuffer_;
    size_t inputPos_ = 0;

    std::vector<float> outputBuffer_;
    size_t outputReadPos_ = 0;
    size_t outputWritePos_ = 0;

    // FFT buffers
    std::vector<std::complex<float>> fftBuffer_;
    std::vector<std::complex<float>> fftOutput_;
    std::vector<float> window_;
    std::vector<float> overlapBuffer_;

    // Phase tracking
    std::vector<float> phaseAccumulator_;
    std::vector<float> lastPhase_;
    std::vector<float> expectedPhase_;
    std::vector<float> magnitude_;
    std::vector<float> phase_;

    void* fftCfg_;
    void* ifftCfg_;

    float processIndependent(float input, float pitchShift);

    void applyWindow(std::vector<float>& buffer);
    void computeFFT(const float* input, std::complex<float>* output);
    void computeIFFT(const std::complex<float>* input, float* output);
};
