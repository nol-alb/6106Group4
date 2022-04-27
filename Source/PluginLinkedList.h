/*
  ==============================================================================

    PluginLinkedList.h
    Created: 25 Feb 2022 11:57:10pm
    Author:  medioq

  ==============================================================================
*/

#ifndef __hdr_PluginLinkedList_h__
#define __hdr_PluginLinkedList_h__

#include <JuceHeader.h>

class PluginLinkedList : public juce::AudioProcessor {
public:
    PluginLinkedList();
    virtual ~PluginLinkedList();
 
    void append(std::unique_ptr<AudioProcessor> processor);
    // void appendLeft(std::unique_ptr<AudioProcessor> processor);
    
    // nodePtr pop();
    // nodePtr popLeft();

    // void insert(nodePtr, int);
    // nodePtr remove(int);
    // nodePtr get(int);

    long long size() const;

    // Inherited via AudioProcessor
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    virtual const juce::String getName() const override;
    virtual void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
    virtual void releaseResources() override;
    virtual void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
    virtual double getTailLengthSeconds() const override;
    virtual bool acceptsMidi() const override;
    virtual bool producesMidi() const override;
    virtual juce::AudioProcessorEditor* createEditor() override;
    virtual bool hasEditor() const override;
    virtual int getNumPrograms() override;
    virtual int getCurrentProgram() override;
    virtual void setCurrentProgram(int index) override;
    virtual const juce::String getProgramName(int index) override;
    virtual void changeProgramName(int index, const juce::String& newName) override;
    virtual void getStateInformation(juce::MemoryBlock& destData) override;
    virtual void setStateInformation(const void* data, int sizeInBytes) override;

private:

    using AudioProcessorGraph = juce::AudioProcessorGraph;
    using AudioGraphIOProcessor = AudioProcessorGraph::AudioGraphIOProcessor;
    using Node = juce::AudioProcessorGraph::Node;
    using NodePtr = Node::Ptr;

    //==============================================================================
    juce::ReferenceCountedArray<Node> pluginList; // currently only one list
    AudioProcessorGraph graph;
    NodePtr audioInputNode = nullptr, audioOutputNode = nullptr;

    //==============================================================================
    void initGraph();
    void updateGraph();

    void __connect(NodePtr a, NodePtr b);
    AudioProcessorGraph::Connection __gen_connection(NodePtr a, NodePtr b, int i);
    // there's no disconnect!~ we will use graph.clear() / graph.removeConnection() to clear everything

    //==============================================================================
    NodePtr __insertProcessor(std::unique_ptr<AudioProcessor> processor);

    //==============================================================================
    bool needToReconstructGraph = false;

    friend class BandComponent;
};

#endif
