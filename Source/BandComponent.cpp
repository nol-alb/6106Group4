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
    pluginComponentViewport.setScrollBarsShown(true, false, true, true);

    __initializeAddPluginButton();
    __updatePluginWrapperComponents();
}

BandComponent::~BandComponent() {
    // pluginLinkedList is not initialized in this class, thus not deleting it
    pluginComponents.clear();
}

void BandComponent::removePluginComponent(const PluginComponent* pluginComponent) {
    int index = pluginComponents.indexOf(pluginComponent);
    pluginComponents.remove(index);
    pluginLinkedList->remove(index);
    __refresh();
}

void BandComponent::__updatePluginWrapperComponents() {
    if (!pluginComponents.isEmpty()) pluginComponents.clear();
    for (auto nodePtr : pluginLinkedList->pluginList) {
        pluginComponents.add(
            new PluginComponent(nodePtr, this)
        );
    }
}

void BandComponent::__initializeAddPluginButton() {
    audioProcessor.updateMenu(pluginMenu);

    addAndMakeVisible(addPluginButton);
    addPluginButton.onClick = [this] {
        pluginMenu.showMenuAsync(
            juce::PopupMenu::Options().withTargetComponent(addPluginButton),
            [this](int res) mutable {
                auto types = audioProcessor.knownPluginList->getTypes();
                int index = juce::KnownPluginList::getIndexChosenByMenu(types, res);
                
                if (0 <= index && index < types.size()) {
                    auto pluginDescription = types[index];
                    pluginLinkedList->append(std::move(audioProcessor.createPlugin(pluginDescription)));
                    __refresh();
                }
            }
        );
    };
}

void BandComponent::paint(juce::Graphics& g) {
    g.setColour(juce::Colours::orange);
    g.drawRoundedRectangle(0, 0, getWidth(), getHeight(), 5, 2);
}

void BandComponent::resized() {
    int w = getWidth();
    int h = getHeight();
    float h_plugin_height = h * pluginHeightRatio;
    float h_button_height = h * buttonHeightRatio;
    auto viewportLocalBounds = getLocalBounds().removeFromTop(h - h_button_height);
    auto buttonLocalBounds = getLocalBounds().removeFromBottom(h_button_height);

    pluginComponentContainer.removeAllChildren();

    juce::FlexBox pluginWrapperComponentFlexBox;
    for (auto wrappedPlugin : pluginComponents) {
        pluginWrapperComponentFlexBox.items.add(juce::FlexItem(w, h_plugin_height, *wrappedPlugin));
        pluginComponentContainer.addAndMakeVisible(*wrappedPlugin);
    }
    pluginWrapperComponentFlexBox.flexDirection = juce::FlexBox::Direction::column;

    auto fullComponentBound = juce::Rectangle<int>(w, h_plugin_height * pluginComponents.size());
    pluginWrapperComponentFlexBox.performLayout(fullComponentBound);
    pluginComponentContainer.setBounds(fullComponentBound);

    pluginComponentViewport.setBounds(viewportLocalBounds);
    pluginComponentViewport.setViewedComponent(&pluginComponentContainer, false);
    addAndMakeVisible(pluginComponentViewport);

    addPluginButton.setBounds(buttonLocalBounds);
}

void BandComponent::__refresh() {
    __updatePluginWrapperComponents();
    resized();
}
