#include "Envelope.h"
#include <algorithm>
#include <cmath>

Envelope::Envelope(const std::vector<Point>& points) : points_(points) {
    std::sort(points_.begin(), points_.end(),
              [](const Point& a, const Point& b) { return a.first < b.first; });
}

float Envelope::getAmplitude(float normalizedTime) const {
    if (points_.empty()) return 1.0f;
    if (points_.size() == 1) return points_[0].second;

    // Clamp to bounds
    if (normalizedTime <= points_.front().first) return points_.front().second;
    if (normalizedTime >= points_.back().first) return points_.back().second;

    // Find segment
    for (size_t i = 0; i < points_.size() - 1; i++) {
        if (normalizedTime >= points_[i].first &&
            normalizedTime <= points_[i + 1].first) {

            // Handle duplicate time points (step functions)
            if (points_[i].first == points_[i + 1].first) {
                return points_[i + 1].second;
            }

            // Handle flat segments (constant value)
            if (std::abs(points_[i].second - points_[i + 1].second) < 1e-6f) {
                return points_[i].second;
            }

            // Handle step boundaries (hold after duplicate time)
            if (i > 0 && points_[i - 1].first == points_[i].first) {
                return points_[i].second;
            }

            float t0 = points_[i].first;
            float t1 = points_[i + 1].first;
            float t = (normalizedTime - t0) / (t1 - t0);

            float p0 = (i > 0) ? points_[i - 1].second : points_[i].second;
            float p1 = points_[i].second;
            float p2 = points_[i + 1].second;
            float p3 = (i + 2 < points_.size()) ? points_[i + 2].second : points_[i + 1].second;

            return catmullRom(t, p0, p1, p2, p3);
        }
    }

    return points_.back().second;
}

float Envelope::catmullRom(float t, float p0, float p1, float p2, float p3) const {
    float t2 = t * t;
    float t3 = t2 * t;

    return 0.5f * (
        (2.0f * p1) +
        (-p0 + p2) * t +
        (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
        (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3
    );
}

void Envelope::setPoints(const std::vector<Point>& points) {
    points_ = points;
    std::sort(points_.begin(), points_.end(),
              [](const Point& a, const Point& b) { return a.first < b.first; });
}
