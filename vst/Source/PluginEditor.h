#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "Presets.h"

class MelodicDelayPluginEditor : public juce::AudioProcessorEditor {
public:
    MelodicDelayPluginEditor(MelodicDelayProcessor&);
    ~MelodicDelayPluginEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    MelodicDelayProcessor& processor;

    juce::ComboBox presetBox;
    juce::Slider tailDurationSlider, delayTimeSlider, feedbackSlider;
    juce::Slider pitchShiftSlider, speedSlider, volumeDecaySlider, mixSlider;

    juce::Label tailDurationLabel, delayTimeLabel, feedbackLabel;
    juce::Label pitchShiftLabel, speedLabel, volumeDecayLabel, mixLabel;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<SliderAttachment> tailDurationAttach, delayTimeAttach, feedbackAttach;
    std::unique_ptr<SliderAttachment> pitchShiftAttach, speedAttach, volumeDecayAttach, mixAttach;

    void setupSlider(juce::Slider& slider, juce::Label& label,
                     const juce::String& labelText,
                     std::unique_ptr<SliderAttachment>& attachment,
                     const juce::String& paramId);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MelodicDelayPluginEditor)
};
