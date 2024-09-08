#pragma once

#include <pch.h>

/** Based on "The Voss algorithm" http://www.firstpr.com.au/dsp/pink-noise/ */
template <size_t QUALITY = 8>
struct Pink_Noise
{
    int frame = -1;
    std::array<xsimd::batch<float>, QUALITY> values {};
    chowdsp::RandomFloat<xsimd::batch<float>> rng { { 0x1234, 0x4567, 0x6789, 0x1212 } };

    void reset() noexcept
    {
        frame = -1;
        values.fill (0.0f);
    }

    /** Generates pink noise (-3dB / octave) */
    inline auto operator()() noexcept
    {
        int lastFrame = frame;
        frame++;
        if (frame >= (1 << QUALITY))
            frame = 0;
        int diff = lastFrame ^ frame;

        xsimd::batch<float> sum {};
        for (size_t i = 0; i < QUALITY; i++)
        {
            if (diff & (1 << i))
            {
                values[i] = rng() - 0.5f;
            }
            sum += values[i];
        }

        return sum * (1.0f / 8.0f);
    }
};

struct LC_Oscillator
{
    LC_Oscillator() = default;

    using Vec = xsimd::batch<float>;

    void prepare (double sample_rate)
    {
        C1.prepare (static_cast<float> (sample_rate));
        I1.prepare (static_cast<float> (sample_rate));

        noise.reset();
    }

    void process (std::span<Vec> buffer)
    {
        const auto frequencies_vec = xsimd::load_aligned (frequencies);
        const auto inductor_val = 1.0f / (chowdsp::Power::ipow<2> (juce::MathConstants<float>::twoPi * frequencies_vec) * cap_val);
        I1.setInductanceValue (inductor_val);

        for (auto& v_out : buffer)
        {
            Vin.setVoltage (1.0f + noise() * 1.0e-3f);

            Vin.incident (S2.reflected());
            S2.incident (Vin.reflected());

            v_out = wdft::voltage<Vec> (C1);
        }
    }

    static constexpr auto cap_val = 1.0e-6f;
    alignas (16) float frequencies[4] { 100.0f, 100.0f, 100.0f, 100.0f };

    wdft::CapacitorT<Vec> C1 { cap_val };
    wdft::InductorT<Vec> I1 { 1.0f };
    wdft::WDFSeriesT<Vec, decltype (I1), decltype (C1)> S2 { I1, C1 };

    wdft::IdealVoltageSourceT<Vec, decltype (S2)> Vin { S2 };

    Pink_Noise<> noise;
};
