/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleFlangerAudioProcessorEditor::SimpleFlangerAudioProcessorEditor(SimpleFlangerAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p),
	m_presetWindows(p),
	m_phaseViewer(std::bind(&MonoAPFArray::m_cutoff, &audioProcessor.simpleFlangerTest.m_left),
		std::bind(&MonoAPFArray::m_cutoff, &audioProcessor.simpleFlangerTest.m_right)),
	m_waveViewer(audioProcessor.simpleFlangerTest.m_lfo),
	m_beginDelay(audioProcessor.paramBeginDelay),
	m_endDelay(audioProcessor.paramEndDelay),
	m_feedback(audioProcessor.paramFeedback),
	m_lfoRate(audioProcessor.paramLfoRate),
	m_drywet(audioProcessor.paramDryWet),
	m_rawMix(audioProcessor.paramRawMix),
	m_processedMix(audioProcessor.paramProcessedMix),
	m_widePhase(audioProcessor.paramWidePhase),
	m_waveTablePos(audioProcessor.paramWaveTablePos),
	m_numNoiseGen(audioProcessor.paramNumNoiseGen),
	m_noiseJitter(audioProcessor.paramNoiseJitter),
	m_feedbackDamp(audioProcessor.paramFeedbackCutoff),
	m_phaserNotch(audioProcessor.paramPhaserNotches),
	m_notchDisperse(audioProcessor.paramQ),
	m_syncLFORate(audioProcessor.paramSyncLFORate),
	m_followBPM(audioProcessor.paramLFOFllowBPM)
{
	// Make sure that before the constructor has finished, you've set the
	// editor's size to whatever you need it to be.
	
	setSize(620, 510);

	addAndMakeVisible(m_phaseViewer);
	m_phaseViewer.setMaxPhase(20000.f);

	addAndMakeVisible(m_waveViewer);

	addAndMakeVisible(m_presetWindows);

	auto c = juce::Slider::rotarySliderFillColourId;

	addAndMakeVisible(m_beginDelay);
	m_beginDelay.setName("MinFrequency");
	m_beginDelay.setColour(c, juce::Colours::green);
	addAndMakeVisible(m_endDelay);
	m_endDelay.setName("MaxFrequency");
	m_endDelay.setColour(c, juce::Colours::green);
	addAndMakeVisible(m_drywet);
	m_drywet.setName("DryWet");
	m_drywet.setColour(c, juce::Colours::pink);
	addAndMakeVisible(m_feedback);
	m_feedback.setColour(c, juce::Colours::mediumvioletred);
	m_feedback.setName("Feedback");
	addChildComponent(m_lfoRate);
	m_lfoRate.setName("LFO Rate");
	m_lfoRate.setColour(c, juce::Colours::mediumpurple);
	addAndMakeVisible(m_syncLFORate);
	m_syncLFORate.setName("Sync Rate");
	m_syncLFORate.setColour(c, juce::Colours::mediumpurple);
	addAndMakeVisible(m_processedMix);
	m_processedMix.setName("ProcessedMix");
	m_processedMix.setColour(c, juce::Colours::pink);
	addAndMakeVisible(m_rawMix);
	m_rawMix.setName("UnProcMix");
	m_rawMix.setColour(c, juce::Colours::pink);
	addAndMakeVisible(m_widePhase);
	m_widePhase.setName("LR Phase");
	m_widePhase.setColour(c, juce::Colours::mediumpurple);
	addAndMakeVisible(m_waveTablePos);
	m_waveTablePos.setName("LFO Shape");
	m_waveTablePos.setColour(c, juce::Colours::mediumpurple);
	m_waveTablePos.onValueChange = [this]() { m_waveViewer.repaint(); };
	addAndMakeVisible(m_numNoiseGen);
	m_numNoiseGen.setColour(c, juce::Colours::darkcyan);
	m_numNoiseGen.setName("Num Noise Point");
	m_numNoiseGen.onValueChange = [this]() { m_waveViewer.repaint(); };
	addAndMakeVisible(m_noiseJitter);
	m_noiseJitter.setColour(c, juce::Colours::darkcyan);
	m_noiseJitter.setName("Noise Jitter");
	m_noiseJitter.onValueChange = [this]() { m_waveViewer.repaint(); };
	addAndMakeVisible(m_feedbackDamp);
	m_feedbackDamp.setName("Damp");
	m_feedbackDamp.setColour(c, juce::Colours::mediumvioletred);
	addAndMakeVisible(m_phaserNotch);
	m_phaserNotch.setName("Notches");
	m_phaserNotch.setColour(c, juce::Colours::yellowgreen);
	addAndMakeVisible(m_notchDisperse);
	m_notchDisperse.setName("Notch Disperse");
	m_notchDisperse.setColour(c, juce::Colours::yellowgreen);

	addAndMakeVisible(m_followBPM);
	m_followBPM.setButtonText("Sync Bpm");
	m_followBPM.onStateChange = [this]() {
		m_syncLFORate.setVisible(audioProcessor.isFollowBPM);
		m_lfoRate.setVisible(!audioProcessor.isFollowBPM);

		if (audioProcessor.isFollowBPM) {
			audioProcessor.parameterValueChanged(audioProcessor.paramSyncLFORate->getParameterIndex(), 0.f);
		}
		else {
			audioProcessor.parameterValueChanged(audioProcessor.paramLfoRate->getParameterIndex(), 0.f);
		}
	};
}

//==============================================================================
void SimpleFlangerAudioProcessorEditor::paint(juce::Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.setColour(juce::Colour(50, 62, 68));
	g.fillAll();
}

void SimpleFlangerAudioProcessorEditor::resized()
{
	// This is generally where you'll want to lay out the positions of any
	// subcomponents in your editor..
	m_presetWindows.setBounds(0, 0, getWidth(), 30);

	m_phaseViewer.setBounds(0, 30, getWidth(), 40);

	// frequency controls
	m_beginDelay.setBounds(0, 90, 100, 100);
	m_endDelay.setBounds(0, 200, 100, 100);

	// mix controls
	m_rawMix.setBounds(120, 90, 80, 80);
	m_processedMix.setBounds(120, 180, 80, 80);
	m_drywet.setBounds(120, 270, 80, 80);

	// notch controls
	m_phaserNotch.setBounds(210, 90, 100, 100);
	m_notchDisperse.setBounds(210, 200, 100, 100);

	// feedback controls
	m_feedback.setBounds(320, 90, 100, 100);
	m_feedbackDamp.setBounds(320, 200, 100, 100);

	// lfo basic controls
	m_waveTablePos.setBounds(430, 90, 80, 80);
	m_lfoRate.setBounds(430, 270, 80, 80);
	m_syncLFORate.setBounds(430, 270, 80, 80);
	m_widePhase.setBounds(430, 180, 80, 80);
	m_followBPM.setBounds(520, 330, 100, 30);

	// noise controls
	m_noiseJitter.setBounds(520, 90, 100, 100);
	m_numNoiseGen.setBounds(520, 200, 100, 100);

	m_waveViewer.setBounds(0, 360, getWidth(), 150);
}