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

class PluginLinkedList: public juce::AudioProcessor {

public:

    class Node: public juce::ReferenceCountedObject {
    public:
        using Ptr = juce::ReferenceCountedObjectPtr<Node>;
        Node::Ptr prev = nullptr, next = nullptr; // two empty node for coding convenience
        std::unique_ptr<AudioProcessor> processor;
    };

    PluginLinkedList();
    virtual ~PluginLinkedList();

    void append(Node::Ptr);
    void appendLeft(Node::Ptr);
    void insert(Node::Ptr, int);
    Node::Ptr remove(int);
    Node::Ptr get(int);

    juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter();

private:
    int indexWrapper(int);
    void __connect(Node::Ptr a, Node::Ptr b); // will be used in both insert & remove
    void __insert(Node::Ptr a, Node::Ptr b, Node::Ptr c); // originally a -> b, then insert c, such that a -> c -> b

    int __len = 0;
    Node::Ptr head = nullptr, tail = nullptr;

    // Inherited via AudioProcessor
    double getTailLengthSeconds() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;
    const juce::String getName() const override;
    void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
};

#endif
