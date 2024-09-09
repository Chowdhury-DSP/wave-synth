#pragma once

#include <pch.h>

struct MXR_Phaser_Stage
{
    using Vec = xsimd::batch<float>;

    // Port B
    wdft::CapacitorT<Vec> C2 { 47.0e-9f };

    // Port C
    wdft::ResistorT<Vec> R5 { 24.0e3f }; // modulate!

    // Port D
    wdft::ResistorT<Vec> R3 { 10.0e3f };

    // Port E
    wdft::ResistorT<Vec> R4 { 10.0e3f };

    struct ImpedanceCalc
    {
        template <typename RType>
        static auto calcImpedance (RType& R)
        {
            constexpr float Ag = 100.0f; // op-amp gain
            constexpr float Ri = 1.0e9f; // op-amp input impedance
            constexpr float Ro = 1.0e-1f; // op-amp output impedance
            const auto [Rb, Rc, Rd, Re] = R.getPortImpedances();

            const auto Ra = (Rc * (Ri * (Re - Ro) + Rd * (Re + Ri + Ag * Ri - Ro)) + Rb * (Rc * Rd + Rc * Re + Rd * Re + Rd * Ri + Ag * Rd * Ri + Re * Ri - (Rc + Rd + Ri) * Ro)) / (Rc * (Rd + Ri) + Ri * (Re - Ro) + Rd * (Re + Ri + Ag * Ri - Ro) + Rb * (Rc + Re + Ri + Ag * Ri - Ro));

            R.setSMatrixData ({ { 0, -((Ra * (Rc * (Rd - Ag * Ri) + Ri * (Re - Ro) + Rd * (Re + Ri + Ag * Ri - Ro))) / (Rc * (Ri * (Re - Ro) + Rd * (Re + Ri + Ag * Ri - Ro)) + Rb * (Rc * Rd + Rc * Re + Rd * Re + Rd * Ri + Ag * Rd * Ri + Re * Ri - (Rc + Rd + Ri) * Ro))), (-(Ra * ((Rb + Rd) * Re + (Rd + Ag * (Rb + Rd) + Re) * Ri)) + Ra * (Rb + Rd + Ri) * Ro) / (Rc * (Ri * (Re - Ro) + Rd * (Re + Ri + Ag * Ri - Ro)) + Rb * (Rc * Rd + Rc * Re + Rd * Re + Rd * Ri + Ag * Rd * Ri + Re * Ri - (Rc + Rd + Ri) * Ro)), -((Ra * ((1 + Ag) * Rc * Ri + Rb * (Rc + Re + Ri + Ag * Ri - Ro))) / (Rc * (Ri * (Re - Ro) + Rd * (Re + Ri + Ag * Ri - Ro)) + Rb * (Rc * Rd + Rc * Re + Rd * Re + Rd * Ri + Ag * Rd * Ri + Re * Ri - (Rc + Rd + Ri) * Ro))), -((Ra * (Rb * (Rc + Ri) + Rc * (Rd + Ri))) / (Rc * (Ri * (Re - Ro) + Rd * (Re + Ri + Ag * Ri - Ro)) + Rb * (Rc * Rd + Rc * Re + Rd * Re + Rd * Ri + Ag * Rd * Ri + Re * Ri - (Rc + Rd + Ri) * Ro))) },
                                { -((Rb * (Rc * Rd + Ri * (Re - Ro) + Rd * (Re + Ri + Ag * Ri - Ro))) / (Rc * (Ri * (Re - Ro) + Rd * (Re + Ri + Ag * Ri - Ro)) + Rb * (Rc * Rd + Rc * Re + Rd * Re + Rd * Ri + Ag * Rd * Ri + Re * Ri - (Rc + Rd + Ri) * Ro))), (-(Rb * (Rc * (Rd + Re) + Re * Ri + Rd * (Re + Ri + Ag * Ri))) + Ra * (Rc * (Rd + Ri) + Ri * (Re - Ro) + Rd * (Re + Ri + Ag * Ri - Ro)) + Rb * (Rc + Rd + Ri) * Ro) / (Rc * (Ri * (Re - Ro) + Rd * (Re + Ri + Ag * Ri - Ro)) + Rb * (Rc * Rd + Rc * Re + Rd * Re + Rd * Ri + Ag * Rd * Ri + Re * Ri - (Rc + Rd + Ri) * Ro)), -((Rb * (Ri * (Ra + Re - Ro) + Rd * (Re + Ri + Ag * Ri - Ro))) / (Rc * (Ri * (Re - Ro) + Rd * (Re + Ri + Ag * Ri - Ro)) + Rb * (Rc * Rd + Rc * Re + Rd * Re + Rd * Ri + Ag * Rd * Ri + Re * Ri - (Rc + Rd + Ri) * Ro))), (Rb * (Rc * (Re - Ro) + Ra * (Rc + Re + Ri + Ag * Ri - Ro))) / (Rc * (Ri * (Re - Ro) + Rd * (Re + Ri + Ag * Ri - Ro)) + Rb * (Rc * Rd + Rc * Re + Rd * Re + Rd * Ri + Ag * Rd * Ri + Re * Ri - (Rc + Rd + Ri) * Ro)), (Rb * (-(Rc * Rd) + Ra * Ri)) / (Rc * (Ri * (Re - Ro) + Rd * (Re + Ri + Ag * Ri - Ro)) + Rb * (Rc * Rd + Rc * Re + Rd * Re + Rd * Ri + Ag * Rd * Ri + Re * Ri - (Rc + Rd + Ri) * Ro)) },
                                { (-(Rc * ((Rb + Rd) * Re + (Rd + Ag * Rd + Re) * Ri)) + Rc * (Rb + Rd + Ri) * Ro) / (Rc * (Ri * (Re - Ro) + Rd * (Re + Ri + Ag * Ri - Ro)) + Rb * (Rc * Rd + Rc * Re + Rd * Re + Rd * Ri + Ag * Rd * Ri + Re * Ri - (Rc + Rd + Ri) * Ro)), -((Rc * (Ri * (Ra + Ag * Ra + Re - Ro) + Rd * (Re + Ri + Ag * Ri - Ro))) / (Rc * (Ri * (Re - Ro) + Rd * (Re + Ri + Ag * Ri - Ro)) + Rb * (Rc * Rd + Rc * Re + Rd * Re + Rd * Ri + Ag * Rd * Ri + Re * Ri - (Rc + Rd + Ri) * Ro))), (-(Ra * Rc * (Rb + Rd + Ri)) + Rb * Ri * (Re - Ro) + Rb * Rd * (Re + Ri + Ag * Ri - Ro)) / (Rc * (Ri * (Re - Ro) + Rd * (Re + Ri + Ag * Ri - Ro)) + Rb * (Rc * Rd + Rc * Re + Rd * Re + Rd * Ri + Ag * Rd * Ri + Re * Ri - (Rc + Rd + Ri) * Ro)), (Rc * ((1 + Ag) * Ra * Ri + Rb * (-Re + Ro))) / (Rc * (Ri * (Re - Ro) + Rd * (Re + Ri + Ag * Ri - Ro)) + Rb * (Rc * Rd + Rc * Re + Rd * Re + Rd * Ri + Ag * Rd * Ri + Re * Ri - (Rc + Rd + Ri) * Ro)), (Rc * (Rb * Rd + Ra * (Rb + Rd + Ri))) / (Rc * (Ri * (Re - Ro) + Rd * (Re + Ri + Ag * Ri - Ro)) + Rb * (Rc * Rd + Rc * Re + Rd * Re + Rd * Ri + Ag * Rd * Ri + Re * Ri - (Rc + Rd + Ri) * Ro)) },
                                { -((Rd * (Rc * Ri + Rb * (Rc + Re + Ri + Ag * Ri - Ro))) / (Rc * (Ri * (Re - Ro) + Rd * (Re + Ri + Ag * Ri - Ro)) + Rb * (Rc * Rd + Rc * Re + Rd * Re + Rd * Ri + Ag * Rd * Ri + Re * Ri - (Rc + Rd + Ri) * Ro))), (Rd * (Rc * (Re + Ag * Ri - Ro) + Ra * (Rc + Re + Ri + Ag * Ri - Ro))) / (Rc * (Ri * (Re - Ro) + Rd * (Re + Ri + Ag * Ri - Ro)) + Rb * (Rc * Rd + Rc * Re + Rd * Re + Rd * Ri + Ag * Rd * Ri + Re * Ri - (Rc + Rd + Ri) * Ro)), (Rd * (Ra * Ri - Rb * (Re + Ag * Ri - Ro))) / (Rc * (Ri * (Re - Ro) + Rd * (Re + Ri + Ag * Ri - Ro)) + Rb * (Rc * Rd + Rc * Re + Rd * Re + Rd * Ri + Ag * Rd * Ri + Re * Ri - (Rc + Rd + Ri) * Ro)), ((Rc * Ri + Rb * (Rc + Ri)) * (Re - Ro) - Ra * Rd * (Rc + Re + Ri + Ag * Ri - Ro)) / (Rc * (Ri * (Re - Ro) + Rd * (Re + Ri + Ag * Ri - Ro)) + Rb * (Rc * Rd + Rc * Re + Rd * Re + Rd * Ri + Ag * Rd * Ri + Re * Ri - (Rc + Rd + Ri) * Ro)), -((Rd * (Rb * Rc + (Ra + Rb + Rc) * Ri)) / (Rc * (Ri * (Re - Ro) + Rd * (Re + Ri + Ag * Ri - Ro)) + Rb * (Rc * Rd + Rc * Re + Rd * Re + Rd * Ri + Ag * Rd * Ri + Re * Ri - (Rc + Rd + Ri) * Ro))) },
                                { -((Re * (Rc * (Rd + Ri) + Rb * (Rc + Ri + Ag * Ri))) / (Rc * (Ri * (Re - Ro) + Rd * (Re + Ri + Ag * Ri - Ro)) + Rb * (Rc * Rd + Rc * Re + Rd * Re + Rd * Ri + Ag * Rd * Ri + Re * Ri - (Rc + Rd + Ri) * Ro))), (-(Rc * Rd * Re) + (Ra + Ag * Ra + Ag * Rc) * Re * Ri) / (Rc * (Ri * (Re - Ro) + Rd * (Re + Ri + Ag * Ri - Ro)) + Rb * (Rc * Rd + Rc * Re + Rd * Re + Rd * Ri + Ag * Rd * Ri + Re * Ri - (Rc + Rd + Ri) * Ro)), (Re * (Ra * (Rb + Rd + Ri) + Rb * (Rd - Ag * Ri))) / (Rc * (Ri * (Re - Ro) + Rd * (Re + Ri + Ag * Ri - Ro)) + Rb * (Rc * Rd + Rc * Re + Rd * Re + Rd * Ri + Ag * Rd * Ri + Re * Ri - (Rc + Rd + Ri) * Ro)), -((Rb * Rc * Re + (1 + Ag) * (Ra + Rb + Rc) * Re * Ri) / (Rc * (Ri * (Re - Ro) + Rd * (Re + Ri + Ag * Ri - Ro)) + Rb * (Rc * Rd + Rc * Re + Rd * Re + Rd * Ri + Ag * Rd * Ri + Re * Ri - (Rc + Rd + Ri) * Ro))), ((1 + Ag) * Rc * Rd * Ri - Ra * Re * (Rd + Ri) - Rc * (Rd + Ri) * Ro + Rb * (-(Ra * Re) + Rd * (Rc + Ri + Ag * Ri) - (Rc + Rd + Ri) * Ro)) / (Rc * (Ri * (Re - Ro) + Rd * (Re + Ri + Ag * Ri - Ro)) + Rb * (Rc * Rd + Rc * Re + Rd * Re + Rd * Ri + Ag * Rd * Ri + Re * Ri - (Rc + Rd + Ri) * Ro)) } });

            return Ra;
        }
    };
    wdft::RtypeAdaptor<Vec, 0, ImpedanceCalc, decltype (C2), decltype (R5), decltype (R3), decltype (R4)> R { C2, R5, R3, R4 };

