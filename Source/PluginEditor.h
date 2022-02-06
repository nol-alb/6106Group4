/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#ifndef __hdr_PluginEditor_h__
#define __hdr_PluginEditor_h__

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class RecursionTestAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    RecursionTestAudioProcessorEditor (RecursionTestAudioProcessor&);
    ~RecursionTestAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    RecursionTestAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RecursionTestAudioProcessorEditor)
};

#endif // __hdr_PluginEditor_h__
