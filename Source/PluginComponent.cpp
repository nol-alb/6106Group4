/*
  ==============================================================================

    PluginPluginEditor.cpp
    Created: 23 Mar 2022 4:26:50am
    Author:  medioq

  ==============================================================================
*/

#include "PluginComponent.h"
#include "BandComponent.h"

PluginComponent::PluginComponent(NodePtr nodePtr, BandComponent* owner) : parentBandComponent(owner) {
    pluginNodePtr = nodePtr;

    // initialize open editor button
    openEditorButton = std::make_unique<juce::TextButton>("edit");
    openEditorButton->onClick = [this] {
        pluginWindow->setVisible(true);
    };
    addAndMakeVisible(*openEditorButton);

    // intialize delete plugin button
    deleteButton = std::make_unique<juce::TextButton>("x");
    deleteButton->onClick = [this] {
        parentBandComponent->removePluginComponent(this);
    };
    addAndMakeVisible(*deleteButton);

    // initialize plugin window
    pluginWindow = std::make_unique<juce::PluginWindow>(pluginNodePtr.get(), juce::PluginWindow::Type::normal);

}

PluginComponent::~PluginComponent() {
    openEditorButton.reset();
    deleteButton.reset();
    pluginWindow.reset();
}

void PluginComponent::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey);

    g.setColour(juce::Colours::white);
    g.drawRoundedRectangle(0, 0, getWidth(), getHeight(), 5, 1);

    g.setFont(14.0f);
    g.drawText(pluginNodePtr->getProcessor()->getName(), getLocalBounds().reduced(10), juce::Justification::centredLeft, true);
}

void PluginComponent::resized() {
    int w = getWidth();
    int h = getHeight();
    auto bounds = getLocalBounds();
    juce::FlexBox utilFlexBox; // this flex box stores 1. open editor buttion; 2. delete plugin button

    // TODO currently only editor button has been implemented. Delete plugin button will be added later.
    utilFlexBox.items.add(juce::FlexItem(w * unitUtilRegionWidthRatio, h, *openEditorButton));
    utilFlexBox.items.add(juce::FlexItem(w * unitUtilRegionWidthRatio, h, *deleteButton));
    
    bounds.removeFromLeft(w * (1 - unitUtilRegionWidthRatio * utilFlexBox.items.size()));
    utilFlexBox.performLayout(bounds);
}