    // Port A
    wdft::IdealVoltageSourceT<Vec, decltype (R)> Vin { R };

    void prepare (double sample_rate)
    {
        C2.prepare (static_cast<float> (sample_rate));
    }

    Vec process_sample (Vec x) noexcept
    {
        Vin.setVoltage (x);

        Vin.incident (R.reflected());
        R.incident (Vin.reflected());

        return wdft::voltage<Vec> (R5) - wdft::voltage<Vec> (R4);
    }
};

struct LC_LFO
{
    LC_LFO() = default;

    void prepare (double sample_rate)
    {
        C1.prepare (sample_rate);
        I1.prepare (sample_rate);
    }

    inline auto process_sample() noexcept
    {
        const auto inductor_val = 1.0 / (chowdsp::Power::ipow<2> (2.0 * M_PI * frequency_hz) * cap_val);
        I1.setInductanceValue (inductor_val);

        Vin.setVoltage (1.0);

        Vin.incident (S2.reflected());
        S2.incident (Vin.reflected());

        return std::array { static_cast<float> (wdft::voltage<double> (C1)),
                            static_cast<float> (wdft::voltage<double> (I1)) };
    }

    static constexpr auto cap_val = 1.0e-6;
    double frequency_hz = 10.0;

    wdft::CapacitorT<double> C1 { cap_val };
    wdft::InductorT<double> I1 { 1.0 };
    wdft::WDFSeriesT<double, decltype (I1), decltype (C1)> S2 { I1, C1 };

