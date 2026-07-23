#include <gtest/gtest.h>
#include "PitchShifter.h"

TEST(PitchShifter, ResampleNoShift) {
    PitchShifter ps(PitchMode::RESAMPLE);
    float output = ps.process(1.0f, 0.0f, 1.0f);
    EXPECT_NEAR(output, 1.0f, 0.1f);
}

TEST(PitchShifter, ResamplePitchUp) {
    PitchShifter ps(PitchMode::RESAMPLE);
    float output = ps.process(1.0f, 12.0f, 2.0f);
    EXPECT_GT(std::abs(output), 0.0f);
}

TEST(PitchShifter, ResampleSpeedChange) {
    PitchShifter ps(PitchMode::RESAMPLE);
    float out1 = ps.process(1.0f, 0.0f, 1.0f);
    PitchShifter ps2(PitchMode::RESAMPLE);
    float out2 = ps2.process(1.0f, 0.0f, 0.5f);
    EXPECT_NE(out1, out2);
}

TEST(PitchShifter, Reset) {
    PitchShifter ps(PitchMode::RESAMPLE);
    ps.process(1.0f, 12.0f, 2.0f);
    ps.reset();
    float output = ps.process(1.0f, 0.0f, 1.0f);
    EXPECT_NEAR(output, 1.0f, 0.1f);
}
