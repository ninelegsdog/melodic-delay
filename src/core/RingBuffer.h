#pragma once
#include <vector>
#include <cstddef>

class RingBuffer {
public:
    explicit RingBuffer(size_t capacity);

    void write(float sample);
    float read(float delaySamples) const;
    void reset();
    size_t getCapacity() const;

private:
    std::vector<float> buffer_;
    size_t writePos_ = 0;
    size_t capacity_;

    float interpolateLagrange(size_t pos, float frac) const;
};