    wdft::IdealVoltageSourceT<double, decltype (S2)> Vin { S2 };
};

template <typename T, typename Next>
class UResistorT final : public wdft::BaseWDF
{
public:
    /** Creates a new WDF Resistor with a given resistance.
     * @param value: resistance in Ohms
     */
    explicit UResistorT (T value, const Next& n) : next (n), R_value (value)
    {
        calcImpedance();
    }

    /** Sets the resistance value of the WDF resistor, in Ohms. */
    void setResistanceValue (T newR)
    {
        if (all (newR == R_value))
            return;

        R_value = newR;
        propagateImpedanceChange();
    }

    /** Computes the impedance of the WDF resistor, Z_R = R. */
    inline void calcImpedance() override
    {
        wdf.R = R_value;
        wdf.G = (T) 1.0 / wdf.R;
    }

    /** Accepts an incident wave into a WDF resistor. */
    inline void incident (T x) noexcept
    {
        wdf.a = x;
    }

    /** Propogates a reflected wave from a WDF resistor. */
    inline T reflected() noexcept
    {
        const auto g = (wdf.R - next.wdf.R) / (wdf.R + next.wdf.R);
        wdf.b = wdf.a * g;
        return wdf.b;
    }

    wdft::WDFMembers<T> wdf;

private:
    const Next& next;
    T R_value = (T) 1.0e-9;
};

