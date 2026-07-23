#pragma once
#include <vector>
#include <utility>

class Envelope {
public:
    using Point = std::pair<float, float>;

    explicit Envelope(const std::vector<Point>& points);

    float getAmplitude(float normalizedTime) const;
    void setPoints(const std::vector<Point>& points);

private:
    std::vector<Point> points_;

    float catmullRom(float t, float p0, float p1, float p2, float p3) const;
};
