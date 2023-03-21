/*
  ==============================================================================

    SimpleSlider.cpp
    Created: 30 Oct 2022 10:05:47am
    Author:  mana

  ==============================================================================
*/

#include <JuceHeader.h>
#include "SimpleSlider.h"

//==============================================================================
namespace simpleFlanger {
    Knob::Knob(juce::RangedAudioParameter* p)
        :juce::SliderParameterAttachment(*p, *this)
    {
        // In your constructor, you should add any child components, and
        // initialise any special settings that your component needs.
        setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
        setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 0, 0);
        setPopupDisplayEnabled(true, false, nullptr);
        juce::Component::setName("unnamed");

        m_laf = std::make_unique<simpleLookAndFeel>();
        setLookAndFeel(m_laf.get());
    }

    Knob::~Knob()
    {
        setLookAndFeel(nullptr);

        m_laf = nullptr;
    }
}