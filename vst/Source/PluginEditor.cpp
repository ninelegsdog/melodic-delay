#include "PluginEditor.h"

MelodicDelayPluginEditor::MelodicDelayPluginEditor(MelodicDelayProcessor& p)
    : AudioProcessorEditor(p), processor(p)
{
    setSize(640, 420);

    auto& presets = getDefaultPresets();
    for (size_t i = 0; i < presets.size(); ++i)
        presetBox.addItem(presets[i].name, static_cast<int>(i + 1));
    presetBox.setSelectedId(1, juce::dontSendNotification);
    presetBox.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff2a2a4a));
    presetBox.setColour(juce::ComboBox::textColourId, juce::Colours::white);
    presetBox.setColour(juce::ComboBox::outlineColourId, juce::Colours::cyan.darker(0.5f));
    presetBox.onChange = [this]() {
        int idx = presetBox.getSelectedItemIndex();
        if (idx >= 0 && idx < static_cast<int>(getDefaultPresets().size()))
            applyPreset(processor.apvts, getDefaultPresets()[idx]);
    };
    addAndMakeVisible(presetBox);

    setupSlider(tailDurationSlider, tailDurationLabel, "Tail (s)",
                tailDurationAttach, "tailDuration");
    setupSlider(delayTimeSlider, delayTimeLabel, "Delay (ms)",
                delayTimeAttach, "delayTime");
    setupSlider(feedbackSlider, feedbackLabel, "Feedback",
                feedbackAttach, "feedbackGain");
    setupSlider(pitchShiftSlider, pitchShiftLabel, "Pitch (st)",
                pitchShiftAttach, "pitchShift");
    setupSlider(speedSlider, speedLabel, "Speed",
                speedAttach, "speedShift");
    setupSlider(volumeDecaySlider, volumeDecayLabel, "Decay",
                volumeDecayAttach, "volumeDecay");
    setupSlider(mixSlider, mixLabel, "Mix",
                mixAttach, "mix");
}

MelodicDelayPluginEditor::~MelodicDelayPluginEditor() {}

void MelodicDelayPluginEditor::setupSlider(
    juce::Slider& slider, juce::Label& label,
    const juce::String& labelText,
    std::unique_ptr<SliderAttachment>& attachment,
    const juce::String& paramId)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
    slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::cyan.darker(0.2f));
    slider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(slider);

    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    label.setFont(juce::Font(12.0f));
    addAndMakeVisible(label);

    attachment = std::make_unique<SliderAttachment>(processor.apvts, paramId, slider);
}

void MelodicDelayPluginEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0xff1a1a2e));

    g.setColour(juce::Colours::cyan.brighter(0.3f));
    g.setFont(juce::Font(22.0f, juce::Font::bold));
    g.drawText("MELODIC DELAY", 20, 10, getWidth() - 40, 30, juce::Justification::centred);

    g.setColour(juce::Colours::grey.darker(0.3f));
    g.setFont(juce::Font(11.0f));
    g.drawText("Preset", getWidth() - 160, 42, 55, 15, juce::Justification::centredRight);

    g.setColour(juce::Colours::cyan.darker(0.5f));
    g.drawLine(20, 42, getWidth() - 20, 42, 1.0f);

    g.drawText("Tail", 80, 55, 80, 15, juce::Justification::centred);
    g.drawText("Delay", 190, 55, 80, 15, juce::Justification::centred);
    g.drawText("Feedback", 300, 55, 80, 15, juce::Justification::centred);
    g.drawText("Pitch", 410, 55, 80, 15, juce::Justification::centred);
    g.drawText("Speed", 520, 55, 80, 15, juce::Justification::centred);

    g.drawText("Decay", 190, 260, 80, 15, juce::Justification::centred);
    g.drawText("Mix", 300, 260, 80, 15, juce::Justification::centred);
}

void MelodicDelayPluginEditor::resized() {
    presetBox.setBounds(getWidth() - 155, 42, 135, 20);

    int y1 = 70;
    int y2 = 200;
    int w = 80;
    int h = 130;
    int gap = 30;

    int x1 = 30;
    tailDurationSlider.setBounds(x1, y1, w, h);
    tailDurationLabel.setBounds(x1, y1 + h, w, 15);

    int x2 = x1 + w + gap;
    delayTimeSlider.setBounds(x2, y1, w, h);
    delayTimeLabel.setBounds(x2, y1 + h, w, 15);

    int x3 = x2 + w + gap;
    feedbackSlider.setBounds(x3, y1, w, h);
    feedbackLabel.setBounds(x3, y1 + h, w, 15);

    int x4 = x3 + w + gap;
    pitchShiftSlider.setBounds(x4, y1, w, h);
    pitchShiftLabel.setBounds(x4, y1 + h, w, 15);

    int x5 = x4 + w + gap;
    speedSlider.setBounds(x5, y1, w, h);
    speedLabel.setBounds(x5, y1 + h, w, 15);

    int x6 = 140;
    volumeDecaySlider.setBounds(x6, y2, w, h);
    volumeDecayLabel.setBounds(x6, y2 + h, w, 15);

    int x7 = x6 + w + gap;
    mixSlider.setBounds(x7, y2, w, h);
    mixLabel.setBounds(x7, y2 + h, w, 15);
}
