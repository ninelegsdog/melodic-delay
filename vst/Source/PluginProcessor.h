#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "MelodicDelay.h"

class MelodicDelayProcessor : public juce::AudioProcessor {
public:
    MelodicDelayProcessor();
    ~MelodicDelayProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Melodic Delay"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    static constexpr int NUM_PARAMS = 7;

    juce::AudioProcessorValueTreeState apvts;

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    MelodicDelay* getDelayForChannel(int channel);

    struct ChannelState {
        std::unique_ptr<MelodicDelay> delay;
        DelayParams params;
    };

    std::vector<ChannelState> channels_;
    double currentSampleRate_ = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MelodicDelayProcessor)
};
