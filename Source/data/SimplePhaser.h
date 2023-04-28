/*
  ==============================================================================

    SimpleFlanger.h
    Created: 30 Oct 2022 10:01:56am
    Author:  mana

  ==============================================================================
*/

#pragma once

#include "defines.h"
#include "lfo.h"
#include "MonoAPFArray.h"

class SimplePhaser
{
public:
    class LPF {
    public:
        void setCutOffFrequency(float cutoff) {
            m_cutoff = cutoff;
            auto g = float(std::tan(juce::MathConstants<double>::pi * m_cutoff / sampleRate));
            G = g / (1 + g);
        }

        float process(float inputValue) {
            auto v = G * (inputValue - s);
            auto y = v + s;
            s = y + v;
            return y;
        }

        std::atomic<float> m_cutoff = 24000.f;
        float sampleRate = 48000.f;
    private:
        float s = 0.f;
        float G = 0.f;
    };

    class StereoHPF {
    public:
        void setCutOffFrequency(float cutoff) {
            m_cutoff = cutoff;
            auto g = float(std::tan(juce::MathConstants<double>::pi * m_cutoff / sampleRate));
            G = g / (1 + g);
        }

        void process(float *left, float *right) {
            auto vl = G * (*left - sl);
            auto vr = G * (*right - sr);
            auto yl = vl + sl;
            auto yr = vr + sr;
            sl = yl + vl;
            sr = yr + vr;
            *left -= yl;
            *right -= yr;
        }

        std::atomic<float> m_cutoff = 24000.f;
        float sampleRate = 48000.f;
    private:
        float sl = 0.f;
        float sr = 0.f;
        float G = 0.f;
    };

    void prepare(float fs, int frameExpected);
    void processceBlock(juce::AudioBuffer<float>& buffer);

    void setDryWet(float var) {
        m_wet.setTargetValue(var);
        m_dry.setTargetValue(1.f - var);
    }

    LFO m_lfo;

    MonoAPFArray m_left;
    MonoAPFArray m_right;
    LPF m_leftDampLPF;
    LPF m_rightDampLPF;
    StereoHPF m_FBhighPassFilter;

    juce::SmoothedValue<float> m_mix = defaultValues::raw;
    juce::SmoothedValue<float> m_DelayMix = defaultValues::wet;
    juce::SmoothedValue<float> m_feedback = defaultValues::fb;

private:
    float m_leftLastSample = 0.f;
    float m_rightLastSample = 0.f;
    std::vector<float> m_lcutoff;
    std::vector<float> m_rcutoff;
    juce::AudioBuffer<float> m_dryBuffer;

public:
    std::atomic<bool> m_noModular = defaultValues::manual;
    juce::SmoothedValue<float> m_phaserNotches = defaultValues::pn;
    juce::SmoothedValue<float> m_q = defaultValues::apfq;
    juce::SmoothedValue<float> m_dry = 1.f - defaultValues::drywet;
    juce::SmoothedValue<float> m_wet = defaultValues::drywet;
    juce::SmoothedValue<float> m_cutoff = defaultValues::cutoff;
};