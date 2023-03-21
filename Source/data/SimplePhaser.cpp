/*
  ==============================================================================

	SimpleFlanger.cpp
	Created: 30 Oct 2022 10:01:56am
	Author:  mana

  ==============================================================================
*/

#include "SimplePhaser.h"

void SimplePhaser::prepare(float fs, int frameExpected)
{
	m_lfo.prepare(fs, frameExpected);
	m_left.prepare(fs, frameExpected);
	m_right.prepare(fs, frameExpected);
	m_dryBuffer.setSize(2, frameExpected);
	m_lcutoff.resize(frameExpected);
	m_rcutoff.resize(frameExpected);
	m_leftDampLPF.sampleRate = fs;
	m_rightDampLPF.sampleRate = fs;
	m_leftDampLPF.setCutOffFrequency(20000.f);
	m_rightDampLPF.setCutOffFrequency(20000.f);
}

void SimplePhaser::processceBlock(juce::AudioBuffer<float>& buffer)
{
	processceBlock(buffer, 0, buffer.getNumSamples());
}

void SimplePhaser::processceBlock(juce::AudioBuffer<float>& buffer, int sampleBegin, int numSamples)
{
	//TODO: use juce::DryWet instead of manually apply
	m_dryBuffer.copyFrom(0, sampleBegin, buffer.getReadPointer(0), numSamples);
	m_dryBuffer.copyFrom(1, sampleBegin, buffer.getReadPointer(1), numSamples);
	auto copyDry = m_dry;
	copyDry.applyGain(m_dryBuffer.getWritePointer(0, sampleBegin), numSamples);
	m_dry.applyGain(m_dryBuffer.getWritePointer(1, sampleBegin), numSamples);
	//m_dry.applyGain(m_dryBuffer, m_dryBuffer.getNumSamples());

	float* lptr = buffer.getWritePointer(0, sampleBegin);
	float* rptr = buffer.getWritePointer(1, sampleBegin);

	m_lfo.getDelayTimes(m_lcutoff, m_rcutoff);

	for (int i = 0; i < numSamples; ++i)
	{
		float delayMix = m_DelayMix.getNextValue();
		float mix = m_mix.getNextValue();
		float feedback = m_feedback.getNextValue();
		float notch = m_phaserNotches.getNextValue();
		float q = m_q.getNextValue();

		m_leftLastSample = m_left.process(lptr[i] + feedback * m_leftDampLPF.process(m_leftLastSample), m_lcutoff[i], notch, q);
		lptr[i] = m_leftLastSample * delayMix + lptr[i] * mix;

		m_rightLastSample = m_right.process(rptr[i] + feedback * m_rightDampLPF.process(m_rightLastSample), m_rcutoff[i], notch, q);
		rptr[i] = m_rightLastSample * delayMix + rptr[i] * mix;
	}

	//m_wet.applyGain(buffer, buffer.getNumSamples());
	auto copyWet = m_wet;
	copyWet.applyGain(buffer.getWritePointer(0, sampleBegin), numSamples);
	m_wet.applyGain(buffer.getWritePointer(1, sampleBegin), numSamples);
	buffer.addFrom(0, sampleBegin, m_dryBuffer.getReadPointer(0, sampleBegin), numSamples);
	buffer.addFrom(1, sampleBegin, m_dryBuffer.getReadPointer(1, sampleBegin), numSamples);
}
