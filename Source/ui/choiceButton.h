/*
  ==============================================================================

    choiceButton.h
    Created: 19 Mar 2023 8:38:10pm
    Author:  mana

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

// I am sorry, the look and feel is defined here
#include "SimpleSlider.h"

//==============================================================================
/*
*/
namespace simpleFlanger {

    class choiceButton : public juce::ToggleButton, public juce::ButtonParameterAttachment
    {
    public:
        explicit choiceButton(juce::AudioParameterBool* attachParam);
        ~choiceButton() override;

    private:
        std::unique_ptr<simpleFlanger::simpleLookAndFeel>m_laf;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(choiceButton)
    };
}