#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <vector>
#include <string>

struct MelodicDelayPreset {
    std::string name;
    float tailDuration;
    float delayTime;
    float feedbackGain;
    float pitchShift;
    float speedShift;
    float volumeDecay;
    float mix;
};

static const std::vector<MelodicDelayPreset>& getDefaultPresets() {
    static const std::vector<MelodicDelayPreset> presets = {
        {"Default",         5.0f,  500.0f, 0.50f,  0.0f, 1.00f, 0.70f, 0.50f},
        {"Dreamy",         15.0f,  800.0f, 0.65f,  3.0f, 1.00f, 0.60f, 0.55f},
        {"Shimmer",        12.0f,  600.0f, 0.60f, 12.0f, 1.00f, 0.55f, 0.50f},
        {"Melodic Up",      8.0f,  400.0f, 0.55f,  5.0f, 1.00f, 0.65f, 0.50f},
        {"Melodic Down",    8.0f,  400.0f, 0.55f, -5.0f, 1.00f, 0.65f, 0.50f},
        {"Cascade",         6.0f,  200.0f, 0.70f,  0.0f, 1.50f, 0.80f, 0.45f},
    };
    return presets;
}

inline void applyPreset(juce::AudioProcessorValueTreeState& apvts,
                        const MelodicDelayPreset& preset) {
    if (auto* p = apvts.getParameter("tailDuration"))
        p->setValueNotifyingHost(p->convertTo0to1(preset.tailDuration));
    if (auto* p = apvts.getParameter("delayTime"))
        p->setValueNotifyingHost(p->convertTo0to1(preset.delayTime));
    if (auto* p = apvts.getParameter("feedbackGain"))
        p->setValueNotifyingHost(p->convertTo0to1(preset.feedbackGain));
    if (auto* p = apvts.getParameter("pitchShift"))
        p->setValueNotifyingHost(p->convertTo0to1(preset.pitchShift));
    if (auto* p = apvts.getParameter("speedShift"))
        p->setValueNotifyingHost(p->convertTo0to1(preset.speedShift));
    if (auto* p = apvts.getParameter("volumeDecay"))
        p->setValueNotifyingHost(p->convertTo0to1(preset.volumeDecay));
    if (auto* p = apvts.getParameter("mix"))
        p->setValueNotifyingHost(p->convertTo0to1(preset.mix));
}
