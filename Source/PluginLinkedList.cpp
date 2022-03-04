/*
  ==============================================================================

    PluginLinkedList.cpp
    Created: 25 Feb 2022 11:57:10pm
        
    Author:  medioq

  ==============================================================================
*/

#include "PluginLinkedList.h"

PluginLinkedList::PluginLinkedList() {
    head = new Node();
    tail = new Node();
    head->next = tail;
    tail->prev = head;
}

/*
Since Node is reference counted object, therefore disconnect everything and they should be released.
*/
PluginLinkedList::~PluginLinkedList() {
    for (Node::Ptr cur = head, temp_next = nullptr; cur != tail; cur = temp_next) {
        temp_next = cur->next;
        cur->next = nullptr;
        cur->prev = nullptr;
    }
    head = nullptr;
    tail = nullptr;
    // all allocated object instances has no pointer pointing to them, so they will be released.
}

inline void PluginLinkedList::__connect(Node::Ptr a, Node::Ptr b) {
    a->next = b;
    b->prev = a;
}

/*
Assume originally we have
a -> b
Then we are going to insert c between a and b. That is to say
a -> c -> b
*/
void PluginLinkedList::__insert(Node::Ptr a, Node::Ptr b, Node::Ptr c) {
    __connect(a, c);
    __connect(c, b);
    ++__len;
}

void PluginLinkedList::append(Node::Ptr node) {
    __insert(tail->prev, tail, node);
}

void PluginLinkedList::appendLeft(Node::Ptr node) {
    __insert(head, head->next, node);
}

int PluginLinkedList::indexWrapper(int index) {
    return (index < 0) ? __len - (-index % __len) : index % __len;
}

PluginLinkedList::Node::Ptr PluginLinkedList::get(int index) {
    int wrappedIndex = indexWrapper(index);
    Node::Ptr cur = nullptr;

    // backward or forward, depending on location.
    if (wrappedIndex < (__len << 1)) {
        for (cur = head->next; wrappedIndex--;) {
            cur = cur->next;
        }
    }
    else {
        for (cur = tail->prev, wrappedIndex = __len - wrappedIndex - 1; wrappedIndex--;) {
            cur = cur->prev;
        }
    }
    return cur;
}

void PluginLinkedList::insert(Node::Ptr node, int index) {
    auto cur = get(index);
    __insert(cur->prev, cur, node);
}

PluginLinkedList::Node::Ptr PluginLinkedList::remove(int index) {
    auto cur = get(index);
    __connect(cur->prev, cur->next);
    return cur;
}

juce::AudioProcessor* JUCE_CALLTYPE PluginLinkedList::createPluginFilter() {
    return new PluginLinkedList();
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

const juce::String PluginLinkedList::getName() const
{
    return juce::String();
}

void PluginLinkedList::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
    Node::Ptr cur = head->next;
    while (cur != tail) {
        cur->processor->prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);
        cur = cur->next;
    }
}

void PluginLinkedList::releaseResources()
{
    Node::Ptr cur = head->next;
    while (cur != tail) {
        cur->processor->releaseResources();
        cur = cur->next;
    }
}

void PluginLinkedList::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    Node::Ptr cur = head->next;
    while (cur != tail) {
        cur->processor->processBlock(buffer, midiMessages);
        cur = cur->next;
    }
}

juce::AudioProcessorEditor* PluginLinkedList::createEditor()
{
    return nullptr;
}

bool PluginLinkedList::hasEditor() const
{
    return false;
}
