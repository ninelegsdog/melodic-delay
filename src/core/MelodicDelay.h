#pragma once
#include "RingBuffer.h"
#include "PitchShifter.h"
#include "Envelope.h"
#include <vector>
#include <utility>

enum class RepeatMode {
    CUMULATIVE,
    FIXED,
    INDEPENDENT
};

struct DelayParams {
    float tailDuration = 5.0f;
    float delayTime = 500.0f;
    float feedbackGain = 0.5f;
    float pitchShift = 0.0f;
    float speedShift = 1.0f;
    float volumeDecay = 0.7f;
    float mix = 0.5f;

    PitchMode pitchMode = PitchMode::RESAMPLE;
    RepeatMode repeatMode = RepeatMode::FIXED;

    std::vector<std::pair<float, float>> envelopePoints;
};

class MelodicDelay {
public:
    MelodicDelay(int sampleRate, const DelayParams& params);

    float process(float input);
    std::vector<float> processBlock(const std::vector<float>& input);
    void reset();
    void setParams(const DelayParams& params);

private:
    int sampleRate_;
    DelayParams params_;

    RingBuffer inputBuffer_;
    RingBuffer feedbackBuffer_;
    PitchShifter pitchShifter_;
    Envelope envelope_;

    int repeatCount_;
    float processRepeat(float delayed, float normalizedTime);
};
