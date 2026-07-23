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

TEST(MelodicDelay, CumulativePitchRise) {
    DelayParams params;
    params.tailDuration = 1.0f;
    params.delayTime = 100.0f;
    params.feedbackGain = 0.6f;
    params.pitchShift = 2.0f; // +2 semitones per repeat
    params.speedShift = 1.0f;
    params.volumeDecay = 0.8f;
    params.pitchMode = PitchMode::RESAMPLE;
    params.repeatMode = RepeatMode::CUMULATIVE;
    params.envelopePoints = {{0.0f, 1.0f}, {1.0f, 0.0f}};

    MelodicDelay delay(48000, params);

    std::vector<float> input(48000, 0.0f);
    input[0] = 1.0f;

    std::vector<float> output = delay.processBlock(input);

    // Verify output is non-zero
    float maxOutput = *std::max_element(output.begin(), output.end());
    EXPECT_GT(maxOutput, 0.0f);
}

TEST(MelodicDelay, IndependentMode) {
    DelayParams params;
    params.tailDuration = 1.0f;
    params.delayTime = 100.0f;
    params.feedbackGain = 0.5f;
    params.pitchShift = 3.0f;
    params.speedShift = 1.0f;
    params.volumeDecay = 0.7f;
    params.pitchMode = PitchMode::RESAMPLE;
    params.repeatMode = RepeatMode::INDEPENDENT;
    params.envelopePoints = {{0.0f, 1.0f}, {1.0f, 0.0f}};

    MelodicDelay delay(48000, params);

    std::vector<float> input(48000, 0.0f);
    input[0] = 1.0f;

    std::vector<float> output = delay.processBlock(input);

    // Verify output is non-zero
    float maxOutput = *std::max_element(output.begin(), output.end());
    EXPECT_GT(maxOutput, 0.0f);
}

TEST(MelodicDelay, CustomEnvelope) {
    DelayParams params;
    params.tailDuration = 1.0f;
    params.delayTime = 100.0f;
    params.feedbackGain = 0.5f;
    params.pitchShift = 0.0f;
    params.speedShift = 1.0f;
    params.volumeDecay = 0.7f;
    params.pitchMode = PitchMode::RESAMPLE;
    params.repeatMode = RepeatMode::FIXED;
    params.envelopePoints = {{0.0f, 1.0f}, {0.5f, 0.3f}, {1.0f, 0.0f}};

    MelodicDelay delay(48000, params);

    std::vector<float> input(48000, 0.0f);
    input[0] = 1.0f;

    std::vector<float> output = delay.processBlock(input);

    // Verify output is non-zero
    float maxOutput = *std::max_element(output.begin(), output.end());
    EXPECT_GT(maxOutput, 0.0f);
}

TEST(MelodicDelay, LongTail) {
    DelayParams params;
    params.tailDuration = 10.0f;
    params.delayTime = 500.0f;
    params.feedbackGain = 0.7f;
    params.pitchShift = 0.0f;
    params.speedShift = 1.0f;
    params.volumeDecay = 0.8f;
    params.pitchMode = PitchMode::RESAMPLE;
    params.repeatMode = RepeatMode::FIXED;
    params.envelopePoints = {{0.0f, 1.0f}, {10.0f, 0.0f}};
    
    MelodicDelay delay(48000, params);
    
    // Process 1 second of impulse
    std::vector<float> input(48000, 0.0f);
    input[0] = 1.0f;
    delay.processBlock(input);
    
    // Process 10 more seconds of silence to let tail fade
    std::vector<float> silence(480000, 0.0f);
    std::vector<float> output = delay.processBlock(silence);
    
    // Check that tail decays
    float maxFirst = *std::max_element(output.begin(), output.begin() + 48000);
    float maxLast = *std::max_element(output.begin() + 432000, output.end());
    
    EXPECT_GT(maxFirst, maxLast);
}

TEST(MelodicDelay, PitchShiftCumulative) {
    DelayParams params;
    params.tailDuration = 1.0f;
    params.delayTime = 100.0f;
    params.feedbackGain = 0.5f;
    params.pitchShift = 7.0f; // Perfect fifth
    params.speedShift = 1.0f;
    params.volumeDecay = 0.7f;
    params.pitchMode = PitchMode::RESAMPLE;
    params.repeatMode = RepeatMode::CUMULATIVE;
    params.envelopePoints = {{0.0f, 1.0f}, {1.0f, 0.0f}};
    
    MelodicDelay delay(48000, params);
    
    std::vector<float> input(48000, 0.0f);
    input[0] = 1.0f;
    
    std::vector<float> output = delay.processBlock(input);
    
    // Verify output is non-zero
    float maxOutput = *std::max_element(output.begin(), output.end());
    EXPECT_GT(maxOutput, 0.0f);
}