struct Phaser_Input
{
    wdft::ResistorCapacitorSeriesT<float> R1_C1 { 10.0e3f, 0.01e-6f };
    wdft::ResistorT<float> R2 { 470.0e3f };
    wdft::WDFSeriesT<float, decltype (R1_C1), decltype (R2)> S1 { R1_C1, R2 };
    wdft::IdealVoltageSourceT<float, decltype (S1)> Vin { S1 };

    void prepare (double sample_rate)
    {
        R1_C1.prepare (static_cast<float> (sample_rate));
    }

    inline float process_sample (float x) noexcept
    {
        Vin.setVoltage (x);
        Vin.incident (S1.reflected());
        S1.incident (Vin.reflected());
        return wdft::voltage<float> (R2);
    }
};

struct Phaser_Output
{
    wdft::ResistiveVoltageSourceT<float> V_dry { 150.0e3f };
    wdft::ResistiveVoltageSourceT<float> V_wet { 150.0e3f };
    wdft::WDFParallelT<float, decltype (V_dry), decltype (V_wet)> Pv { V_dry, V_wet };

    wdft::ResistorT<float> R6 { 150.0e3f };
    wdft::WDFSeriesT<float, decltype (Pv), decltype (R6)> S1 { Pv, R6 };

    wdft::ResistorT<float> R7 { 56.0e3f };
    wdft::WDFParallelT<float, decltype (S1), decltype (R7)> P1 { S1, R7 };

    wdft::CapacitorT<float> C3 { 47.0e-9f };
    wdft::WDFSeriesT<float, decltype (P1), decltype (C3)> S_out { P1, C3 };

