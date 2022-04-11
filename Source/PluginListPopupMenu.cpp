/*
  ==============================================================================

    PluginListPopupMenu.cpp
    Created: 6 Feb 2022 8:35:07pm
    Author:  medioqrity

  ==============================================================================
*/

// #include "PluginListPopupMenu.h"
#include "PluginInclude.h"

PluginListPopupMenu::PluginListPopupMenu(int menuIndex, RecursionTestAudioProcessor& audioProcessor)
    : menuIndex(menuIndex), audioProcessor(audioProcessor) {
    
    textButton = new juce::TextButton("Plugin Slot");
    m = new juce::PopupMenu();
    createPluginEditorButton = new juce::TextButton("Open Editor");

    updateTextButtonPopupMenu();
    updateCreatePluginEditorButton();

    addAndMakeVisible(*textButton);
    addAndMakeVisible(*createPluginEditorButton);
}

PluginListPopupMenu::~PluginListPopupMenu() {
    delete textButton;
    delete m;
    delete createPluginEditorButton;
}

void PluginListPopupMenu::paint(juce::Graphics& g) {
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void PluginListPopupMenu::resized() {
    auto r = getLocalBounds();
    textButton->setBounds(r.removeFromRight(r.getWidth() / 2));
    createPluginEditorButton->setBounds(r.removeFromLeft(r.getWidth() / 2));
}

void PluginListPopupMenu::updateTextButtonPopupMenu() {
    m->clear();
    juce::KnownPluginList::addToMenu(
        *m,
        this->audioProcessor.pluginLists->getTypes(),
        juce::KnownPluginList::SortMethod::sortAlphabetically
    );
    textButton->onClick = [this]{
        m->showMenuAsync(juce::PopupMenu::Options().withTargetComponent(textButton),
            [this](int res) mutable {
                auto types = this->audioProcessor.pluginLists->getTypes();
                int index = juce::KnownPluginList::getIndexChosenByMenu(types, res);

                if (0 <= index && index < types.size()) {
                    auto pluginDescription = types[index];
                    this->audioProcessor.setPluginAtIndex(
                        this->menuIndex, 
                        pluginDescription
                    );
                    this->textButton->setButtonText(pluginDescription.name);
                }
            }
        );
    };
}


// class JUCE_API  AudioProcessorEditor : public juce::Component
// {
// public:
//     void userTriedToCloseWindow() override {
//         if (isOnDesktop()) {
//             removeFromDesktop();
//         }
//     }
// };

void PluginListPopupMenu::updateCreatePluginEditorButton() {
    createPluginEditorButton->onClick = [this] {
        juce::AudioProcessorEditor* tempEditor;
        tempEditor = this->audioProcessor.createEditorAtIndex(this->menuIndex);

        if (tempEditor != nullptr) {
            tempEditor->setOpaque(true);
            tempEditor->setVisible(true);
            if (!tempEditor->isOnDesktop()) {
                tempEditor->setTopLeftPosition(100, 100);
                tempEditor->addToDesktop(juce::ComponentPeer::windowHasTitleBar | juce::ComponentPeer::windowHasDropShadow | juce::ComponentPeer::windowHasCloseButton, 0);
                tempEditor->setTitle(this->textButton->getButtonText());
                DBG(this->textButton->getButtonText());
                DBG(tempEditor->getTitle());
            }
        }
        else {
            DBG("SCREWED up");
        }
    };
}

void PluginListPopupMenu::changeListenerCallback(juce::ChangeBroadcaster*) {
    
}

