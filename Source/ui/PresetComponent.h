/*
  ==============================================================================

	PresetComponent.h
	Created: 19 Mar 2023 7:15:53pm
	Author:  mana

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../PluginProcessor.h"

//==============================================================================
/*
*/
class PresetComponent : public juce::Component, public juce::Button::Listener
{
public:
	explicit PresetComponent(SimpleFlangerAudioProcessor& p);
	~PresetComponent() override = default;

	void resized() override;

	void buttonClicked(juce::Button* button) override;

	auto& getProcessor() const { return m_processorBind; }

private:
	void scanAndSetPresets();

	juce::TextButton m_saveButton;
	juce::TextButton m_loadButton;
	juce::ComboBox m_presetSelecter;

	juce::FileChooser m_saveAndLoadChooser;

	SimpleFlangerAudioProcessor& m_processorBind;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetComponent)
};
