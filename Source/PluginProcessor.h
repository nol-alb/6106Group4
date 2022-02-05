﻿/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

    The plugin first creates a AudioProcessorGraph, which is a child of 
    AudioProcessor (it's composite pattern).  There will be 
    AudioProcessorGraph::Node in the graph as vertices.  These vertices contains 
    AudioProcessor* points to real VST instances.
j
    Since it's composite pattern, we can recursively call 
    AudioProcessor*->processBlock() to process.

    The VST instances can be created using 
    AudioPluginFormatManager.createPluginInstance().

    The list of known plugins can be scanned using 
    KnownPluginList.scanAndAddFile() and KnownPluginList.getTypeForFile().

    To list plugins, there's a class called PluginListComponent.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class RecursionTestAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    RecursionTestAudioProcessor();
    ~RecursionTestAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //==============================================================================
    juce::AudioProcessorGraph *graph; /* the graph that stores all instances and their connections.
                                     Here is an overview of how it looks:
                         ┌─────┐                           ┌───────────────┐
                         │INPUT│                           │SIDECHAIN INPUT│
                         └──┬──┘                           └───────┬───────┘
                            │                                      │
                    ┌───────▼────────┐                             │
                    │Pre Chain: VST 0│                             │
                    └───────┬────────┘                             |
                            │                                      │
                    ┌───────▼────────┐                             │
                    │Pre Chain: VST 1│                             │
                    └───────┬────────┘                             │
                           ...                                     │
                   ┌────────▼─────────┐                            │
       ┌───────────┤Frequency Splitter├───────────┐                │
       │           └────────┬─────────┘           │                │
       │                    │                     │                │
┌──────▼───────┐     ┌──────▼───────┐     ┌───────▼──────┐         │
│Chain 0: VST 0│     │Chain 1: VST 0│     │Chain 2: VST 0│         │
└──────┬───────┘     └──────┬───────┘     └───────┬──────┘         │
       │                    │                     │                │
┌──────▼───────┐     ┌──────▼───────┐     ┌───────▼──────┐         │
│Chain 0: VST 1│     │Chain 1: VST 1│     │Chain 2: VST 1◄─────────┘
└──────┬───────┘     └──────┬───────┘     └───────┬──────┘
       │                    │                     │
┌──────▼───────┐     ┌──────▼───────┐     ┌───────▼──────┐
│Chain 0: VST 2│     │Chain 1: VST 2│     │Chain 2: VST 2│
└──────┬───────┘     └──────┬───────┘     └───────┬──────┘
      ...                  ...                   ...
┌──────▼───────┐     ┌──────▼───────┐     ┌───────▼──────┐
│   Utility    │     │   Utility    │     │   Utility    │
└──────┬───────┘     └──────┬───────┘     └───────┬──────┘
       │                    │                     │
       │                 ┌──▼───┐                 │
       └─────────────────►Merger◄─────────────────┘
                         └──┬───┘
                            │
                    ┌───────▼─────────┐
                    │Post Chain: VST 0│
                    └───────┬─────────┘
                            │
                    ┌───────▼─────────┐
                    │Post Chain: VST 1│
                    └───────┬─────────┘
                           ...
                         ┌──▼───┐
                         │OUTPUT│
                         └──────┘
    */
    
    //==============================================================================
    juce::KnownPluginList *pluginList;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RecursionTestAudioProcessor)
};
