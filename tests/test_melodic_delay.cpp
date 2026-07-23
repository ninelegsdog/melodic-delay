#include <gtest/gtest.h>
#include <vector>
#include <algorithm>
#include "MelodicDelay.h"

TEST(MelodicDelay, ImpulseEcho) {
    DelayParams params;
    params.tailDuration = 1.0f;
    params.delayTime = 100.0f;
    params.feedbackGain = 0.5f;
    params.pitchShift = 0.0f;
    params.speedShift = 1.0f;
    params.volumeDecay = 0.7f;
    params.mix = 1.0f;
    params.pitchMode = PitchMode::RESAMPLE;
    params.repeatMode = RepeatMode::FIXED;
    params.envelopePoints = {{0.0f, 1.0f}, {1.0f, 0.0f}};

    MelodicDelay delay(48000, params);

    std::vector<float> input(48000, 0.0f);
    input[0] = 1.0f;

    std::vector<float> output = delay.processBlock(input);

    float maxInSecond = *std::max_element(output.begin() + 4800, output.begin() + 9600);
    EXPECT_GT(maxInSecond, 0.0f);
}

TEST(MelodicDelay, FeedbackDecay) {
    DelayParams params;
    params.tailDuration = 1.0f;
    params.delayTime = 100.0f;
    params.feedbackGain = 0.5f;
    params.pitchShift = 0.0f;
    params.speedShift = 1.0f;
    params.volumeDecay = 0.7f;
    params.mix = 1.0f;
    params.pitchMode = PitchMode::RESAMPLE;
    params.repeatMode = RepeatMode::FIXED;
    params.envelopePoints = {{0.0f, 1.0f}, {1.0f, 0.0f}};

    MelodicDelay delay(48000, params);

    std::vector<float> input(48000, 0.0f);
    input[0] = 1.0f;

    std::vector<float> output = delay.processBlock(input);

    float peak1 = *std::max_element(output.begin() + 4800, output.begin() + 9600);
    float peak2 = *std::max_element(output.begin() + 9600, output.begin() + 14400);

    EXPECT_GT(peak1, peak2);
}

TEST(MelodicDelay, DryWetMix) {
    DelayParams params;
    params.tailDuration = 0.1f;
    params.delayTime = 50.0f;
    params.feedbackGain = 0.0f;
    params.pitchShift = 0.0f;
    params.speedShift = 1.0f;
    params.volumeDecay = 1.0f;
    params.mix = 0.5f;
    params.pitchMode = PitchMode::RESAMPLE;
    params.repeatMode = RepeatMode::FIXED;
    params.envelopePoints = {{0.0f, 1.0f}, {0.1f, 0.0f}};

    MelodicDelay delay(48000, params);

    std::vector<float> input(4800, 0.0f);
    input[0] = 1.0f;

    std::vector<float> output = delay.processBlock(input);

    EXPECT_NEAR(output[0], 1.0f, 0.1f);
}
