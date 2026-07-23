#include "RingBuffer.h"
#include <algorithm>
#include <cmath>

RingBuffer::RingBuffer(size_t capacity)
    : buffer_(capacity, 0.0f), capacity_(capacity) {}

void RingBuffer::write(float sample) {
    buffer_[writePos_] = sample;
    writePos_ = (writePos_ + 1) % capacity_;
}

float RingBuffer::read(float delaySamples) const {
    if (delaySamples < 0) delaySamples = 0;
    if (delaySamples >= capacity_) delaySamples = capacity_ - 1;

    float readPos = static_cast<float>(writePos_) - delaySamples - 1;
    while (readPos < 0) readPos += capacity_;

    size_t intPos = static_cast<size_t>(readPos) % capacity_;
    float fracPos = readPos - static_cast<size_t>(readPos);

    if (fracPos < 1e-6f) return buffer_[intPos];

    return interpolateLagrange(intPos, fracPos);
}

float RingBuffer::interpolateLagrange(size_t pos, float frac) const {
    auto at = [&](int offset) -> float {
        size_t idx = (pos + offset + capacity_) % capacity_;
        return buffer_[idx];
    };

    float p0 = at(-1);
    float p1 = at(0);
    float p2 = at(1);
    float p3 = at(2);

    float t = frac;
    float t2 = t * t;

    return (-p0 * t * (1-t) * (1-t) + p1 * (1-t) * (1-t) * (2+t)
            + p2 * t * (2-3*t+t2) - p3 * t2 * (1-t)) * 0.5f +
           (p1 + p2) * 0.5f;
}

void RingBuffer::reset() {
    std::fill(buffer_.begin(), buffer_.end(), 0.0f);
    writePos_ = 0;
}

size_t RingBuffer::getCapacity() const {
    return capacity_;
}
