/*
  ==============================================================================

    WaveViewer.h
    Created: 15 Nov 2022 8:37:36pm
    Author:  mana

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../data/lfo.h"

//==============================================================================
/*
*/

class WaveViewer  : public juce::Component
{
public:
    explicit WaveViewer(LFO& lfo);
    ~WaveViewer() override = default;

    void paint (juce::Graphics&) override;

private:
    LFO& m_table;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveViewer)
};
