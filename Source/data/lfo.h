/*
  ==============================================================================

	lfo.h
	Created: 12 Nov 2022 9:03:05pm
	Author:  mana

  ==============================================================================
*/

#pragma once
#include "waveTable.h"

namespace DTSupportor {
	class lfoSupportor {
	public:
		WaveTable tableTest;
		//==============================================================================

		void getDelayTimes(std::vector<float>& left, std::vector<float>& right) {
			const juce::ScopedLock lm(tableTest.m_mutex);

			auto frame = (int)left.size();

			for (int i = 0; i < frame; ++i) {
				m_rphase[i] = fmodf(m_lfoLPhase + m_stereoPhase.getNextValue() + 1.f, 1.f);
				m_lphase[i] = m_lfoLPhase;

				updatePhase();
			}

			tableTest.getNextBlock(m_jitter, m_wtPos, m_lphase, left);
			tableTest.getNextBlock(m_jitter, m_wtPos, m_rphase, right);

			for (int i = 0; i < frame; ++i) {
				float begin = m_belowFrequency.getNextValue();
				float end = m_upperFrequency.getNextValue();
				float range = end - begin;

				left[i] = (left[i] + 1.f) * 0.5f * range + begin;
				right[i] = (right[i] + 1.f) * 0.5f * range + begin;
			}
		}

		void prepare(float fs, int frameExcept) {
			m_sampleRate = fs;

			m_lphase.resize(frameExcept);
			m_rphase.resize(frameExcept);
		}

		//==============================================================================
		void setBPMRate(float val) {
			m_syncBPM = val;
			setRate(m_bpm / 60.f / m_syncBPM * 0.1f);
		}

		void setBPM(float bpm) {
			m_bpm = bpm;
			setRate(m_bpm / 60.f / m_syncBPM * 0.1f);
		}

		void setRate(float newRate) {
			m_rate.setTargetValue(newRate);
		}

		float getRate()const {
			return m_rate.getTargetValue();
		}

		void setPhase(float p) {
			m_stereoPhase.setTargetValue(p);
		}

		float getPhase() const {
			return m_stereoPhase.getTargetValue();
		}

		void updatePhase() {
			m_lfoLPhase += m_rate.getNextValue() / m_sampleRate;
			while (m_lfoLPhase > 1.f)
				m_lfoLPhase -= 1.f;
		}

		juce::SmoothedValue<float> m_belowFrequency;
		juce::SmoothedValue<float> m_upperFrequency;
		juce::SmoothedValue<float> m_stereoPhase;
		juce::SmoothedValue<float> m_rate;
		std::atomic<float> m_jitter = 0.f;
		std::atomic<float> m_wtPos = 0.f;
		std::atomic<float> m_syncBPM = 1.f;
		// this value only change in sound thread so it is no need to be atomic
		float m_bpm = 0.f;
	private:
		std::vector<float> m_lphase;
		std::vector<float> m_rphase;
		float m_sampleRate = 48000.f;
		float m_lfoLPhase = 0.f;
	};
}