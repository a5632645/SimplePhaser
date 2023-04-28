/*
  ==============================================================================

    functions.h
    Created: 25 Apr 2023 8:22:52pm
    Author:  mana

  ==============================================================================
*/

#pragma once

#include <cmath>

static inline float semitoneToHz(float st) {
	return 440.f * std::pow(2.f, (st - 69.f) / 12.f);
}

static inline float hzToSemitone(float hz) {
	return 12.f * log2f(hz / 440.f) + 69.f;

}
