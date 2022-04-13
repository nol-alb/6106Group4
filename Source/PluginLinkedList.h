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
    using nodePtr = juce::AudioProcessorGraph::Node::Ptr;
    using nodeAndChannel = juce::AudioProcessorGraph::NodeAndChannel;
public:
    PluginLinkedList();
    virtual ~PluginLinkedList();
 
    void append(nodePtr);
    void appendLeft(nodePtr);
    
    nodePtr pop();
    nodePtr popLeft();

    void insert(nodePtr, int);
    nodePtr remove(int);
    nodePtr get(int);

    long long size() const;

private:

    void __connect(nodePtr a, nodePtr b);
    void __disconnect(nodePtr a, nodePtr b);

    void __insert(nodePtr a, nodePtr b, nodePtr c); // originally a -> b, then insert c, such that a -> c -> b
    void __remove(nodePtr a, nodePtr b, nodePtr c); // originally a -> b -> c, then remove b, such that a -> c

    void __indexWrapper(int&);

    juce::AudioProcessorGraph::Connection __genConnection(nodePtr a, nodePtr b, int channelIndex);

    nodePtr audioInputNode, audioOutputNode;

    juce::AudioProcessorGraph graph;
    std::list<nodePtr> nodeLinkedList;

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
};


// class PluginLinkedList: public juce::AudioProcessor {
// 
// public:
// 
//     class Node: public juce::ReferenceCountedObject {
//     public:
//         Node(std::unique_ptr<AudioProcessor> p);
// 
//         virtual ~Node();
// 
//         void set(std::unique_ptr<AudioProcessor> p);
// 
//         void clear();
// 
//         using Ptr = juce::ReferenceCountedObjectPtr<Node>;
//         Node::Ptr prev = nullptr, next = nullptr; // two empty node for coding convenience
//         std::unique_ptr<AudioProcessor> processor;
//     };
// 
//     PluginLinkedList();
//     virtual ~PluginLinkedList();
// 
//     void append(Node::Ptr);
//     void appendLeft(Node::Ptr);
//     void insert(Node::Ptr, int);
//     Node::Ptr remove(int);
//     Node::Ptr get(int);
// 
//     juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter();
// 
//     // Inherited via AudioProcessor
//     double getTailLengthSeconds() const override;
//     bool acceptsMidi() const override;
//     bool producesMidi() const override;
//     int getNumPrograms() override;
//     int getCurrentProgram() override;
//     void setCurrentProgram(int index) override;
//     const juce::String getProgramName(int index) override;
//     void changeProgramName(int index, const juce::String& newName) override;
//     void getStateInformation(juce::MemoryBlock& destData) override;
//     void setStateInformation(const void* data, int sizeInBytes) override;
//     const juce::String getName() const override;
//     void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
//     void releaseResources() override;
//     void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
//     juce::AudioProcessorEditor* createEditor() override;
//     bool hasEditor() const override;
// 
// private:
//     int indexWrapper(int);
//     void __connect(Node::Ptr a, Node::Ptr b); // will be used in both insert & remove
//     void __insert(Node::Ptr a, Node::Ptr b, Node::Ptr c); // originally a -> b, then insert c, such that a -> c -> b
// 
//     int __len = 0;
//     Node::Ptr head = nullptr, tail = nullptr;
// 
// };

#endif
