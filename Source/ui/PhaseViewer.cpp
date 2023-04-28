/*
  ==============================================================================

    PhaseViewer.cpp
    Created: 30 Oct 2022 10:01:42am
    Author:  mana

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PhaseViewer.h"
#include "../data/lfo.h"

static const float st0hz     = hzToSemitone(19.f);
static const float st20000hz = hzToSemitone(20001.f);
static const float width     = st20000hz - st0hz;

//==============================================================================
PhaseViewer::PhaseViewer(std::function<float(void)> left, std::function<float(void)> right)
    :m_funGetLeft(left), m_funGetRight(right)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

    startTimerHz(25);
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

    lCurrent = hzToSemitone(lCurrent);
    rCurrent = hzToSemitone(rCurrent);

    lCurrent = (lCurrent - st0hz) / width;
    rCurrent = (rCurrent - st0hz) / width;

    auto bound = getLocalBounds();

    float interval = bound.getHeight() * kInterValFractor / 2.f;

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

    g.setColour(juce::Colours::grey);
    g.drawRect(lrec);
    g.drawRect(rrec);
    g.drawRect(bound);
    g.drawRect(getLocalBounds());
}
