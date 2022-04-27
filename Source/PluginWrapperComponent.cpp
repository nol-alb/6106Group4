/*
  ==============================================================================

    PluginPluginEditor.cpp
    Created: 23 Mar 2022 4:26:50am
    Author:  medioq

  ==============================================================================
*/

#include "PluginWrapperComponent.h"

PluginWrapperComponent::PluginWrapperComponent(const juce::String& name) {
    pluginName = name; // copy initialization
}

PluginWrapperComponent::~PluginWrapperComponent() {
}

void PluginWrapperComponent::paint(juce::Graphics& g) {
    g.setColour(juce::Colours::blue);
    g.drawRoundedRectangle(0, 0, getWidth(), getHeight(), 5, 1);
}

void PluginWrapperComponent::resized() {

}
