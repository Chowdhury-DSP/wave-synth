#pragma once

#include <pch.h>

struct DC_Blocker
{
    void prepare (double sample_rate)
    {
        C1.prepare (static_cast<float> (sample_rate));
    }

    void process (std::span<float> buffer)
    {
        for (auto& x : buffer)
        {
            Vin.setVoltage (x);

            Vin.incident (S1.reflected());
            S1.incident (Vin.reflected());

            x = wdft::voltage<float> (R1);
        }
    }

    wdft::ResistorT<float> R1 { 3.5e3f }; // cutoff = 45 Hz
    wdft::CapacitorT<float> C1 { 1.0e-6f };
    wdft::WDFSeriesT<float, decltype (R1), decltype (C1)> S1 { R1, C1 };

    wdft::IdealVoltageSourceT<float, decltype (S1)> Vin { S1 };
};
