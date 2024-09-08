#pragma once

#include <pch.h>

struct AR_Envelope
{
    struct Params : chowdsp::ParamHolder
    {
        chowdsp::TimeMsParameter::Ptr attack {
            PID { "env_attack", 100 },
            "Attack",
            chowdsp::ParamUtils::createNormalisableRange (1.0f, 100.0f, 10.0f),
            10.0f,
        };

        chowdsp::TimeMsParameter::Ptr release {
            PID { "env_release", 100 },
            "Release",
            chowdsp::ParamUtils::createNormalisableRange (10.0f, 1'000.0f, 100.0f),
            100.0f,
        };

        Params() : ParamHolder { "Envelope" }
        {
            add (attack, release);
        }
    };

    using Vec = xsimd::batch<float>;

    static constexpr auto cap_val = 0.47e-6f;
    wdft::ResistorCapacitorParallelT<Vec> Co_Rrel { 300.0e3f, cap_val };
    wdft::ResistorT<Vec> Ratt { 10.0f };
    wdft::WDFSeriesT<Vec, decltype (Co_Rrel), decltype (Ratt)> So { Co_Rrel, Ratt };

    wdft::ResistiveVoltageSourceT<Vec> Vin { 1.0f };
    wdft::WDFSeriesT<Vec, decltype (Vin), decltype (So)> Sroot { Vin, So };
    wdft::DiodeT<Vec, decltype (Sroot)> diode { Sroot, 1.0e-12f };

    Params& params;

    AR_Envelope (Params& env_params) : params { env_params }
    {
    }

    void prepare (double fs)
    {
        Co_Rrel.prepare (static_cast<float> (fs));
    }

    void process (std::span<Vec> buffer)
    {
        const auto attack_time = 0.001f * params.attack->getCurrentValue();
        // const auto release_time = std::max (0.001f * params.release->getCurrentValue(), attack_time * 2.0f);
        const auto release_time = 0.001f * params.release->getCurrentValue();

        const auto r_release = release_time / cap_val;
        Co_Rrel.setResistanceValue(r_release);

        const auto r_attack = attack_time * r_release / std::max (cap_val * r_release - attack_time, 1.0f);
        Ratt.setResistanceValue (r_attack);

        for (auto& x : buffer)
        {
            Vin.setVoltage (-x);

            diode.incident (Sroot.reflected());
            Sroot.incident (diode.reflected());

            x = chowdsp::Math::algebraicSigmoid (-wdft::voltage<Vec> (Co_Rrel));
        }
    }
};
