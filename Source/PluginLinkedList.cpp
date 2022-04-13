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
    using AudioGraphIOProcessor = juce::AudioProcessorGraph::AudioGraphIOProcessor;
    audioInputNode = graph.addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::audioInputNode));
    audioOutputNode = graph.addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::audioOutputNode));

    __connect(audioInputNode, audioOutputNode);
}
PluginLinkedList::~PluginLinkedList() {
    nodeLinkedList.clear();
}

void PluginLinkedList::append(nodePtr ptr) {
    nodePtr last = (!nodeLinkedList.empty()) ? nodeLinkedList.back() : audioInputNode;
    nodeLinkedList.emplace_back(ptr);
    __insert(last, audioOutputNode, ptr); // insert ptr between last and audioOutputNode
}

void PluginLinkedList::appendLeft(nodePtr ptr) {
    nodePtr head = (!nodeLinkedList.empty()) ? nodeLinkedList.front() : audioOutputNode;
    nodeLinkedList.emplace_front(ptr);
    __insert(audioInputNode, head, ptr); // insert ptr between audioInputNode and head
}

void PluginLinkedList::insert(nodePtr ptr, int index) {
    // insert ptr, so that it's index is index
    __indexWrapper(index);

    if (index == 0) {
        appendLeft(ptr);
    }
    else if (index == nodeLinkedList.size()) {
        append(ptr);
    }
    else {
        auto l_front = nodeLinkedList.begin();
        
        // not using PluginLinkedList::get(int) because it's not returning iterator
        // if use get() then we have to run through 2 * index nodes
        std::advance(l_front, index - 1);
        nodePtr left = *l_front;

        std::advance(l_front, 1);
        nodePtr right = *l_front;

        __insert(left, right, ptr);
        nodeLinkedList.insert(l_front, ptr);
    }
}

PluginLinkedList::nodePtr PluginLinkedList::pop() {
    if (!nodeLinkedList.empty()) {
        nodePtr last = nodeLinkedList.back(); nodeLinkedList.pop_back();
        nodePtr last_prev = (!nodeLinkedList.empty()) ? nodeLinkedList.back() : audioInputNode;
        __remove(last_prev, last, audioOutputNode); // remove last and connect last_prev and audioOutputNode
        return last;
    }

    // otherwise we are trying to pop from empty container & graph
    return nullptr; // should we throw exception here?
}


PluginLinkedList::nodePtr PluginLinkedList::popLeft() {
    if (!nodeLinkedList.empty()) {
        nodePtr head = nodeLinkedList.front(); nodeLinkedList.pop_front();
        nodePtr head_next = (!nodeLinkedList.empty()) ? nodeLinkedList.front() : audioOutputNode;
        __remove(audioInputNode, head, head_next); // remove head and connect audioInputNode and head_next
        return head;
    }

    return nullptr;
}

PluginLinkedList::nodePtr PluginLinkedList::remove(int index) {
    // sanity check
    assert(0 <= index && index < nodeLinkedList.size());

    if (index == 0) {
        return popLeft();
    }
    else if (index == nodeLinkedList.size() - 1) {
        return pop();
    }
    else {
        auto l_front = nodeLinkedList.begin();
        
        // not using PluginLinkedList::get(int) because it's not returning iterator
        // if use get() then we have to run through 2 * index nodes
        std::advance(l_front, index - 1);
        nodePtr left = *l_front;

        std::advance(l_front, 1);
        nodePtr target = *l_front;
    
        std::advance(l_front, 1);
        nodePtr right = *l_front;

        __disconnect(left, target);
        __disconnect(target, right);
        __connect(left, right);

        nodeLinkedList.remove(target);
    }
}

PluginLinkedList::nodePtr PluginLinkedList::get(int index) {
    auto l_front = nodeLinkedList.begin();
    std::advance(l_front, index);
    return *l_front;
}

