/*
  ==============================================================================

    BandWrapper.cpp
    Created: 21 Apr 2022 3:31:37pm
    Author:  medioqrity

  ==============================================================================
*/

#include "BandComponent.h"

BandComponent::BandComponent(PluginLinkedList* list) {
    pluginLinkedList = list;
    __updatePluginWrapperComponents();
}

BandComponent::~BandComponent() {
    // pluginList is not initialized in this class, thus not deleting it
    pluginWrapperComponents.clear();
}

void BandComponent::__updatePluginWrapperComponents() {
    if (!pluginWrapperComponents.isEmpty()) pluginWrapperComponents.clear();
    for (auto nodePtr : pluginLinkedList->pluginList) {
        addAndMakeVisible(
            pluginWrapperComponents.add(
                new PluginWrapperComponent(nodePtr->getProcessor()->getName())
            )
        );
    }
}

void BandComponent::paint(juce::Graphics& g) {
    g.setColour(juce::Colours::orange);
    g.drawRoundedRectangle(0, 0, getWidth(), getHeight(), 5, 2);
}

void BandComponent::resized() {
    int w = getWidth();
    float h = getHeight() * pluginHeightRatio;
    auto localBounds = getLocalBounds();

    juce::FlexBox pluginWrapperComponentFlexBox;
    for (auto wrappedPlugin : pluginWrapperComponents) {
        pluginWrapperComponentFlexBox.items.add(juce::FlexItem(w, h, *wrappedPlugin));
    }
    pluginWrapperComponentFlexBox.flexDirection = juce::FlexBox::Direction::column;
    pluginWrapperComponentFlexBox.performLayout(localBounds);
}
