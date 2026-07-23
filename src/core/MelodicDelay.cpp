#include "MelodicDelay.h"
#include <algorithm>
#include <cmath>

MelodicDelay::MelodicDelay(int sampleRate, const DelayParams& params)
    : sampleRate_(sampleRate)
    , params_(params)
    , inputBuffer_(static_cast<size_t>(sampleRate * params.tailDuration))
    , feedbackBuffer_(static_cast<size_t>(sampleRate * params.tailDuration))
    , pitchShifter_(params.pitchMode, sampleRate)
    , envelope_(params.envelopePoints.empty()
                ? std::vector<std::pair<float, float>>{{0.0f, 1.0f}, {params.tailDuration, 0.0f}}
                : params.envelopePoints)
    , repeatCount_(0) {}

float MelodicDelay::process(float input) {
    inputBuffer_.write(input);

    float delaySamples = params_.delayTime * sampleRate_ / 1000.0f;
    float delayedInput = inputBuffer_.read(delaySamples);
    float delayedFeedback = feedbackBuffer_.read(delaySamples);

    float normalizedTime = static_cast<float>(repeatCount_) / delaySamples * params_.delayTime / 1000.0f / params_.tailDuration;
    normalizedTime = std::min(normalizedTime, 1.0f);

    float processed = processRepeat(delayedInput + delayedFeedback, normalizedTime);

    float envelopeValue = envelope_.getAmplitude(normalizedTime);
    processed *= envelopeValue;

    feedbackBuffer_.write(processed * params_.feedbackGain);

    repeatCount_++;
    return input + processed * params_.mix;
}

float MelodicDelay::processRepeat(float delayed, float normalizedTime) {
    float pitch = 0.0f;
    float speed = 1.0f;
    float volume = 1.0f;
    float echoIndex = normalizedTime * params_.tailDuration / (params_.delayTime / 1000.0f);

    switch (params_.repeatMode) {
        case RepeatMode::FIXED:
            pitch = params_.pitchShift;
            speed = params_.speedShift;
            volume = params_.volumeDecay;
            break;

        case RepeatMode::CUMULATIVE:
            pitch = params_.pitchShift * echoIndex;
            speed = std::pow(params_.speedShift, echoIndex);
            volume = std::pow(params_.volumeDecay, echoIndex);
            break;

        case RepeatMode::INDEPENDENT:
            pitch = params_.pitchShift * normalizedTime * 24.0f;
            speed = params_.speedShift * (1.0f + normalizedTime);
            volume = params_.volumeDecay * (1.0f - normalizedTime);
            break;
    }

    return pitchShifter_.process(delayed, pitch, speed) * volume;
}

std::vector<float> MelodicDelay::processBlock(const std::vector<float>& input) {
    std::vector<float> output(input.size());
    for (size_t i = 0; i < input.size(); i++) {
        output[i] = process(input[i]);
    }
    return output;
}

void MelodicDelay::reset() {
    inputBuffer_.reset();
    feedbackBuffer_.reset();
    pitchShifter_.reset();
    repeatCount_ = 0;
}

void MelodicDelay::setParams(const DelayParams& params) {
    params_ = params;
    envelope_.setPoints(params.envelopePoints);
}
