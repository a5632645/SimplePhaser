/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "data/SimplePhaser.h"

#define SF_ID(x) "IDSF_"##x
#define PARAM_BEGIN_DELAY "minFreqeuncy"
#define PARAM_END_DELAY  "maxFrequency"
#define PARAM_FEEDBACK "feedback"
#define PARAM_LFORATE "lfoRate"
#define PARAM_SYNCLFORATE "syncLfoRate"
#define PARAM_DRYWET "dryWet"
#define PARAM_RAWMIX "rawMix"
#define PARAM_PROCESSEDMIX "processedMix"
#define PARAM_WIDEPHASE "widePhase"
#define PARAM_WAVETABLEPOS "waveTablePosition"
#define PARAM_NOISEGEN "numNoiseGen"
#define PARAM_NOISEJITTER "noiseJitter"
#define PARAM_DAMP "feedbackCutoff"
#define PARAM_PHASERSTATE "phaserNotches"
#define PARAM_Q "Q"
#define PARAM_FOLLOWBPM "lfoFollowBPM"

//==============================================================================
/**
*/
class SimpleFlangerAudioProcessor : public juce::AudioProcessor
#if JucePlugin_Enable_ARA
	, public juce::AudioProcessorARAExtension
#endif
	, public juce::AudioProcessorParameter::Listener
{
public:
	//==============================================================================
	SimpleFlangerAudioProcessor();
	~SimpleFlangerAudioProcessor() override;

	//==============================================================================
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

	void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

	//==============================================================================
	juce::AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override;

	//==============================================================================
	const juce::String getName() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool isMidiEffect() const override;
	double getTailLengthSeconds() const override;

	//==============================================================================
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram(int index) override;
	const juce::String getProgramName(int index) override;
	void changeProgramName(int index, const juce::String& newName) override;

	//==============================================================================
	void getStateInformation(juce::MemoryBlock& destData) override;
	void setStateInformation(const void* data, int sizeInBytes) override;

	juce::AudioProcessorValueTreeState::ParameterLayout getParamLayout() const;

	juce::AudioProcessorValueTreeState m_apvts;
	SimplePhaser simpleFlangerTest;
	bool isFollowBPM = true;

	juce::RangedAudioParameter* paramBeginDelay;
	juce::RangedAudioParameter* paramEndDelay;
	juce::RangedAudioParameter* paramFeedback;
	juce::RangedAudioParameter* paramLfoRate;
	juce::RangedAudioParameter* paramDryWet;
	juce::RangedAudioParameter* paramRawMix;
	juce::RangedAudioParameter* paramProcessedMix;
	juce::RangedAudioParameter* paramWidePhase;
	juce::RangedAudioParameter* paramWaveTablePos;
	juce::RangedAudioParameter* paramNumNoiseGen;
	juce::RangedAudioParameter* paramNoiseJitter;
	juce::RangedAudioParameter* paramFeedbackCutoff;
	juce::RangedAudioParameter* paramPhaserNotches;
	juce::RangedAudioParameter* paramQ;
	juce::AudioParameterChoice* paramSyncLFORate;
	juce::AudioParameterBool* paramLFOFllowBPM;

	void parameterValueChanged(int parameterIndex, float newValue) override;
	void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override;
private:
	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleFlangerAudioProcessor)
};
