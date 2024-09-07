#pragma once

#include <pch.h>

struct Params : chowdsp::ParamHolder
{
};

using State = chowdsp::PluginStateImpl<Params>;

class Wave_Synth : public chowdsp::PluginBase<State>
{
public:
    Wave_Synth();

    void prepareToPlay (double sample_rate, int samples_per_block) override;
    void releaseResources() override {}
    void processAudioBlock (juce::AudioBuffer<float>& buffer) override;

    juce::AudioProcessorEditor* createEditor() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Wave_Synth)
};
