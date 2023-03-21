/*
  ==============================================================================

    choiceButton.cpp
    Created: 19 Mar 2023 8:38:10pm
    Author:  mana

  ==============================================================================
*/

#include <JuceHeader.h>
#include "choiceButton.h"

namespace simpleFlanger {
    //==============================================================================
    choiceButton::choiceButton(juce::AudioParameterBool* attachParam)
        :juce::ButtonParameterAttachment(*attachParam, *this)
    {
        // In your constructor, you should add any child components, and
        // initialise any special settings that your component needs.
        m_laf = std::make_unique<simpleFlanger::simpleLookAndFeel>();
        setLookAndFeel(m_laf.get());
    }

    choiceButton::~choiceButton()
    {
        setLookAndFeel(nullptr);
        m_laf = nullptr;
    }
}