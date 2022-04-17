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
            bool anyMoreFile = scanner.skipNextFile();
            if (!anyMoreFile) {
                break;
            }
        }
        if (nameOfNextPluginToBeScanned.contains(juce::String("Mono"))) {
            scanner.scanNextFile(true, nameOfNextPluginToBeScanned);
            break;
        }
        else {
            bool anyMoreFile = scanner.skipNextFile();
            if (!anyMoreFile) {
                break;
            }
        }
    }
    
    juce::String errorString;
    auto scannedPluginList = pluginLists->getTypes();
    auto monoPluginDescription = scannedPluginList[0];
    auto pluginInstance = audioPluginFormatManager->createPluginInstance(monoPluginDescription, getSampleRate(), getBlockSize(), errorString);
    pluginLinkedList.append(std::move(pluginInstance));
}

RecursionTestAudioProcessor::~RecursionTestAudioProcessor()
{
    delete audioPluginFormatManager;
    delete pluginLists;
    delete pluginFormatToScan;
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
    return juce::String();
}

void RecursionTestAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void RecursionTestAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    pluginLinkedList.prepareToPlay(sampleRate, samplesPerBlock);
}

void RecursionTestAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.

    pluginLinkedList.releaseResources();
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
    // clean junk channels
    for (auto i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i) {
        buffer.clear(i, 0, buffer.getNumSamples());
    }
    pluginLinkedList.processBlock(buffer, midiMessages);
}

//==============================================================================
bool RecursionTestAudioProcessor::hasEditor() const
{
    return false; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* RecursionTestAudioProcessor::createEditor()
{
    // return new RecursionTestAudioProcessorEditor (*this);
    return nullptr;
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

void RecursionTestAudioProcessor::reset() {
}

//==============================================================================
juce::AudioProcessorEditor* RecursionTestAudioProcessor::createEditorAtIndex(int index) {
    return nullptr;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RecursionTestAudioProcessor();
}
