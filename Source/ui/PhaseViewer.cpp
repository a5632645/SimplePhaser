/*
  ==============================================================================

    PhaseViewer.cpp
    Created: 30 Oct 2022 10:01:42am
    Author:  mana

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PhaseViewer.h"

//==============================================================================
PhaseViewer::PhaseViewer(std::function<float(void)> left, std::function<float(void)> right)
    :m_funGetLeft(left), m_funGetRight(right)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

    startTimerHz(15);
}

PhaseViewer::~PhaseViewer()
{
    stopTimer();
}

void PhaseViewer::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    float lCurrent = m_funGetLeft();
    float rCurrent = m_funGetRight();

    lCurrent = juce::jlimit(0.f, m_maxPhase, lCurrent) / m_maxPhase;
    rCurrent = juce::jlimit(0.f, m_maxPhase, rCurrent) / m_maxPhase;

    auto bound = getLocalBounds();
    bound.removeFromBottom(kTextSize);

    float interval = bound.getHeight() * kInterValFractor / 2.f;

    // the texts
    g.setColour(juce::Colours::white);
    g.drawSingleLineText("0", bound.getX(), bound.getBottom() + kTextSize - 2.f);
    g.drawSingleLineText(juce::String(m_maxPhase), bound.getRight() - 3 * kTextSize, bound.getBottom() + kTextSize - 2.f);

    // the two rects
    float pointHeight = bound.getHeight() / 2.f - interval;

    g.setColour(juce::Colours::blue);
    bound.removeFromRight(kPointSize);
    float x = bound.getX() + lCurrent * bound.getWidth();
    float y = bound.getY();

    juce::Rectangle<float> lrec(x, y, kPointSize, pointHeight);
    g.fillRect(lrec);
    
    g.setColour(juce::Colours::red);
    x = bound.getX() + rCurrent * bound.getWidth();
    y = bound.getCentreY() + interval;

    juce::Rectangle<float> rrec(x, y, kPointSize, pointHeight);
    g.fillRect(rrec);

    // frameworks
    bound = getLocalBounds();
    bound.removeFromBottom(kTextSize);

    g.setColour(juce::Colours::grey);
    g.drawRect(lrec);
    g.drawRect(rrec);
    g.drawRect(bound);
    g.drawRect(getLocalBounds());
}
