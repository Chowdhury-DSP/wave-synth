#include "Plugin_Editor.h"

namespace
{
constexpr int schematic_width = 740;
constexpr int width = schematic_width + 400;
constexpr int height = 380;
} // namespace

Plugin_Editor::Plugin_Editor (Wave_Synth& plugin)
    : juce::AudioProcessorEditor { plugin },
      params { plugin }
{
    addAndMakeVisible (params);

    const auto fs = cmrc::gui::get_filesystem();
    const auto schematic_file = fs.open ("circuit.png");
    schematic = juce::ImageCache::getFromMemory (schematic_file.begin(), static_cast<int> (schematic_file.size()));

    setSize (width, height);
}

void Plugin_Editor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);

    auto b = getLocalBounds();
    g.drawImage (schematic, b.removeFromRight (schematic_width).toFloat(), juce::RectanglePlacement::stretchToFit);

    static constexpr float dashes[] = { 5.0f, 5.0f };
    g.setColour (juce::Colours::black);

    static constexpr float osc_x = 550.0f;
    static constexpr float env_y = 225.0f;
    static constexpr float filter_x = 775.0f;

    g.drawDashedLine (juce::Line { osc_x, 0.0f, osc_x, static_cast<float> (height) },
                      dashes,
                      2,
                      1.5f);
    g.drawDashedLine (juce::Line { osc_x,
                                   env_y,
                                   static_cast<float> (width),
                                   env_y, },
                      dashes,
                      2,
                      1.5f);
    g.drawDashedLine (juce::Line { filter_x, 0.0f, filter_x, env_y },
                      dashes,
                      2,
                      1.5f);

    g.setFont (juce::Font { 24.0f }.boldened());
    g.drawFittedText ("OSCILLATOR", 405, 0, 150, 30, juce::Justification::left, 1);
    g.drawFittedText ("WAVESHAPER", 560, 0, 150, 30, juce::Justification::left, 1);
    g.drawFittedText ("FILTER", 785, 0, 150, 30, juce::Justification::left, 1);
    g.drawFittedText ("ENVELOPE GENERATOR", 880, 300, 250, 30, juce::Justification::left, 1);
}

void Plugin_Editor::resized()
{
    auto b = getLocalBounds();
    params.setBounds (b.removeFromLeft (width - schematic_width));
}
