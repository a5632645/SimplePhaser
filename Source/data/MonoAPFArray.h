/*
  ==============================================================================

	MonoFlanger.h
	Created: 30 Oct 2022 9:01:21pm
	Author:  mana

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class MonoAPFArray {
public:
	static constexpr auto kmaxNotches = 32;
	static constexpr auto kNumAPF = kmaxNotches;

	class APF {
	public:
		float process(float inputValue, float g) {
			float s = LPF(inputValue, g);

			return 2 * s - inputValue;
		}
	private:
		float LPF(float inputValue, float g) {
			auto v = g * (inputValue - sign1);
			auto y = v + sign1;
			sign1 = y + v;
			return y;
		}

		float sign1 = 0.f;
	};

	//===================================================================================

	struct filterCoefficents {
		double c0;
		double c1;
		double c2;
		double c3;
		double c4;
	};

	// this apf is a second order apf
	class AllPassFilter {
	private:
		struct {
			double xm1 = 0.0f;
			double xm2 = 0.0f;
			double ym1 = 0.0f;
			double ym2 = 0.0f;
		} temp;
	public:
		float process(float in, const MonoAPFArray::filterCoefficents& coeff) {
			float tempx;
			float tempy;
			tempx = in;
			tempy = static_cast<float>(coeff.c0 * tempx +
				coeff.c1 * temp.xm1 +
				coeff.c2 * temp.xm2 -
				coeff.c3 * temp.ym1 -
				coeff.c4 * temp.ym2);

			temp.ym2 = temp.ym1;
			temp.ym1 = tempy;

			temp.xm2 = temp.xm1;
			temp.xm1 = tempx;

			return tempy;
		}
	};

	//===================================================================================

	void prepare(float fs, int /*frameExpected*/) {
		m_sampleRate = fs;
		setParam(m_cutoff, m_q);
	}

	void setParam(float cutoff, float q) {
		auto g = float(std::tan(juce::MathConstants<double>::pi * cutoff / m_sampleRate));
		m_g = g / (1 + g);

		const double pi = 3.14159265358979323846;
		m_cutoff = cutoff;
		m_q = q;
		double b0;
		double b1;
		double b2;
		double a0;
		double a1;
		double a2;
		double w0 = 2 * pi * cutoff / m_sampleRate; // is this sr or sr/2 ?
		double cosw0 = cos(w0);
		double sinw0 = sin(w0);
		double alpha = sinw0 * (2 * q);

		b0 = 1 - alpha;
		b1 = -2 * cosw0;
		b2 = 1 + alpha;
		a0 = 1 + alpha;
		a1 = -2 * cosw0;
		a2 = 1 - alpha;

		m_sharedCoefficent.c0 = b0 / a0;
		m_sharedCoefficent.c1 = b1 / a0;
		m_sharedCoefficent.c2 = b2 / a0;
		m_sharedCoefficent.c3 = a1 / a0;
		m_sharedCoefficent.c4 = a2 / a0;
	}

	float process(float inputValue, float cutoff, int notch, float q) {
		m_cutoff = cutoff;
		m_q = q;
		setParam(cutoff, q);
		for (int i = 0; i < notch; i++) {
			//inputValue = m_apfs[i].process(inputValue, m_g);
			inputValue = m_apfs[i].process(inputValue, m_sharedCoefficent);
		}
		return inputValue;
	}

	float m_cutoff = 0.f;
	float m_q = 0.71f;
	float m_sampleRate = 0.f;
private:
	filterCoefficents m_sharedCoefficent;

	//std::array<APF, kmaxNotches> m_apfs;
	std::array<AllPassFilter, kNumAPF> m_apfs;

	float m_g = 0.f;
};