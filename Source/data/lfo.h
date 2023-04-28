/*
  ==============================================================================

	lfo.h
	Created: 12 Nov 2022 9:03:05pm
	Author:  mana

  ==============================================================================
*/

#pragma once

#include "defines.h"
#include "functions.h"
#include "waveTable.h"

class LFO {
public:
	WaveTable tableTest;

	//==============================================================================

	void getDelayTimes(std::vector<float>& left, std::vector<float>& right) {
		const juce::ScopedLock lm(tableTest.m_mutex);

		auto frame = (int)left.size();

		if (m_bSyncBPM) {
			for (int i = 0; i < frame; ++i) {
				m_rphase[i] = fmodf(m_lfoLPhase + m_stereoPhase.getNextValue() + 1.f, 1.f);
				m_lphase[i] = m_lfoLPhase;

				float rate = m_bpm / 60.f / m_bpmRate.getNextValue() * 0.1f;

				m_lfoLPhase += rate / m_sampleRate;
				while (m_lfoLPhase > 1.f)
					m_lfoLPhase -= 1.f;
			}
		}
		else {
			for (int i = 0; i < frame; ++i) {
				m_rphase[i] = fmodf(m_lfoLPhase + m_stereoPhase.getNextValue() + 1.f, 1.f);
				m_lphase[i] = m_lfoLPhase;

				m_lfoLPhase += m_rate.getNextValue() / m_sampleRate;
				while (m_lfoLPhase > 1.f)
					m_lfoLPhase -= 1.f;
			}
		}

		tableTest.getNextBlock(m_jitter, m_wtPos, m_lphase, left);
		tableTest.getNextBlock(m_jitter, m_wtPos, m_rphase, right);

		for (int i = 0; i < frame; ++i) {
			float begin = m_minSt.getNextValue();
			float end = m_maxSt.getNextValue();
			float range = end - begin;

			// get st values
			left[i] = (left[i] + 1.f) * 0.5f * range + begin;
			right[i] = (right[i] + 1.f) * 0.5f * range + begin;

			// st to frequency
			m_leftOut = semitoneToHz(left[i]);
			m_rightOut = semitoneToHz(right[i]);
			left[i] = m_leftOut.getCurrentValue();
			right[i] = m_rightOut.getCurrentValue();
		}
	}

	void prepare(float fs, int frameExcept) {
		m_sampleRate = fs;

		m_lphase.resize(frameExcept);
		m_rphase.resize(frameExcept);
		m_leftOut.reset(defaultValues::step);
		m_rightOut.reset(defaultValues::step);
	}

	void setMin(float fre) {
		m_minSt = hzToSemitone(fre);
	}

	void setMax(float fre) {
		m_maxSt = hzToSemitone(fre);
	}

	//==============================================================================
	juce::SmoothedValue<float> m_stereoPhase = defaultValues::phase;
	juce::SmoothedValue<float> m_rate = defaultValues::lfoRate;
	juce::SmoothedValue<float> m_bpmRate = defaultValues::syncRate;
	std::atomic<float> m_jitter = defaultValues::nj;
	std::atomic<float> m_wtPos = defaultValues::wtpos;
	std::atomic<bool> m_bSyncBPM = defaultValues::bpmfollow;
	// this value only change in sound thread so it is no need to be atomic
	float m_bpm = defaultValues::bpm;
private:
	juce::SmoothedValue<float> m_maxSt = hzToSemitone(defaultValues::maxFre);
	juce::SmoothedValue<float> m_minSt = hzToSemitone(defaultValues::minFre);
	juce::SmoothedValue<float> m_leftOut;
	juce::SmoothedValue<float> m_rightOut;
	std::vector<float> m_lphase;
	std::vector<float> m_rphase;
	float m_sampleRate = 48000.f;
	float m_lfoLPhase = 0.f;
};
