/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleFlangerAudioProcessor::SimpleFlangerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
	),
	m_apvts(*this, nullptr, juce::Identifier("SimpleFlangerAPVTS"), getParamLayout())
#endif
{
	isFollowBPM = false;

	paramBeginDelay = m_apvts.getParameter(SF_ID(PARAM_BEGIN_DELAY));
	paramDryWet = m_apvts.getParameter(SF_ID(PARAM_DRYWET));
	paramEndDelay = m_apvts.getParameter(SF_ID(PARAM_END_DELAY));
	paramFeedback = m_apvts.getParameter(SF_ID(PARAM_FEEDBACK));
	paramLfoRate = m_apvts.getParameter(SF_ID(PARAM_LFORATE));
	paramProcessedMix = m_apvts.getParameter(SF_ID(PARAM_PROCESSEDMIX));
	paramRawMix = m_apvts.getParameter(SF_ID(PARAM_RAWMIX));
	paramWidePhase = m_apvts.getParameter(SF_ID(PARAM_WIDEPHASE));
	paramWaveTablePos = m_apvts.getParameter(SF_ID(PARAM_WAVETABLEPOS));
	paramNumNoiseGen = m_apvts.getParameter(SF_ID(PARAM_NOISEGEN));
	paramNoiseJitter = m_apvts.getParameter(SF_ID(PARAM_NOISEJITTER));
	paramFeedbackCutoff = m_apvts.getParameter(SF_ID(PARAM_DAMP));
	paramPhaserNotches = m_apvts.getParameter(SF_ID(PARAM_PHASERSTATE));
	paramQ = m_apvts.getParameter(SF_ID(PARAM_Q));
	paramSyncLFORate = dynamic_cast<juce::AudioParameterChoice*>(m_apvts.getParameter(SF_ID(PARAM_SYNCLFORATE)));
	paramLFOFllowBPM = dynamic_cast<juce::AudioParameterBool*>(m_apvts.getParameter(SF_ID(PARAM_FOLLOWBPM)));

	paramBeginDelay->addListener(this);
	paramDryWet->addListener(this);
	paramEndDelay->addListener(this);
	paramFeedback->addListener(this);
	paramLfoRate->addListener(this);
	paramProcessedMix->addListener(this);
	paramRawMix->addListener(this);
	paramWidePhase->addListener(this);
	paramWaveTablePos->addListener(this);
	paramNumNoiseGen->addListener(this);
	paramNoiseJitter->addListener(this);
	paramFeedbackCutoff->addListener(this);
	paramPhaserNotches->addListener(this);
	paramQ->addListener(this);
	paramSyncLFORate->addListener(this);

	paramLFOFllowBPM->addListener(this);
}

SimpleFlangerAudioProcessor::~SimpleFlangerAudioProcessor()
{
}

//==============================================================================
const juce::String SimpleFlangerAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool SimpleFlangerAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool SimpleFlangerAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool SimpleFlangerAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double SimpleFlangerAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int SimpleFlangerAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
	// so this should be at least 1, even if you're not really implementing programs.
}

int SimpleFlangerAudioProcessor::getCurrentProgram()
{
	return 0;
}

void SimpleFlangerAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String SimpleFlangerAudioProcessor::getProgramName(int index)
{
	return {};
}

void SimpleFlangerAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void SimpleFlangerAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	// Use this method as the place to do any pre-playback
	// initialisation that you need..

	simpleFlangerTest.prepare((float)sampleRate, samplesPerBlock);
}

void SimpleFlangerAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SimpleFlangerAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	juce::ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	// Some plugin hosts, such as certain GarageBand versions, will only
	// load plugins that support stereo bus layouts.
	if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
		return false;

	// This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
#endif

	return true;
#endif
}
#endif

void SimpleFlangerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	juce::ScopedNoDenormals noDenormals;
	auto totalNumInputChannels = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();

	// In case we have more outputs than inputs, this code clears any output
	// channels that didn't contain input data, (because these aren't
	// guaranteed to be empty - they may contain garbage).
	// This is here to avoid people getting screaming feedback
	// when they first compile a plugin, but obviously you don't need to keep
	// this code if your algorithm always overwrites all the output channels.
	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
		buffer.clear(i, 0, buffer.getNumSamples());

	// This is the place where you'd normally do the guts of your plugin's
	// audio processing...
	// Make sure to reset the state if your inner loop is processing
	// the samples and the outer loop is handling the channels.
	// Alternatively, you can process the samples with the channels
	// interleaved by keeping the same state.

	if (isFollowBPM) {
		auto head = getPlayHead()->getPosition();
		if (head) {
			auto bpm = head->getBpm();
			if (bpm) {
				simpleFlangerTest.m_lfo.setBPM(*bpm);
			}
		}
	}

	simpleFlangerTest.processceBlock(buffer);
}

//==============================================================================
bool SimpleFlangerAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SimpleFlangerAudioProcessor::createEditor()
{
	return new SimpleFlangerAudioProcessorEditor(*this);
}

//==============================================================================
void SimpleFlangerAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
	auto state = m_apvts.copyState();
	std::unique_ptr<juce::XmlElement> xml(state.createXml());
	copyXmlToBinary(*xml, destData);
}

void SimpleFlangerAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
	std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

	if (xmlState.get() != nullptr)
		if (xmlState->hasTagName(m_apvts.state.getType()))
			m_apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessorValueTreeState::ParameterLayout SimpleFlangerAudioProcessor::getParamLayout() const
{
	juce::AudioProcessorValueTreeState::ParameterLayout layout;

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		SF_ID(PARAM_BEGIN_DELAY),
		PARAM_BEGIN_DELAY,
		juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.3f),
		0.f,
		"hz"
	));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		SF_ID(PARAM_END_DELAY),
		PARAM_END_DELAY,
		juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.3f),
		0.f,
		"hz"
	));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		SF_ID(PARAM_DRYWET),
		PARAM_DRYWET,
		juce::NormalisableRange<float>(0.f, 1.f),
		0.f
	));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		SF_ID(PARAM_FEEDBACK),
		PARAM_FEEDBACK,
		juce::NormalisableRange<float>(-0.99f, 0.99f),
		0.f
	));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		SF_ID(PARAM_LFORATE),
		PARAM_LFORATE,
		juce::NormalisableRange<float>(0.f,10.f,0.002f,0.3f),
		0.f,
		"hz"
	));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		SF_ID(PARAM_PROCESSEDMIX),
		PARAM_PROCESSEDMIX,
		juce::NormalisableRange<float>(-1.f, 1.f),
		0.f
	));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		SF_ID(PARAM_RAWMIX),
		PARAM_RAWMIX,
		juce::NormalisableRange<float>(0.f, 1.f),
		0.f
	));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		SF_ID(PARAM_WIDEPHASE),
		PARAM_WIDEPHASE,
		juce::NormalisableRange<float>(0.f, 360.f),
		0.f,
		"degree"
	));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		SF_ID(PARAM_WAVETABLEPOS),
		PARAM_WAVETABLEPOS,
		juce::NormalisableRange<float>(0.f, 1.f),
		0.f
	));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		SF_ID(PARAM_NOISEGEN),
		PARAM_NOISEGEN,
		juce::NormalisableRange<float>(1.f, 512.f, 1.f, 0.3f),
		64.f
	));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		SF_ID(PARAM_NOISEJITTER),
		PARAM_NOISEJITTER,
		juce::NormalisableRange<float>(0.f, 1.f),
		0.f
	));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		SF_ID(PARAM_DAMP),
		PARAM_DAMP,
		juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.3f),
		20000.f,
		"hz"
	));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		SF_ID(PARAM_PHASERSTATE),
		PARAM_PHASERSTATE,
		juce::NormalisableRange<float>(0.f, MonoAPFArray::kmaxNotches, 1.f),
		0.f
	));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		SF_ID(PARAM_Q),
		PARAM_Q,
		juce::NormalisableRange<float>(0.005f, 10.f, 0.001f, 0.3f),
		0.71f
	));

	layout.add(std::make_unique<juce::AudioParameterChoice>(
		SF_ID(PARAM_SYNCLFORATE),
		PARAM_SYNCLFORATE,
		juce::StringArray({ "8","4","2","1","1/2","1/4","1/8","1/16" }),
		3
	));

	layout.add(std::make_unique<juce::AudioParameterBool>(
		SF_ID(PARAM_FOLLOWBPM),
		PARAM_FOLLOWBPM,
		false
	));

	return layout;
}

