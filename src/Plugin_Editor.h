#pragma once

#include "Wave_Synth.h"

struct Plugin_Editor : juce::AudioProcessorEditor
{
    explicit Plugin_Editor (Wave_Synth& plugin);

    void paint(juce::Graphics &g) override;
    void resized() override;

    chowdsp::ParametersViewEditor params;
    juce::Image synth_schematic;
    juce::Image phaser_schematic;
};
