#include "Wave_Synth.h"

Wave_Synth::Wave_Synth() = default;

void Wave_Synth::prepareToPlay ([[maybe_unused]] double sample_rate, [[maybe_unused]] int samples_per_block)
{
}

void Wave_Synth::processAudioBlock ([[maybe_unused]] juce::AudioBuffer<float>& buffer)
{
}

juce::AudioProcessorEditor* Wave_Synth::createEditor()
{
    return new chowdsp::ParametersViewEditor { *this, state, state.params };
}

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Wave_Synth();
}
