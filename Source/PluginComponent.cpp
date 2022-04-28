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
    g.drawText(pluginName, getLocalBounds().reduced(10), juce::Justification::centredLeft, true);
}

void PluginComponent::resized() {
    int w = getWidth();
    int h = getHeight();
    auto bounds = getLocalBounds();
    juce::FlexBox utilFlexBox; // this flex box stores 1. open editor buttion; 2. delete plugin button

    // TODO currently only editor button has been implemented. Delete plugin button will be added later.
    utilFlexBox.items.add(juce::FlexItem(w * utilRegionWidthRatio, h, *openEditorButton));
    
    bounds.removeFromLeft(w * (1 - utilRegionWidthRatio));
    utilFlexBox.performLayout(bounds);
}
