/*
  ==============================================================================

    PhaseViewer.h
    Created: 30 Oct 2022 10:01:42am
    Author:  mana

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class PhaseViewer  : public juce::Component,
                     public juce::Timer
{
public:
    PhaseViewer(std::function<float(void)> left, std::function<float(void)> right);
    ~PhaseViewer() override;

    void paint (juce::Graphics&) override;

    void timerCallback() override
    {
        repaint();
    }

    //==============================================================================
private:
    std::function<float(void)> m_funGetLeft;
    std::function<float(void)> m_funGetRight;

    static constexpr float kPointSize = 10.f;
    static constexpr float kInterValFractor = 0.1f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PhaseViewer)
};
