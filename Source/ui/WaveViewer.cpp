/*
  ==============================================================================

	WaveViewer.cpp
	Created: 15 Nov 2022 8:37:36pm
	Author:  mana

  ==============================================================================
*/

#include <JuceHeader.h>
#include "WaveViewer.h"

//==============================================================================
WaveViewer::WaveViewer(DTSupportor::lfoSupportor& lfo)
	:m_table(lfo)
{
	// In your constructor, you should add any child components, and
	// initialize any special settings that your component needs.
}

void WaveViewer::paint(juce::Graphics& g)
{
	/* This demo code just fills the component's background and
	   draws some placeholder text to get you started.

	   You should replace everything in this method with your own
	   drawing code..
	*/
	g.setColour(juce::Colours::black);
	g.fillAll();

	const int width = getWidth();
	std::vector<float> wavePoint;
	wavePoint.resize(width);
	for (int i = 0; i < width; ++i) {
		wavePoint[i] = i / (float)width;
	}
	m_table.tableTest.getNextBlock(m_table.m_jitter, m_table.m_wtPos, wavePoint, wavePoint);

	const int center = getHeight() / 2;
	const int height = center - 4;
	for (float& w : wavePoint) {
		w = w * height + center;
	}

	juce::Path p;
	p.startNewSubPath(0, wavePoint[0]);
	for (int i = 1; i < width; ++i) {
		p.lineTo(i, wavePoint[i]);
	}

	g.setColour(juce::Colours::lightgreen);
	g.strokePath(p, juce::PathStrokeType(1.f));

	g.setColour(juce::Colours::grey);
	g.drawRect(getLocalBounds(), 1.f);
}