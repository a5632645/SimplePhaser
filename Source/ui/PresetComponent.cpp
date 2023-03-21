/*
  ==============================================================================

	PresetComponent.cpp
	Created: 19 Mar 2023 7:15:53pm
	Author:  mana

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PresetComponent.h"

static juce::File getPresetFolder() {
	auto userFolder = juce::File::getSpecialLocation(juce::File::SpecialLocationType::userApplicationDataDirectory);
	auto f = userFolder.getChildFile(ProjectInfo::projectName);
	if (!f.exists()) f.createDirectory();
	auto presetFloder = f.getChildFile("preset");
	if (!presetFloder.exists()) presetFloder.createDirectory();

	return presetFloder;
}

static juce::File findPreset(const juce::String& nameWithoutExtension) {
	auto presetFiles = getPresetFolder().findChildFiles(
		juce::File::TypesOfFileToFind::findFiles,
		false,
		"*.xml"
	);

	for (auto const& f : presetFiles) {
		if (f.getFileNameWithoutExtension() == nameWithoutExtension) {
			return f;
		}
	}

	return juce::File();
}

//==============================================================================
PresetComponent::PresetComponent(SimpleFlangerAudioProcessor& p)
	:m_saveAndLoadChooser("Load or Save", getPresetFolder(), "*.xml"),
	m_processorBind(p)
{
	// In your constructor, you should add any child components, and
	// initialise any special settings that your component needs.
	addAndMakeVisible(m_loadButton);
	m_loadButton.setButtonText("Load");
	m_loadButton.addListener(this);
	addAndMakeVisible(m_saveButton);
	m_saveButton.setButtonText("Save");
	m_saveButton.addListener(this);
	addAndMakeVisible(m_presetSelecter);
	m_presetSelecter.setTextWhenNoChoicesAvailable("None");
	m_presetSelecter.setTextWhenNothingSelected("custom");
	m_presetSelecter.onChange = [this]() {
		auto f = findPreset(m_presetSelecter.getText());
		if (f == juce::File()) {
			// hide this choice due to i can not delete it
			m_presetSelecter.setItemEnabled(m_presetSelecter.getSelectedId(), false);
			return;
		}
		this->getProcessor().m_apvts.replaceState(juce::ValueTree::fromXml(*juce::XmlDocument::parse(f)));
	};

	scanAndSetPresets();
}

void PresetComponent::resized()
{
	// This method is where you should set the bounds of any child
	// components that your component contains..
	m_saveButton.setBounds(0, 0, 50, getHeight());
	m_loadButton.setBounds(m_saveButton.getRight(), 0, 50, getHeight());
	m_presetSelecter.setBounds(m_loadButton.getRight(), 0, getWidth() - m_loadButton.getRight(), getHeight());
}

void PresetComponent::buttonClicked(juce::Button* button)
{
	using flags = juce::FileBrowserComponent::FileChooserFlags;

	if (button == &m_loadButton) {
		m_saveAndLoadChooser.launchAsync(
			flags::openMode,
			[this](juce::FileChooser const& fc) {
				auto presetToLoad = fc.getResult();
				this->scanAndSetPresets();
				if (!presetToLoad.exists()) return;
				this->getProcessor().m_apvts.replaceState(juce::ValueTree::fromXml(*juce::XmlDocument::parse(presetToLoad)));
			}
		);
	}
	else if (button == &m_saveButton) {
		m_saveAndLoadChooser.launchAsync(
			flags::saveMode | flags::warnAboutOverwriting,
			[this](juce::FileChooser const& fc) {
				auto presetToSave = fc.getResult();
				this->scanAndSetPresets();
				if (presetToSave == juce::File{}) return;
				auto state = this->getProcessor().m_apvts.copyState();
				state.createXml()->writeTo(presetToSave);
				// saved and set combox to this name
				this->scanAndSetPresets();
				m_presetSelecter.setText(presetToSave.getFileNameWithoutExtension());
			}
		);
	}
}

void PresetComponent::scanAndSetPresets()
{
	m_presetSelecter.clear();

	auto presetFiles = getPresetFolder().findChildFiles(
		juce::File::TypesOfFileToFind::findFiles,
		false,
		"*.xml"
	);

	int id = 1;
	for (auto const& f : presetFiles) {
		m_presetSelecter.addItem(f.getFileNameWithoutExtension(), id);
		id++;
	}
}