juce::AudioProcessorGraph::Connection PluginLinkedList::__genConnection(nodePtr a, nodePtr b, int channelIndex) {
    return juce::AudioProcessorGraph::Connection(
        nodeAndChannel{ a->nodeID, channelIndex }, nodeAndChannel{ b->nodeID, channelIndex }
    );
}

void PluginLinkedList::__insert(nodePtr a, nodePtr b, nodePtr c) {
    __disconnect(a, b);
    __connect(a, c);
    __connect(c, b);
}

void PluginLinkedList::__remove(nodePtr a, nodePtr b, nodePtr c) {
    __disconnect(a, b);
    __disconnect(b, c);
    __connect(a, c);
}

void PluginLinkedList::__connect(nodePtr a, nodePtr b) {
    for (int i = 0; i < 2; ++i) {
        graph.addConnection(__genConnection(a, b, i));
    }
}

void PluginLinkedList::__disconnect(nodePtr a, nodePtr b) {
    for (int i = 0; i < 2; ++i) {
        graph.removeConnection(__genConnection(a, b, i));
    }
}

void PluginLinkedList::__indexWrapper(int& index) {
    auto size = nodeLinkedList.size();
    while (index < 0) index += size;
    while (index > size) index -= size; // not index >= size because insert(size) means append
}

long long PluginLinkedList::size() const {
    return nodeLinkedList.size();
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
    graph.setPlayConfigDetails (getMainBusNumInputChannels(),
                                getMainBusNumOutputChannels(),
                                sampleRate, maximumExpectedSamplesPerBlock);
    graph.prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);
}

void PluginLinkedList::releaseResources()
{
    graph.releaseResources();
}

