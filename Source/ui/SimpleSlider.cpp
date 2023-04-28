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
        constexpr auto pi = juce::MathConstants<float>::pi;
        // In your constructor, you should add any child components, and
        // initialise any special settings that your component needs.
        setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
        setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 0, 0);
        setPopupDisplayEnabled(true, false, nullptr);
        setRotaryParameters(1.3f*pi, 2.7f*pi, true);
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