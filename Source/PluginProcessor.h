/*
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

    The list of known plugins can be scanned using PluginDirectoryScanner.

    To list plugins, there's a class called PluginListComponent.

  ==============================================================================
*/

#ifndef __hdr_PluginProcessor_h__
#define __hdr_PluginProcessor_h__

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

    //==============================================================================
    void setPluginAtIndex(int index, juce::PluginDescription& pluginDescription);

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
    const int numPluginMenu = 3;

    // TODO these stuff should not be managed by plugin processor
    juce::AudioPluginFormatManager *audioPluginFormatManager;
    juce::KnownPluginList *pluginLists;

    // the plugin id chain that maintains the order of each plugin.
    std::vector<juce::AudioProcessorGraph::NodeID> pluginNodeIDChain;

    // the file that will be used in plugin scan to detect if plugin is dead.
    const juce::File deadVSTFiles = juce::File();

    // placeholder nodeID to reserve space for the plugin chain.
    juce::AudioProcessorGraph::NodeID emptyNode { UINT32_MAX };

    // the plugin format to be scanned. Should be released after called delete.
    juce::VST3PluginFormat *pluginFormatToScan = new juce::VST3PluginFormat();

    friend class RecursionTestAudioProcessorEditor;
    friend class PluginListPopupMenu;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RecursionTestAudioProcessor)
};

#endif // __hdr_PluginProcessor_h__
