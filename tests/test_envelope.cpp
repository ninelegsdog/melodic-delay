#include <gtest/gtest.h>
#include "Envelope.h"

TEST(Envelope, LinearFade) {
    Envelope env({{0.0f, 1.0f}, {1.0f, 0.0f}});
    EXPECT_NEAR(env.getAmplitude(0.0f), 1.0f, 0.01f);
    EXPECT_NEAR(env.getAmplitude(0.5f), 0.5f, 0.01f);
    EXPECT_NEAR(env.getAmplitude(1.0f), 0.0f, 0.01f);
}

TEST(Envelope, ExponentialDecay) {
    Envelope env({{0.0f, 1.0f}, {0.5f, 0.3f}, {1.0f, 0.0f}});
    EXPECT_NEAR(env.getAmplitude(0.0f), 1.0f, 0.01f);
    EXPECT_NEAR(env.getAmplitude(0.5f), 0.3f, 0.01f);
    EXPECT_NEAR(env.getAmplitude(1.0f), 0.0f, 0.01f);
}

TEST(Envelope, SCurve) {
    Envelope env({{0.0f, 0.0f}, {0.3f, 0.1f}, {0.7f, 0.9f}, {1.0f, 1.0f}});
    EXPECT_NEAR(env.getAmplitude(0.0f), 0.0f, 0.01f);
    EXPECT_NEAR(env.getAmplitude(0.5f), 0.5f, 0.1f);  // Middle should be ~0.5
    EXPECT_NEAR(env.getAmplitude(1.0f), 1.0f, 0.01f);
}

TEST(Envelope, Staircase) {
    Envelope env({
        {0.0f, 1.0f}, {0.25f, 1.0f}, {0.25f, 0.5f},
        {0.5f, 0.5f}, {0.5f, 0.25f}, {1.0f, 0.0f}
    });
    EXPECT_NEAR(env.getAmplitude(0.0f), 1.0f, 0.01f);
    EXPECT_NEAR(env.getAmplitude(0.1f), 1.0f, 0.01f);
    EXPECT_NEAR(env.getAmplitude(0.4f), 0.5f, 0.01f);
    EXPECT_NEAR(env.getAmplitude(0.8f), 0.25f, 0.01f);
}

TEST(Envelope, SetPoints) {
    Envelope env({{0.0f, 1.0f}, {1.0f, 0.0f}});
    env.setPoints({{0.0f, 0.0f}, {1.0f, 1.0f}});
    EXPECT_NEAR(env.getAmplitude(0.5f), 0.5f, 0.01f);
}

TEST(Envelope, OutOfBoundsClamp) {
    Envelope env({{0.0f, 1.0f}, {1.0f, 0.0f}});
    EXPECT_NEAR(env.getAmplitude(-0.5f), 1.0f, 0.01f);
    EXPECT_NEAR(env.getAmplitude(1.5f), 0.0f, 0.01f);
}
