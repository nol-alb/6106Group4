/*
  ==============================================================================

    PluginLinkedList.cpp
    Created: 25 Feb 2022 11:57:10pm
        
    Author:  medioq

  ==============================================================================
*/

#include <cassert>
#include "PluginLinkedList.h"

PluginLinkedList::PluginLinkedList()
    : AudioProcessor(BusesProperties().withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                                      .withOutput ("Output", juce::AudioChannelSet::stereo(), true)) {
}
PluginLinkedList::~PluginLinkedList() {
    graph.clear();
}

void PluginLinkedList::append(std::unique_ptr<AudioProcessor> processor) {
    pluginList.add(__insertProcessor(std::move(processor)));
}

// void PluginLinkedList::appendLeft(std::unique_ptr<AudioProcessor> processor) {
// }
// 
// void PluginLinkedList::insert(nodePtr ptr, int index) {
// }
// 
// PluginLinkedList::nodePtr PluginLinkedList::pop() {
// }
// 
// 
// PluginLinkedList::nodePtr PluginLinkedList::popLeft() {
// }
// 
// PluginLinkedList::nodePtr PluginLinkedList::remove(int index) {
// }
// 
// PluginLinkedList::nodePtr PluginLinkedList::get(int index) {
// }

long long PluginLinkedList::size() const {
    return pluginList.size();
}

const juce::String PluginLinkedList::getName() const
{
    return juce::String();
}

bool PluginLinkedList::isBusesLayoutSupported(const BusesLayout& layouts) const {
    if (layouts.getMainInputChannelSet()  == juce::AudioChannelSet::disabled()
     || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::disabled())
        return false;
 
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
 
    return layouts.getMainInputChannelSet() == layouts.getMainOutputChannelSet();
}

void PluginLinkedList::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
    setPlayConfigDetails(
        getMainBusNumInputChannels(),
        getMainBusNumOutputChannels(),
        sampleRate,
        maximumExpectedSamplesPerBlock
    );

    graph.setPlayConfigDetails(
        getMainBusNumInputChannels(),
        getMainBusNumOutputChannels(),
        sampleRate,
        maximumExpectedSamplesPerBlock
    );
    graph.prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);

    initGraph();
}

void PluginLinkedList::releaseResources()
{
    graph.releaseResources();
}

void PluginLinkedList::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // clean junk channels
    for (auto i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i) {
        buffer.clear(i, 0, buffer.getNumSamples());
    }

    updateGraph();

    graph.processBlock(buffer, midiMessages);
}

double PluginLinkedList::getTailLengthSeconds() const
{
    return 0.0;
}

bool PluginLinkedList::acceptsMidi() const
{
    return false;
}

bool PluginLinkedList::producesMidi() const
{
    return false;
}

juce::AudioProcessorEditor* PluginLinkedList::createEditor()
{
    return nullptr;
}

bool PluginLinkedList::hasEditor() const
{
    return false;
}

int PluginLinkedList::getNumPrograms()
{
    return 0;
}

int PluginLinkedList::getCurrentProgram()
{
    return 0;
}

void PluginLinkedList::setCurrentProgram(int index)
{
}

const juce::String PluginLinkedList::getProgramName(int index)
{
    return juce::String();
}

void PluginLinkedList::changeProgramName(int index, const juce::String& newName)
{
}

void PluginLinkedList::getStateInformation(juce::MemoryBlock& destData)
{
}

void PluginLinkedList::setStateInformation(const void* data, int sizeInBytes)
{
}
void PluginLinkedList::initGraph() {
    for (auto connection : graph.getConnections()) {
        graph.removeConnection(connection);
    }
    if (audioInputNode  != nullptr) graph.removeNode(audioInputNode->nodeID);
    if (audioOutputNode != nullptr) graph.removeNode(audioOutputNode->nodeID);

    audioInputNode  = graph.addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::audioInputNode));
    audioOutputNode = graph.addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::audioOutputNode));
    
    __connect(audioInputNode, audioOutputNode);
}

void PluginLinkedList::updateGraph() {
    if (!pluginList.isEmpty()) {
        for (auto connection : graph.getConnections()) {
            graph.removeConnection(connection);
        }

        NodePtr lastNode = audioInputNode;
        for (auto node : pluginList) {
            node->getProcessor()->setPlayConfigDetails(getMainBusNumInputChannels(),
                                                       getMainBusNumOutputChannels(),
                                                       getSampleRate(), getBlockSize());
            __connect(lastNode, node);
            lastNode = node;
        }

        __connect(lastNode, audioOutputNode);

        for (auto node : graph.getNodes()) {
            node->getProcessor()->enableAllBuses();
        }
    }
}

void PluginLinkedList::__connect(NodePtr a, NodePtr b) {
    for (int i = 0; i < 2; ++i) {
        graph.addConnection(__gen_connection(a, b, i));
    }
}

juce::AudioProcessorGraph::Connection
PluginLinkedList::__gen_connection(NodePtr a, NodePtr b, int i) {
    return AudioProcessorGraph::Connection({
        AudioProcessorGraph::NodeAndChannel{ a->nodeID, i },
        AudioProcessorGraph::NodeAndChannel{ b->nodeID, i }
    });
}

PluginLinkedList::NodePtr PluginLinkedList::__insertProcessor(std::unique_ptr<AudioProcessor> processor) {
    return graph.addNode(std::move(processor));
}
