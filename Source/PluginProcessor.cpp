/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginInclude.h"

//==============================================================================
RecursionTestAudioProcessor::RecursionTestAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    // graph = new juce::AudioProcessorGraph();
    audioPluginFormatManager = new juce::AudioPluginFormatManager();

    // plugin format manager initialization
    audioPluginFormatManager->addDefaultFormats(); /* call this method is ESSENTIAL!!!
                                                   otherwise it would report "No compatible plug-in 
                                                   format exists for this plug-in" when trying to 
                                                   `createPluginInstance` below */

    // plugin list initialization
    pluginLists = new juce::KnownPluginList();

    // prepare for scan

    juce::FileSearchPath pluginSearchPath("C:/Program Files/Common Files/VST3");
    juce::PluginDirectoryScanner scanner(*pluginLists, *pluginFormatToScan, pluginSearchPath, false, deadVSTFiles, false);

    DBG("Trying to scan plugins");

    while (true) {
        juce::String nameOfNextPluginToBeScanned = scanner.getNextPluginFileThatWillBeScanned();

        if (nameOfNextPluginToBeScanned.contains(JucePlugin_Name)) {
            scanner.skipNextFile();
        }
        try {
            bool anyMoreFile = scanner.scanNextFile(true, nameOfNextPluginToBeScanned);
            // anyMoreFile = false;
            if (!anyMoreFile) {
                break;
            }
        }
        catch (std::exception ce) {
        }

    };

    // fileLogger.logMessage("Plugin scan completed. Here's list of plugin that is available to use");
    // int cnt = 0;
    // for (auto pluginDescription : pluginLists->getTypes()) {
    //     DBG("Plugin " << cnt++ << ": " << pluginDescription.name);
    // }

    // initialize the plugin linked lists. Currently we use only one linked list as there's no frequency splitting utilities (at least in this branch)
    pluginLinkedLists.add(std::make_unique<PluginLinkedList>());
    for (int i = 0; i < numPluginMenu; ++i) {
        PluginLinkedList::Node::Ptr new_node = new PluginLinkedList::Node(nullptr);
        pluginLinkedLists[0]->append(new_node);
    }

    // the following code is just for test. Please don't use them for other purposes.
    // they are not carefully written to be sustainable, and should soly serve for test.
    /*
    juce::AudioProcessorGraph::Node::Ptr pluginNode;
    juce::String errorString;
    auto scannedPluginList = pluginLists->getTypes();
    for (auto pluginDescription : scannedPluginList) {
        DBG("Loading Plugin " << pluginDescription.name << ", with fs=" << getSampleRate() << ", block size=" << getBlockSize());
        errorString.clear();
    
        auto pluginInstance = audioPluginFormatManager->createPluginInstance(pluginDescription, getSampleRate(), getBlockSize(), errorString);
        if (pluginInstance != nullptr) {
            pluginNode = graph->addNode(std::move(pluginInstance));
            DBG("Plugin " << pluginDescription.name << " Loaded");
            break;
        }
        else {
            DBG("Cannot load plugin" << pluginDescription.name << ", error message: " << errorString);
        }
    }
    
    for (int channel = 0; channel < 2; ++channel) {
        graph->addConnection({ {inputNode->nodeID, channel}, {pluginNode->nodeID, channel} });
        graph->addConnection({ {pluginNode->nodeID, channel}, {outputNode->nodeID, channel} });
    }
    */
}

RecursionTestAudioProcessor::~RecursionTestAudioProcessor()
{
    delete audioPluginFormatManager;
    delete pluginLists;
    delete pluginFormatToScan;
    pluginLinkedLists.clear();
}

//==============================================================================
const juce::String RecursionTestAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool RecursionTestAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool RecursionTestAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool RecursionTestAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double RecursionTestAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int RecursionTestAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int RecursionTestAudioProcessor::getCurrentProgram()
{
    return 0;
}

void RecursionTestAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String RecursionTestAudioProcessor::getProgramName (int index)
{
    return {};
}

void RecursionTestAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void RecursionTestAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    for (auto pluginLinkedList : pluginLinkedLists) {
        pluginLinkedList->prepareToPlay(sampleRate, samplesPerBlock);
    }
}

void RecursionTestAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    for (auto pluginLinkedList : pluginLinkedLists) {
        pluginLinkedList->releaseResources();
    }
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool RecursionTestAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void RecursionTestAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // juce::ScopedNoDenormals noDenormals;
    // auto totalNumInputChannels  = getTotalNumInputChannels();
    // auto totalNumOutputChannels = getTotalNumOutputChannels();

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    // for (int channel = 0; channel < totalNumInputChannels; ++channel)
    // {
    //     auto* channelData = buffer.getWritePointer (channel);

    //     // ..do something to the data...
    // }

    for (auto pluginLinkedList : pluginLinkedLists) {
        pluginLinkedList->processBlock(buffer, midiMessages);
    }
}

//==============================================================================
bool RecursionTestAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* RecursionTestAudioProcessor::createEditor()
{
    return new RecursionTestAudioProcessorEditor (*this);
}

//==============================================================================
void RecursionTestAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void RecursionTestAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
void RecursionTestAudioProcessor::setPluginAtIndex(int index, juce::PluginDescription& pluginDescription) {

    // this function will be called when user choose from text button's popup menu.
    // we need to get lock to update the internal graph.
    juce::ScopedLock lock(getCallbackLock());

    juce::AudioProcessorGraph::Node::Ptr pluginNode;
    juce::String errorString;
    auto scannedPluginList = pluginLists->getTypes();
    auto pluginInstance = audioPluginFormatManager->createPluginInstance(pluginDescription, getSampleRate(), getBlockSize(), errorString);
    if (pluginInstance != nullptr) {
        auto nodePtr = pluginLinkedLists[0]->get(index);
        nodePtr->set(std::move(pluginInstance));
        DBG("Plugin " << pluginDescription.name << " Loaded");
    }
    else {
        DBG("Cannot load plugin" << pluginDescription.name << ", error message: " << errorString);
    }
}

juce::AudioProcessorEditor* RecursionTestAudioProcessor::createEditorAtIndex(int index) {
    auto nodePtr = pluginLinkedLists[0]->get(index);
    if (nodePtr != nullptr && nodePtr->processor != nullptr) {
        return nodePtr->processor->createEditorIfNeeded();
    }
    return nullptr;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RecursionTestAudioProcessor();
}
