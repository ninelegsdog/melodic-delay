#include "PluginProcessor.h"
#include "PluginEditor.h"

MelodicDelayProcessor::MelodicDelayProcessor()
    : AudioProcessor(BusesProperties()
          .withInput("Input", juce::AudioChannelSet::stereo(), true)
          .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    channels_.resize(getTotalNumInputChannels());
}

MelodicDelayProcessor::~MelodicDelayProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout MelodicDelayProcessor::createParameterLayout() {
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "tailDuration", "Tail Duration",
        juce::NormalisableRange<float>(0.1f, 30.0f, 0.1f), 5.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "delayTime", "Delay Time",
        juce::NormalisableRange<float>(1.0f, 5000.0f, 1.0f), 500.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "feedbackGain", "Feedback",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "pitchShift", "Pitch Shift",
        juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "speedShift", "Speed",
        juce::NormalisableRange<float>(0.25f, 4.0f, 0.01f), 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "volumeDecay", "Volume Decay",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.7f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "mix", "Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));

    return layout;
}

void MelodicDelayProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    currentSampleRate_ = sampleRate;

    DelayParams params;
    params.tailDuration = apvts.getRawParameterValue("tailDuration")->load();
    params.delayTime = apvts.getRawParameterValue("delayTime")->load();
    params.feedbackGain = apvts.getRawParameterValue("feedbackGain")->load();
    params.pitchShift = apvts.getRawParameterValue("pitchShift")->load();
    params.speedShift = apvts.getRawParameterValue("speedShift")->load();
    params.volumeDecay = apvts.getRawParameterValue("volumeDecay")->load();
    params.mix = apvts.getRawParameterValue("mix")->load();
    params.pitchMode = PitchMode::RESAMPLE;
    params.repeatMode = RepeatMode::CUMULATIVE;
    params.envelopePoints = {{0.0f, 1.0f}, {params.tailDuration, 0.0f}};

    for (auto& ch : channels_) {
        ch.params = params;
        ch.delay = std::make_unique<MelodicDelay>(static_cast<int>(sampleRate), params);
    }
}

void MelodicDelayProcessor::releaseResources() {}

void MelodicDelayProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) {
    juce::ScopedNoDenormals noDenormals;

    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    DelayParams params;
    params.tailDuration = apvts.getRawParameterValue("tailDuration")->load();
    params.delayTime = apvts.getRawParameterValue("delayTime")->load();
    params.feedbackGain = apvts.getRawParameterValue("feedbackGain")->load();
    params.pitchShift = apvts.getRawParameterValue("pitchShift")->load();
    params.speedShift = apvts.getRawParameterValue("speedShift")->load();
    params.volumeDecay = apvts.getRawParameterValue("volumeDecay")->load();
    params.mix = apvts.getRawParameterValue("mix")->load();
    params.pitchMode = PitchMode::RESAMPLE;
    params.repeatMode = RepeatMode::CUMULATIVE;
    params.envelopePoints = {{0.0f, 1.0f}, {params.tailDuration, 0.0f}};

    for (int ch = 0; ch < numChannels; ch++) {
        if (ch >= static_cast<int>(channels_.size())) break;

        channels_[ch].delay->setParams(params);
        float* channelData = buffer.getWritePointer(ch);
        for (int i = 0; i < numSamples; i++) {
            channelData[i] = channels_[ch].delay->process(channelData[i]);
        }
    }
}

juce::AudioProcessorEditor* MelodicDelayProcessor::createEditor() {
    return new MelodicDelayPluginEditor(*this);
}

void MelodicDelayProcessor::getStateInformation(juce::MemoryBlock& destData) {
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void MelodicDelayProcessor::setStateInformation(const void* data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(apvts.state.getType())) {
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new MelodicDelayProcessor();
}
