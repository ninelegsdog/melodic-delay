#include <gtest/gtest.h>
#include "RingBuffer.h"

TEST(RingBuffer, InitialState) {
    RingBuffer buf(1024);
    EXPECT_EQ(buf.getCapacity(), 1024u);
}

TEST(RingBuffer, WriteReadImmediate) {
    RingBuffer buf(1024);
    buf.write(0.5f);
    float read = buf.read(0);
    EXPECT_FLOAT_EQ(read, 0.5f);
}

TEST(RingBuffer, WriteReadDelayed) {
    RingBuffer buf(1024);
    buf.write(1.0f);
    float read = buf.read(0);
    EXPECT_FLOAT_EQ(read, 1.0f);
}

TEST(RingBuffer, OverwriteOldest) {
    RingBuffer buf(4);
    buf.write(1.0f);
    buf.write(2.0f);
    buf.write(3.0f);
    buf.write(4.0f);
    buf.write(5.0f);
    float read = buf.read(0);
    EXPECT_FLOAT_EQ(read, 5.0f);
}

TEST(RingBuffer, ResetClears) {
    RingBuffer buf(1024);
    buf.write(1.0f);
    buf.reset();
    float read = buf.read(0);
    EXPECT_FLOAT_EQ(read, 0.0f);
}

TEST(RingBuffer, ReadWithInterpolation) {
    RingBuffer buf(1024);
    buf.write(0.0f);
    buf.write(1.0f);
    buf.write(0.0f);
    float read = buf.read(0.5f);
    EXPECT_GT(read, 0.0f);
    EXPECT_LT(read, 1.0f);
}
