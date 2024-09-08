#pragma once

#include <pch.h>

struct Diode_Clipper
{
    struct Params : chowdsp::ParamHolder
    {
        chowdsp::FreqHzParameter::Ptr high_cut {
            PID { "dc_high_cut", 100 },
            "Diode Clipper High Cut",
            chowdsp::ParamUtils::createNormalisableRange (200.0f, 20'000.0f, 2000.0f),
            20'000.0f,
        };

        Params()
        {
            add (high_cut);
        }
    };

    explicit Diode_Clipper (Params& _params) : params { _params }
    {
    }

    using Vec = xsimd::batch<float>;

    void prepare (double sample_rate)
    {
        C_lpf.prepare (static_cast<float> (sample_rate));
    }

    void process (std::span<Vec> buffer)
    {
        const auto cap_lpf = 1.0f / (juce::MathConstants<float>::twoPi * params.high_cut->getCurrentValue() * res_val);
        C_lpf.setCapacitanceValue (cap_lpf);

        for (auto& x : buffer)
        {
            Vin.setVoltage (x);

            diode_pair.incident (P1.reflected());
            P1.incident (diode_pair.reflected());

            x = wdft::voltage<Vec> (C_lpf);
        }
    }

    Params& params;

    static constexpr auto res_val = 1.0e3f;
    wdft::ResistiveVoltageSourceT<Vec> Vin { res_val };

    wdft::CapacitorT<Vec> C_lpf { 1.0e-6f };
    wdft::WDFParallelT<Vec, decltype (Vin), decltype (C_lpf)> P1 { Vin, C_lpf };

    wdft::DiodePairT<Vec, decltype (P1)> diode_pair { P1, 1.0e-9f };
};
