/*
  ==============================================================================

    PluginPluginEditor.cpp
    Created: 23 Mar 2022 4:26:50am
    Author:  medioq

  ==============================================================================
*/

#include "PluginComponent.h"

PluginComponent::PluginComponent(const juce::String& name) {
    pluginName = name; // copy initialization

    // add button
    openEditorButton = std::make_unique<juce::TextButton>("edit");
    addAndMakeVisible(*openEditorButton);

    openEditorButton->onClick = [this] {
        
    };
}

PluginComponent::~PluginComponent() {
    openEditorButton.reset();
}

void PluginComponent::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey);

    g.setColour(juce::Colours::white);
    g.drawRoundedRectangle(0, 0, getWidth(), getHeight(), 5, 1);

    g.setFont(14.0f);
    g.drawText(pluginName, getLocalBounds(), juce::Justification::centred, true);
}

void PluginComponent::resized() {

}
