#pragma once

#include "dsp/AR_Envelope.h"
#include "dsp/DC_Blocker.h"
#include "dsp/Diode_Clipper.h"
#include "dsp/Diode_Rectifier.h"
#include "dsp/LC_Oscillator.h"
#include "dsp/Sallen_Key_Filter.h"
#include "dsp/Phaser.h"

#include <clap/ext/draft/scratch-memory.h>

enum class Waveshaper_Type
{
    Diode_Clipper = 1,
    Diode_Rectifier = 2,
};

struct Filter_Params : chowdsp::ParamHolder
{
    chowdsp::SemitonesParameter::Ptr cutoff {
        PID { "vcf_cutoff", 100 },
        "Cutoff",
        juce::NormalisableRange { 0.0f, 48.0f },
        24.0f,
    };

    chowdsp::FloatParameter::Ptr q_val {
        PID { "vcf_q", 100 },
        "Resonance",
        chowdsp::ParamUtils::createNormalisableRange (1.0f, 10.0f, 5.0f),
        5.0f,
        &chowdsp::ParamUtils::floatValToString,
        &chowdsp::ParamUtils::stringToFloatVal,
    };

    Filter_Params() : ParamHolder { "Filter" }
    {
        add (cutoff, q_val);
    }
};

struct Params : chowdsp::ParamHolder
{
    chowdsp::EnumChoiceParameter<Waveshaper_Type>::Ptr waveshaper {
        PID { "waveshaper", 100 },
        "Waveshaper",
        Waveshaper_Type::Diode_Clipper,
    };
    Diode_Clipper::Params diode_clipper_params;
    Diode_Rectifier::Params diode_rectifier_params;
    Filter_Params filter_params;
    AR_Envelope::Params env_params;
    Phaser::Params phaser_params;

    Params()
    {
        // some parameters are hidden for ease-of-use...
        add (
            // waveshaper,
             // diode_rectifier_params,
             // diode_clipper_params,
             filter_params,
             env_params,
             phaser_params);
    }
};

using State = chowdsp::PluginStateImpl<Params>;

class Wave_Synth : public chowdsp::SynthBase<State>
{
public:
    Wave_Synth();

    void prepareToPlay (double sample_rate, int samples_per_block) override;
    void releaseResources() override {}
    void processSynth (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) override;
    bool isBusesLayoutSupported (const juce::AudioProcessor::BusesLayout& layouts) const override;

    juce::AudioProcessorEditor* createEditor() override;

    void process_midi (const juce::MidiBuffer& midi) noexcept;
    void process_voices (chowdsp::ArenaAllocatorView arena, std::span<xsimd::batch<float>> voice_buffer) noexcept;

    static constexpr int os_ratio = 4;
    static constexpr int num_voices = 4;

    size_t arena_bytes_required = 0;
    clap_host_scratch_memory* host_scratch_memory {};
    std::vector<std::byte> internal_arena_memory {};
    // chowdsp::ArenaAllocator<> arena;

    using AA_Filter = chowdsp::EllipticFilter<8>;
    chowdsp::Downsampler<float, AA_Filter, false> downsampler;

    struct Voice_Info
    {
        int32_t note_number = 69;
        bool active = false;
    };
    std::array<Voice_Info, num_voices> voice_info {};
    size_t next_voice_index {};
    float pitch_bend_factor = 1.0f;

    LC_Oscillator sine_oscillator;
    Diode_Clipper diode_clipper { state.params.diode_clipper_params };
    Diode_Rectifier diode_rectifier { state.params.diode_rectifier_params };
    Sallen_Key_Filter sallen_key_filter;
    AR_Envelope envelope { state.params.env_params };
    Phaser phaser { state.params.phaser_params };
    DC_Blocker dc_blocker;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Wave_Synth)
};
