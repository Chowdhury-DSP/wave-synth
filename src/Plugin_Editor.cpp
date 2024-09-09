#include "Plugin_Editor.h"

namespace
{
constexpr int schematic_width = 740;
constexpr int params_width = 400;
constexpr int width = schematic_width + params_width;
constexpr int synth_schem_height = 380;
constexpr int phaser_schem_height = 175;
constexpr int height = synth_schem_height + phaser_schem_height;
} // namespace

Plugin_Editor::Plugin_Editor (Wave_Synth& plugin)
    : juce::AudioProcessorEditor { plugin },
      params { plugin }
{
    addAndMakeVisible (params);

    const auto fs = cmrc::gui::get_filesystem();
    const auto synth_schematic_file = fs.open ("synth.png");
    synth_schematic = juce::ImageCache::getFromMemory (synth_schematic_file.begin(), static_cast<int> (synth_schematic_file.size()));

    const auto phaser_schematic_file = fs.open ("phaser.png");
    phaser_schematic = juce::ImageCache::getFromMemory (phaser_schematic_file.begin(), static_cast<int> (phaser_schematic_file.size()));

    setSize (width, height);
}

void Plugin_Editor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);

    auto b = getLocalBounds();
    auto schem_area = b.removeFromRight (schematic_width);
    g.drawImage (synth_schematic, schem_area.removeFromTop (synth_schem_height).toFloat(), juce::RectanglePlacement::stretchToFit);
    g.drawImage (phaser_schematic, schem_area.removeFromTop (phaser_schem_height).toFloat(), juce::RectanglePlacement::stretchToFit);

    static constexpr float dashes[] = { 5.0f, 5.0f };
    g.setColour (juce::Colours::black);

    static constexpr float osc_x = 550.0f;
    static constexpr float env_y = 225.0f;
    static constexpr float filter_x = 775.0f;

    g.drawDashedLine (juce::Line { osc_x, 0.0f, osc_x, static_cast<float> (synth_schem_height) },
                      dashes,
                      2,
                      1.5f);
    g.drawDashedLine (juce::Line {
                          osc_x,
                          env_y,
                          static_cast<float> (width),
                          env_y,
                      },
                      dashes,
                      2,
                      1.5f);
    g.drawDashedLine (juce::Line { filter_x, 0.0f, filter_x, env_y },
                      dashes,
                      2,
                      1.5f);
    g.drawDashedLine (juce::Line { static_cast<float> (params_width),
                                   static_cast<float> (synth_schem_height),
                                   static_cast<float> (width),
                                   static_cast<float> (synth_schem_height) },
                      dashes,
                      2,
                      1.5f);

    g.setFont (juce::Font { 24.0f }.boldened());
    g.drawFittedText ("OSCILLATOR", 405, 0, 150, 30, juce::Justification::left, 1);
    g.drawFittedText ("WAVESHAPER", 560, 0, 150, 30, juce::Justification::left, 1);
    g.drawFittedText ("FILTER", 785, 0, 150, 30, juce::Justification::left, 1);
    g.drawFittedText ("ENVELOPE GENERATOR", 880, 300, 250, 30, juce::Justification::left, 1);
    g.drawFittedText ("PHASER", 405, 385, 150, 30, juce::Justification::left, 1);
}

void Plugin_Editor::resized()
{
    auto b = getLocalBounds();
    params.setBounds (b.removeFromLeft (params_width));
}
