/*
  ==============================================================================

    BandWrapper.cpp
    Created: 21 Apr 2022 3:31:37pm
    Author:  medioqrity

  ==============================================================================
*/

#include "BandComponent.h"

BandComponent::BandComponent() {
}

BandComponent::~BandComponent() {
}

void BandComponent::paint(juce::Graphics& g) {
    g.setColour(juce::Colours::orange);
    g.drawRoundedRectangle(0, 0, getWidth(), getHeight(), 5, 2);
}

void BandComponent::resized() {
}
