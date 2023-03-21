/*
  ==============================================================================

    SimpleFlanger.h
    Created: 30 Oct 2022 10:01:56am
    Author:  mana

  ==============================================================================
*/

#pragma once
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

        float m_cutoff = 24000.f;
        float sampleRate = 48000.f;
    private:
        float s = 0.f;
        float G = 0.f;
    };

    void prepare(float fs, int frameExpected);
    void processceBlock(juce::AudioBuffer<float>& buffer);
    void processceBlock(juce::AudioBuffer<float>& buffer, int sampleBegin, int numSamples);

    void setDryWet(float var) {
        m_wet.setTargetValue(var);
        m_dry.setTargetValue(1.f - var);
    }

    DTSupportor::lfoSupportor m_lfo;

    MonoAPFArray m_left;
    MonoAPFArray m_right;
    LPF m_leftDampLPF;
    LPF m_rightDampLPF;

    juce::SmoothedValue<float> m_mix;
    juce::SmoothedValue<float> m_DelayMix;
    juce::SmoothedValue<float> m_feedback;

private:
    float m_leftLastSample = 0.f;
    float m_rightLastSample = 0.f;

    std::vector<float> m_lcutoff;
    std::vector<float> m_rcutoff;

public:
    juce::SmoothedValue<float> m_phaserNotches;
    juce::SmoothedValue<float> m_q = 0.71f;
    juce::SmoothedValue<float> m_dry = 1.f;
    juce::SmoothedValue<float> m_wet;
    juce::AudioBuffer<float> m_dryBuffer;
};