void PluginLinkedList::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // clean up unused channels
    for (auto i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i) {
        buffer.clear(i, 0, buffer.getNumSamples());
    }

    for (auto node : nodeLinkedList) {
        node->getProcessor()->setPlayConfigDetails(
            getMainBusNumInputChannels(),
            getMainBusNumOutputChannels(),
            getSampleRate(),
            getBlockSize()
        );
    }

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
// PluginLinkedList::Node::Node(std::unique_ptr<juce::AudioProcessor> p) {
//     processor = std::move(p);
// }
// 
// PluginLinkedList::Node::~Node() {
//     clear();
// }
// 
// void PluginLinkedList::Node::clear() {
//     if (processor != nullptr) {
//         auto editor = processor->getActiveEditor();
//         if (editor != nullptr) {
//             if (editor->isOnDesktop()) {
//                 editor->removeFromDesktop();
//             }
//             delete editor;
//             editor = nullptr;
//         }
//         processor.reset();
//     }
// }
// 
// void PluginLinkedList::Node::set(std::unique_ptr<juce::AudioProcessor> p) {
//     clear();
//     processor = std::move(p);
// }
// 
// PluginLinkedList::PluginLinkedList() {
//     head = new Node(nullptr);
//     tail = new Node(nullptr);
//     head->next = tail;
//     tail->prev = head;
// }
// 
// /*
// Since Node is reference counted object, therefore disconnect everything and they should be released.
// */
// PluginLinkedList::~PluginLinkedList() {
//     for (Node::Ptr cur = head, temp_next = nullptr; cur != tail; cur = temp_next) {
//         temp_next = cur->next;
//         cur->next = nullptr;
//         cur->prev = nullptr;
//         cur.reset();
//     }
//     head = nullptr;
//     tail = nullptr;
//     // all allocated object instances has no pointer pointing to them, so they will be released.
// }
// 
// inline void PluginLinkedList::__connect(Node::Ptr a, Node::Ptr b) {
//     a->next = b;
//     b->prev = a;
// }
// 
// /*
// Assume originally we have
// a -> b
// Then we are going to insert c between a and b. That is to say
// a -> c -> b
// */
// void PluginLinkedList::__insert(Node::Ptr a, Node::Ptr b, Node::Ptr c) {
//     __connect(a, c);
//     __connect(c, b);
//     ++__len;
// }
// 
// void PluginLinkedList::append(Node::Ptr node) {
//     __insert(tail->prev, tail, node);
// }
// 
// void PluginLinkedList::appendLeft(Node::Ptr node) {
//     __insert(head, head->next, node);
// }
// 
// int PluginLinkedList::indexWrapper(int index) {
//     return (index < 0) ? __len - (-index % __len) : index % __len;
// }
// 
// PluginLinkedList::Node::Ptr PluginLinkedList::get(int index) {
//     int wrappedIndex = indexWrapper(index);
//     Node::Ptr cur = nullptr;
// 
//     // backward or forward, depending on location.
//     if (wrappedIndex < (__len << 1)) {
//         for (cur = head->next; wrappedIndex--;) {
//             cur = cur->next;
//         }
//     }
//     else {
//         for (cur = tail->prev, wrappedIndex = __len - wrappedIndex - 1; wrappedIndex--;) {
//             cur = cur->prev;
//         }
//     }
//     return cur;
// }
// 
// void PluginLinkedList::insert(Node::Ptr node, int index) {
//     auto cur = get(index);
//     __insert(cur->prev, cur, node);
// }
// 
// PluginLinkedList::Node::Ptr PluginLinkedList::remove(int index) {
//     auto cur = get(index);
//     __connect(cur->prev, cur->next);
//     return cur;
// }
// 
// juce::AudioProcessor* JUCE_CALLTYPE PluginLinkedList::createPluginFilter() {
//     return new PluginLinkedList();
// }
// 
// double PluginLinkedList::getTailLengthSeconds() const
// {
//     return 0.0;
// }
// 
// bool PluginLinkedList::acceptsMidi() const
// {
//     return false;
// }
// 
// bool PluginLinkedList::producesMidi() const
// {
//     return false;
// }
// 
// int PluginLinkedList::getNumPrograms()
// {
//     return 0;
// }
// 
// int PluginLinkedList::getCurrentProgram()
// {
//     return 0;
// }
// 
// void PluginLinkedList::setCurrentProgram(int index)
// {
// }
// 
// const juce::String PluginLinkedList::getProgramName(int index)
// {
//     return juce::String();
// }
// 
// void PluginLinkedList::changeProgramName(int index, const juce::String& newName)
// {
// }
// 
// void PluginLinkedList::getStateInformation(juce::MemoryBlock& destData)
// {
// }
// 
// void PluginLinkedList::setStateInformation(const void* data, int sizeInBytes)
// {
// }
// 
// const juce::String PluginLinkedList::getName() const
// {
//     return juce::String();
// }
// 
// void PluginLinkedList::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
// {
//     Node::Ptr cur = head->next;
//     while (cur != tail) {
//         if (cur->processor != nullptr) {
//             cur->processor->prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);
//         }
//         cur = cur->next;
//     }
// }
// 
// void PluginLinkedList::releaseResources()
// {
//     Node::Ptr cur = head->next;
//     while (cur != tail) {
//         if (cur->processor != nullptr) {
//             cur->processor->releaseResources();
//         }
//         cur = cur->next;
//     }
// }
// 
// void PluginLinkedList::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
// {
//     int num_channel = buffer.getNumChannels();
// 
//     for (int i = 0; i < num_channel; ++i) {
//         buffer.applyGain(0.1);
//     }
// 
//     Node::Ptr cur = head->next;
//     while (cur != tail) {
//         if (cur->processor != nullptr) {
//             cur->processor->processBlock(buffer, midiMessages);
//         }
//         cur = cur->next;
//     }
// }
// 
// juce::AudioProcessorEditor* PluginLinkedList::createEditor()
// {
//     return nullptr;
// }
// 
// bool PluginLinkedList::hasEditor() const
// {
//     return false;
// }