    UResistorT<float, decltype (S_out)> R_out { 150.0e3f, S_out };

    void prepare (double sample_rate)
    {
        C3.prepare (static_cast<float> (sample_rate));
    }

    inline float process_sample (float v_dry, float v_wet) noexcept
    {
        V_dry.setVoltage (v_dry);
        V_wet.setVoltage (v_wet);
        R_out.incident (S_out.reflected());
        S_out.incident (R_out.reflected());
        return 7.0f * wdft::voltage<float> (R_out);
    }
};

struct Phaser
{
    struct Params : chowdsp::ParamHolder
    {
        chowdsp::BoolParameter::Ptr on_off {
            PID { "phaser_onoff", 100 },
            "On/Off",
            false,
        };

        chowdsp::FreqHzParameter::Ptr rate {
            PID { "phaser_rate", 100 },
            "Rate",
            chowdsp::ParamUtils::createNormalisableRange (1.0f, 10.0f, 2.5f),
            1.0f,
        };

        Params() : ParamHolder { "Phaser" }
        {
            add (on_off, rate);
        }
    };

    std::array<MXR_Phaser_Stage, 2> stages_4;
    alignas (16) std::array<std::array<float, 4>, 2> stage_io {};

    Phaser_Input input;
    std::array<Phaser_Output, 2> outputs;
    LC_LFO lfo;

    // JFET params
    static constexpr auto V_p = -5.012f;
    static constexpr auto I_s0 = 1.0e-3f;
    static constexpr auto lambda = 4.0e-3f;

    Params& params;
    explicit Phaser (Params& phaser_params) : params { phaser_params }
    {
    }

    void prepare (double sample_rate)
    {
        input.prepare (sample_rate);
        for (auto& mono_stage : stages_4)
            mono_stage.prepare (sample_rate);
        for (auto& output : outputs)
            output.prepare (sample_rate);
        lfo.prepare (sample_rate);
    }

    void process (std::span<const float> in_buffer,
                  std::span<float> out_left_buffer,
                  std::span<float> out_right_buffer)
    {
        const auto num_samples = in_buffer.size();

        if (params.on_off->get() == false)
        {
            for (size_t n = 0; n < num_samples; ++n)
            {
                out_left_buffer[n] = in_buffer[n];
                out_right_buffer[n] = in_buffer[n];
            }
            return;
        }

        lfo.frequency_hz = static_cast<double> (params.rate->getCurrentValue());
        std::array<float, 2> temp_outputs {};
        for (size_t n = 0; n < num_samples; ++n)
        {
            const auto v_in = input.process_sample (in_buffer[n]);
            const auto lfo_v = lfo.process_sample();

            for (auto [idx, stage] : chowdsp::enumerate (stages_4))
            {
                const auto v1_k = 5.0f + 4.99f * chowdsp::Math::algebraicSigmoid (lfo_v[idx]);
                const auto v2_k1 = wdft::voltage<xsimd::batch<float>> (stage.R5);
                const auto R_jfet = xsimd::select (xsimd::broadcast (v1_k - V_p) >= v2_k1,
                                                   V_p * V_p / (2.0f * I_s0 * (v1_k - V_p - 0.5f * v2_k1)),
                                                   v2_k1 / (I_s0 * chowdsp::Power::ipow<2> (1.0f - v1_k / V_p) * (1.0f + lambda * v2_k1)));

                static constexpr auto R5_base = 24.0e3f;
                const auto R5 = (R5_base * R_jfet) / (R5_base + R_jfet);
                stage.R5.setResistanceValue (R5);

                stage_io[idx][3] = v_in;
                const auto y = stage.process_sample (xsimd::load_aligned (stage_io[idx].data()));
                xsimd::store_aligned (stage_io[idx].data(), y);
                temp_outputs[idx] = stage_io[idx][0];
                stage_io[idx][0] = stage_io[idx][1];
                stage_io[idx][1] = stage_io[idx][2];
                stage_io[idx][2] = stage_io[idx][3];
            }

            out_left_buffer[n] = outputs[0].process_sample (v_in, temp_outputs[0]);
            out_right_buffer[n] = outputs[1].process_sample (v_in, temp_outputs[1]);
        }
    }
};
