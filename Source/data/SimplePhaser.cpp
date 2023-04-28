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
	m_FBhighPassFilter.sampleRate = fs;
	m_leftDampLPF.setCutOffFrequency(defaultValues::fblp);
	m_rightDampLPF.setCutOffFrequency(defaultValues::fblp);
	m_FBhighPassFilter.setCutOffFrequency(defaultValues::fbhp);
	m_cutoff.reset(defaultValues::step);
}

void SimplePhaser::processceBlock(juce::AudioBuffer<float>& buffer)
{
	auto numSample = buffer.getNumSamples();
	float* lptr = buffer.getWritePointer(0);
	float* rptr = buffer.getWritePointer(1);

	//TODO: use juce::DryWet instead of manually apply
	m_dryBuffer.copyFrom(0, 0, buffer.getReadPointer(0), numSample);
	m_dryBuffer.copyFrom(1, 0, buffer.getReadPointer(1), numSample);
	m_dry.applyGain(m_dryBuffer, numSample);

	if (m_noModular) {
		for (float& val : m_lcutoff) {
			val = m_cutoff.getNextValue();
		}
		std::copy(m_lcutoff.begin(), m_lcutoff.end(), m_rcutoff.begin());
	}
	else {
		m_lfo.getDelayTimes(m_lcutoff, m_rcutoff);
	}


	for (int i = 0; i < numSample; ++i)
	{
		float delayMix = m_DelayMix.getNextValue();
		float mix = m_mix.getNextValue();
		float feedback = m_feedback.getNextValue();
		float notch = m_phaserNotches.getNextValue();
		float q = m_q.getNextValue();

		float fbl = m_leftDampLPF.process(m_leftLastSample);
		float fbr = m_rightDampLPF.process(m_rightLastSample);
		m_FBhighPassFilter.process(&fbl, &fbr);
		m_leftLastSample = m_left.process(lptr[i] + feedback * fbl, m_lcutoff[i], notch, q);
		lptr[i] = m_leftLastSample * delayMix + lptr[i] * mix;

		m_rightLastSample = m_right.process(rptr[i] + feedback * fbr, m_rcutoff[i], notch, q);
		rptr[i] = m_rightLastSample * delayMix + rptr[i] * mix;
	}

	m_wet.applyGain(buffer, numSample);
	buffer.addFrom(0, 0, m_dryBuffer.getReadPointer(0), numSample);
	buffer.addFrom(1, 0, m_dryBuffer.getReadPointer(1), numSample);
}