void SimpleFlangerAudioProcessor::parameterValueChanged(int parameterIndex, float newValue)
{
	auto& s = simpleFlangerTest;

	if (parameterIndex == paramBeginDelay->getParameterIndex()) {
		auto val = paramBeginDelay->convertFrom0to1(paramBeginDelay->getValue());
		s.m_lfo.m_belowFrequency = val;
	}
	else if (parameterIndex == paramDryWet->getParameterIndex()) {
		s.setDryWet(paramDryWet->getValue());
	}
	else if (parameterIndex == paramEndDelay->getParameterIndex()) {
		auto val = paramEndDelay->convertFrom0to1(paramEndDelay->getValue());
		s.m_lfo.m_upperFrequency = val;
	}
	else if (parameterIndex == paramFeedback->getParameterIndex()) {
		auto val = paramFeedback->convertFrom0to1(paramFeedback->getValue());
		s.m_feedback = val;
	}
	else if (parameterIndex == paramLfoRate->getParameterIndex()) {
		auto val = paramLfoRate->convertFrom0to1(paramLfoRate->getValue());
		s.m_lfo.setRate(val);
	}
	else if (parameterIndex == paramProcessedMix->getParameterIndex()) {
		auto val = paramProcessedMix->convertFrom0to1(paramProcessedMix->getValue());
		s.m_DelayMix = val * 0.5f;
	}
	else if (parameterIndex == paramRawMix->getParameterIndex()) {
		auto val = paramRawMix->convertFrom0to1(paramRawMix->getValue());
		s.m_mix = val * 0.5f;
	}
	else if (parameterIndex == paramWidePhase->getParameterIndex()) {
		s.m_lfo.setPhase(paramWidePhase->getValue());
	}
	else if (parameterIndex == paramWaveTablePos->getParameterIndex()) {
		s.m_lfo.m_wtPos = paramWaveTablePos->getValue();
	}
	else if (parameterIndex == paramNumNoiseGen->getParameterIndex()) {
		auto val = paramNumNoiseGen->convertFrom0to1(paramNumNoiseGen->getValue());
		s.m_lfo.tableTest.genNoise((int)val);
	}
	else if (parameterIndex == paramNoiseJitter->getParameterIndex()) {
		s.m_lfo.m_jitter = paramNoiseJitter->getValue();
	}
	else if (parameterIndex == paramFeedbackCutoff->getParameterIndex()) {
		auto cutoff = paramFeedbackCutoff->convertFrom0to1(paramFeedbackCutoff->getValue());
		s.m_leftDampLPF.setCutOffFrequency(cutoff);
		s.m_rightDampLPF.setCutOffFrequency(cutoff);
	}
	else if (parameterIndex == paramPhaserNotches->getParameterIndex()) {
		auto notch = paramPhaserNotches->convertFrom0to1(paramPhaserNotches->getValue());
		s.m_phaserNotches = notch;
	}
	else if (parameterIndex == paramQ->getParameterIndex()) {
		auto q = paramQ->convertFrom0to1(paramQ->getValue());
		s.m_q = q;
	}
	else if (parameterIndex == paramLFOFllowBPM->getParameterIndex()) {
		isFollowBPM = paramLFOFllowBPM->get();
	}
	else if (parameterIndex == paramSyncLFORate->getParameterIndex()) {
		if (!isFollowBPM) return;

		static std::array<float, 8> vals{8, 4, 2, 1, 1 / 2.f, 1 / 4.f, 1 / 8.f, 1 / 16.f};
		auto val = vals.at(paramSyncLFORate->getIndex());
		s.m_lfo.setBPMRate(val);
	}
}

void SimpleFlangerAudioProcessor::parameterGestureChanged(int parameterIndex, bool gestureIsStarting)
{
	// i don't know how this work
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new SimpleFlangerAudioProcessor();
}