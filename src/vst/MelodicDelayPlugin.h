#pragma once
#include "MelodicDelay.h"
#include <string>

// VST Plugin Stub - for future JUCE integration
class MelodicDelayPlugin {
public:
    MelodicDelayPlugin(int sampleRate = 48000)
        : delay_(sampleRate, DelayParams{}) {}
    
    // Parameter management
    void setParameter(int index, float value) {
        DelayParams params = params_;
        switch (index) {
            case 0: params.tailDuration = value * 30.0f; break;
            case 1: params.delayTime = value * 5000.0f; break;
            case 2: params.feedbackGain = value; break;
            case 3: params.pitchShift = (value - 0.5f) * 48.0f; break;
            case 4: params.speedShift = 0.25f + value * 3.75f; break;
            case 5: params.volumeDecay = value; break;
            case 6: params.mix = value; break;
        }
        params_ = params;
        delay_.setParams(params_);
    }
    
    float getParameter(int index) const {
        switch (index) {
            case 0: return params_.tailDuration / 30.0f;
            case 1: return params_.delayTime / 5000.0f;
            case 2: return params_.feedbackGain;
            case 3: return (params_.pitchShift / 48.0f) + 0.5f;
            case 4: return (params_.speedShift - 0.25f) / 3.75f;
            case 5: return params_.volumeDecay;
            case 6: return params_.mix;
            default: return 0.0f;
        }
    }
    
    // Audio processing
    void process(float** inputs, float** outputs, int numSamples) {
        for (int ch = 0; ch < 2; ch++) {
            for (int i = 0; i < numSamples; i++) {
                outputs[ch][i] = delay_.process(inputs[ch][i]);
            }
        }
    }
    
    void reset() { delay_.reset(); }

private:
    MelodicDelay delay_;
    DelayParams params_;
};
