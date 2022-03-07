/*
  ==============================================================================

    PluginListPopupMenu.h
    Created: 6 Feb 2022 8:35:07pm
    Author:  medioqrity

  ==============================================================================
*/

#ifndef __hdr_PluginListPopupMenu_h__
#define __hdr_PluginListPopupMenu_h__

#include <JuceHeader.h>
#include "PluginProcessor.h"

class PluginListPopupMenu : public juce::Component, 
                            public juce::ChangeListener {
public:
    PluginListPopupMenu(int, RecursionTestAudioProcessor &);
    virtual ~PluginListPopupMenu(); // do I really need virtual here...?

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    //==============================================================================
    void changeListenerCallback(juce::ChangeBroadcaster*) override;

private:
    void updateTextButtonPopupMenu();
    void updateCreatePluginEditorButton();

    juce::TextButton* textButton;
    juce::PopupMenu* m;
    juce::TextButton* createPluginEditorButton;

    juce::AudioProcessorEditor* editor;
    RecursionTestAudioProcessor& audioProcessor;
    int menuIndex;
};

#endif // __hdr_PluginListPopupMenu_h__
