/*
  ==============================================================================

    defines.h
    Created: 25 Apr 2023 8:22:39pm
    Author:  mana

  ==============================================================================
*/

#pragma once

namespace defaultValues {

#define PARAM_BEGIN_DELAY "minFreqeuncy"
    static constexpr auto minFre        = 20.f;

#define PARAM_END_DELAY  "maxFrequency"
    static constexpr auto maxFre        = 20000.f;

#define PARAM_FEEDBACK "feedback"
    static constexpr auto fb            = 0.f;

#define PARAM_LFORATE "lfoRate"
    static constexpr auto lfoRate       = 0.f;

#define PARAM_SYNCLFORATE "syncLfoRate"
    static constexpr auto syncRate      = 1.f;

#define PARAM_DRYWET "dryWet"
    static constexpr auto drywet        = 1.f;

#define PARAM_RAWMIX "rawMix"
    static constexpr auto raw           = 1.f;

#define PARAM_PROCESSEDMIX "processedMix"
    static constexpr auto wet           = 1.f;

#define PARAM_WIDEPHASE "widePhase"
    static constexpr auto phase         = 0.f;

#define PARAM_WAVETABLEPOS "waveTablePosition"
    static constexpr auto wtpos         = 0.f;

#define PARAM_NOISEGEN "numNoiseGen"
    static constexpr auto npg           = 64.f;

#define PARAM_NOISEJITTER "noiseLevel"
    static constexpr auto nj            = 0.f;

#define PARAM_DAMP "FBlowpass"
    static constexpr auto fblp          = 20000.f;

#define PARAM_FBHP "Fbhighpass"
    static constexpr auto fbhp          = 20.f;

#define PARAM_PHASERSTATE "phaserNotches"
    static constexpr auto pn            = 0.f;

#define PARAM_Q "notchDisperse"
    static constexpr auto apfq          = 0.71f;

#define PARAM_FOLLOWBPM "BPMSync"
    static constexpr bool bpmfollow     = true;

#define PARAM_MANUAL "manual"
    static constexpr bool manual        = false;

#define PARAM_CUTOFF "manualCutoff"
    static constexpr auto cutoff        = 20.f;

    static constexpr auto bpm           = 120.f;

    static constexpr auto minNoiseGen   = 2.f;
    static constexpr auto maxNoiseGen   = 64.f;
    static constexpr auto maxNumNotches = 64;
    static constexpr auto step          = 1024;
};