/*
  ==============================================================================

	waveTable.h
	Created: 18 Nov 2022 11:14:45am
	Author:  mana

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "defines.h"

class WaveTable {
public:
	WaveTable()
	{
		m_waves.resize(5);
		for (auto& w : m_waves) {
			w.resize(kPresetSize + 1uLL);
		}

		for (int i = 0; i < kPresetSize; ++i) {
			m_waves[0][i] = sinf(float(i) / static_cast<float>(kPresetSize) * juce::MathConstants<float>::twoPi);
		}

		for (int i = 0; i < kPresetSize; ++i) {
			m_waves[1][i] = -(acosf(sinf(float(i) / static_cast<float>(kPresetSize) * juce::MathConstants<float>::twoPi)) / juce::MathConstants<float>::pi - 0.5f) * 2.f;
		}

		for (int i = 0; i < kPresetSize / 2; ++i) {
			m_waves[2][i] = i / static_cast<float>(kPresetSize / 2);
		}
		for (int i = kPresetSize / 2; i < kPresetSize; ++i) {
			m_waves[2][i] = m_waves[2][static_cast<size_t>(i) - kPresetSize / 2] - 1.f;
		}

		for (int i = 0; i < kPresetSize; ++i) {
			m_waves[3][i] = m_waves[0][i] > 0.0f ? 1.0f : -1.0f;
		}

		genNoise(defaultValues::npg);

		for (auto& w : m_waves) {
			w[kPresetSize] = w[0];
		}
	}

	void genNoise(int nNoiseGen) {
		const juce::ScopedLock lm(m_mutex);

		int couter = 0;
		for (int i = 0; i < nNoiseGen; ++i) {
			auto nSame = kPresetSize / nNoiseGen;
			auto val = (rand() / float(INT16_MAX) - 0.5f) * 2.f;
			for (int j = 0; j < nSame; j++) {
				m_waves[4][couter] = val;
				couter++;
			}
		}
		m_waves[4][kPresetSize] = m_waves[4][0];
	}

	void getNextBlock(float jit, float wtPos, const std::vector<float>& indexs, std::vector<float>& buffer)
	{
		auto size = indexs.size();
		auto wt = wtPos * (m_waves.size() - 2);

		auto beforewt = (int)floor(wt);
		auto intervalwt = wt - beforewt;
		int nextwt = beforewt + 1;
		nextwt = nextwt > 4 ? 4 : nextwt;

		auto& wA = m_waves[beforewt];
		auto& wB = m_waves[nextwt];

		for (size_t i = 0; i < size; ++i)
		{
			float index = indexs[i] * kPresetSize;
			auto before = (int)floor(index);
			int next = before + 1;
			next &= kmask;
			float inter = index - (float)before;

			auto sa = (wA[before] + inter * (wA[next] - wA[before]));
			auto sb = (wB[before] + inter * (wB[next] - wB[before]));
			buffer[i] = sa + (sb - sa) * intervalwt + jit * m_waves[4][before];

			if (buffer[i] > 1.f)buffer[i] = 1.f;
			if (buffer[i] < -1.f)buffer[i] = -1.f;
		}
	}

	static constexpr int kPresetSize = 2048;
	static constexpr int kmask = kPresetSize - 1;
	std::vector<std::vector<float>> m_waves;
	juce::CriticalSection m_mutex;
};