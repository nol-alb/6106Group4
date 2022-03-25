/*
  ==============================================================================

    PluginPluginEditor.h
    Created: 23 Mar 2022 4:26:50am
    Author:  medioq

  ==============================================================================
*/

#ifndef __hdr_PluginPluginEditor_h__
#define __hdr_PluginPluginEditor_h__

#include <JuceHeader.h>

class VSTPluginEditorWrapper : public juce::AudioProcessorEditor {
public:
    void userTriedToCloseWindow() override;
};

#endif // __hdr_PluginPluginEditor_h__
