#include "Wave_Synth.h"
#include "Plugin_Editor.h"

Wave_Synth::Wave_Synth() = default;

void Wave_Synth::prepareToPlay (double sample_rate, int samples_per_block)
{
    const auto spec = juce::dsp::ProcessSpec {
        sample_rate * os_ratio,
        static_cast<uint32_t> (samples_per_block * os_ratio),
        2,
    };
    downsampler.prepare (spec, os_ratio);

    sine_oscillator.prepare (spec.sampleRate);
    diode_clipper.prepare (spec.sampleRate);
    diode_rectifier.prepare (spec.sampleRate);
    sallen_key_filter.prepare (spec.sampleRate);
    envelope.prepare (spec.sampleRate);
    phaser.prepare (spec.sampleRate);
    dc_blocker.prepare (spec.sampleRate);

    const auto arena_bytes_required = spec.maximumBlockSize * num_voices * 2 * sizeof (float)
                                      + spec.maximumBlockSize * 2 * sizeof (float);
    arena.reset (arena_bytes_required + 128);
}

void Wave_Synth::processSynth (juce::AudioBuffer<float>& output_buffer, juce::MidiBuffer& midi)
{
    // set up buffer
    jassert (output_buffer.getNumChannels() == 2); // requires stereo output!
    const auto os_num_samples = output_buffer.getNumSamples() * os_ratio;
    auto voices_buffer = chowdsp::arena::make_span<float> (arena, os_num_samples * num_voices, 16);
    auto voices_simd_buffer = std::span { reinterpret_cast<xsimd::batch<float>*> (voices_buffer.data()), static_cast<size_t> (os_num_samples) };

    // synth processing
    process_midi (midi);
    process_voices (voices_simd_buffer);

    // sum all the voices down to a mono buffer
    for (size_t n = 0; n < static_cast<size_t> (os_num_samples); ++n)
        voices_buffer[n] = xsimd::reduce_add (voices_simd_buffer[n]);

    // post-processing
    const auto mono_buffer = voices_buffer.subspan (0, static_cast<size_t> (os_num_samples));
    dc_blocker.process (mono_buffer);
    auto stereo_buffer = chowdsp::make_temp_buffer<float> (arena, 2, os_num_samples);
    phaser.process (mono_buffer,
                    stereo_buffer.getWriteSpan (0),
                    stereo_buffer.getWriteSpan (1));
    chowdsp::BufferMath::applyFunctionSIMD (stereo_buffer,
                                            [] (auto x)
                                            { return chowdsp::Math::algebraicSigmoid (0.5f * x); });

    // downsample into the output buffer
    downsampler.process (stereo_buffer, output_buffer);

    // cleanup
    chowdsp::BufferMath::sanitizeBuffer (output_buffer, 5.0f);
    arena.clear();
}

void Wave_Synth::process_midi (const juce::MidiBuffer& midi) noexcept
{
    for (auto midi_event : midi)
    {
        auto midi_message = midi_event.getMessage();
        if (midi_message.isNoteOn())
        {
            auto& new_info = voice_info[next_voice_index++];
            if (next_voice_index >= num_voices)
                next_voice_index = 0;
            new_info = {
                .note_number = midi_message.getNoteNumber(),
                .active = true,
            };
        }
        else if (midi_message.isNoteOff())
        {
            for (auto& info : voice_info)
            {
                if (info.note_number == midi_message.getNoteNumber())
                    info.active = false;
            }
        }
        else if (midi_message.isPitchWheel())
        {
            const auto pitch_wheel_pos = midi_message.getPitchWheelValue();
            const auto wheel_0_1 = static_cast<float> (pitch_wheel_pos) / static_cast<float> (0x3fff);
            jassert (wheel_0_1 >= 0.0f && wheel_0_1 <= 1.0f);
            pitch_bend_factor = std::exp2 (wheel_0_1 * 2.0f - 1.0f); // +/- 1 octave
        }
    }
}

void Wave_Synth::process_voices (std::span<xsimd::batch<float>> voice_buffer) noexcept
{
    auto envelope_buffer = chowdsp::arena::make_span<xsimd::batch<float>> (arena, voice_buffer.size());
    auto envelope_masks = std::span { reinterpret_cast<std::array<float, 4>*> (envelope_buffer.data()), envelope_buffer.size() };
    std::fill (envelope_buffer.begin(), envelope_buffer.end(), xsimd::batch<float> {});

    const auto midi_note_to_hertz = [this] (float key)
    {
        return 440.0f * pitch_bend_factor * std::exp2 ((key - 69.0f) * (1.0f / 12.0f));
    };

    const auto filter_cutoff_semitones = state.params.filter_params.cutoff->getCurrentValue();
    for (const auto [idx, info] : chowdsp::enumerate (voice_info))
    {
        if (info.active)
        {
            sine_oscillator.frequencies[idx] = midi_note_to_hertz (static_cast<float> (info.note_number));
            sallen_key_filter.cutoff_hz[idx] = midi_note_to_hertz (static_cast<float> (info.note_number) + filter_cutoff_semitones);
            for (auto& m : envelope_masks)
                m[idx] = 1.0f;
        }
    }

    sine_oscillator.process (voice_buffer);

    const auto waveshaper_type = state.params.waveshaper->get();
    if (waveshaper_type == Waveshaper_Type::Diode_Clipper)
        diode_clipper.process (voice_buffer);
    else if (waveshaper_type == Waveshaper_Type::Diode_Rectifier)
        diode_rectifier.process (voice_buffer);

    sallen_key_filter.q_val = state.params.filter_params.q_val->getCurrentValue();
    sallen_key_filter.process (voice_buffer);

    envelope.process (envelope_buffer);
    for (auto [x, env] : chowdsp::zip (voice_buffer, envelope_buffer))
        x *= env;
}

bool Wave_Synth::isBusesLayoutSupported (const juce::AudioProcessor::BusesLayout& layouts) const
{
    // only supports stereo (for now)
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // input and output layout must be the same
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

juce::AudioProcessorEditor* Wave_Synth::createEditor()
{
    return new Plugin_Editor { *this };
}

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Wave_Synth();
}
