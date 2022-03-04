/*
  ==============================================================================

    PluginListPopupMenu.cpp
    Created: 6 Feb 2022 8:35:07pm
    Author:  medioqrity

  ==============================================================================
*/

#include "PluginListPopupMenu.h"

PluginListPopupMenu::PluginListPopupMenu(int menuIndex, RecursionTestAudioProcessor& audioProcessor)
    : menuIndex(menuIndex), audioProcessor(audioProcessor) {
    
    textButton = new juce::TextButton("Plugin Slot");
    m = new juce::PopupMenu();

    updateTextButtonPopupMenu();

    addAndMakeVisible(*textButton);
}

PluginListPopupMenu::~PluginListPopupMenu() {
    delete textButton;
    delete m;
}

void PluginListPopupMenu::paint(juce::Graphics& g) {
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void PluginListPopupMenu::resized() {
    auto r = getLocalBounds();
    textButton->setBounds(r.removeFromBottom(28).reduced(2));
}

void PluginListPopupMenu::updateTextButtonPopupMenu() {
    textButton->onClick = [this]{
        m->clear();
        juce::KnownPluginList::addToMenu(
            *m,
            this->audioProcessor.pluginLists->getTypes(),
            juce::KnownPluginList::SortMethod::sortAlphabetically
        );
        m->showMenuAsync(juce::PopupMenu::Options().withTargetComponent(textButton),
            [this](int res) mutable {
                auto types = this->audioProcessor.pluginLists->getTypes();
                int index = juce::KnownPluginList::getIndexChosenByMenu(types, res);
                auto pluginDescription = types[index];
                this->audioProcessor.setPluginAtIndex(
                    this->menuIndex, 
                    pluginDescription
                );
                this->textButton->setButtonText(pluginDescription.name);
            }
        );
    };
}

void PluginListPopupMenu::changeListenerCallback(juce::ChangeBroadcaster*) {
    
}

