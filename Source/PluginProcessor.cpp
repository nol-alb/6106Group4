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
    knownPluginList = new juce::KnownPluginList();

    // prepare for scan
    juce::FileSearchPath pluginSearchPath("C:/Program Files/Common Files/VST3");
    juce::PluginDirectoryScanner scanner(*knownPluginList, *pluginFormatToScan, pluginSearchPath, false, deadVSTFiles, false);

    DBG("Trying to scan plugins");

    // scan plugins
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

    using namespace Params;
    const auto& params = GetParams();
    auto floatHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName) {
        param = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(paramName)));
        jassert(param != nullptr);
    };
    auto boolHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName) {
        param = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(paramName)));
        jassert(param != nullptr);
    };    

    floatHelper(lowMidCrossover,Names::Low_Mid_Crossover_Freq);
    floatHelper(midHighCrossover, Names::Mid_High_Crossover_Freq);

    LP1.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    HP1.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    
    AP2.setType(juce::dsp::LinkwitzRileyFilterType::allpass);
    
    LP2.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    HP2.setType(juce::dsp::LinkwitzRileyFilterType::highpass);

    //==============================================================================
    // initialize graphs, each band has one corresponding graph.
    // also initialize plugin lists.
    for (int i = 0; i < numBand; ++i) {
        graphs.add(new AudioProcessorGraph());
        pluginLists.add(new juce::ReferenceCountedArray<Node>());
        audioInputNodes.add(nullptr);
        audioOutputNodes.add(nullptr);
    }
    
    //==============================================================================
    // insert plugins (temporary, should be replaced with UI)
    juce::String errorString;
    auto scannedPluginList = pluginLists->getTypes();
    auto monoPluginDescription = scannedPluginList[0];
    auto pluginInstance = audioPluginFormatManager->createPluginInstance(monoPluginDescription, getSampleRate(), getBlockSize(), errorString);
    pluginLinkedList.append(std::move(pluginInstance));
}

RecursionTestAudioProcessor::~RecursionTestAudioProcessor()
{
    delete audioPluginFormatManager;
    delete knownPluginList;
    delete pluginFormatToScan;
    for (auto& graph : graphs) { graph->clear(); }
    for (auto& pluginList : pluginLists) { pluginList->clear(); }
    graphs.clear();
    pluginLists.clear();
    audioInputNodes.clear();
    audioOutputNodes.clear();
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
    __prepareFilters(sampleRate, samplesPerBlock);

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

    //==============================================================================
    for ( auto& fb : filterBuffers ) {
        fb = buffer;
    }
    
    auto lowMidCutoffFreq = lowMidCrossover->get();
    LP1.setCutoffFrequency(lowMidCutoffFreq);
    HP1.setCutoffFrequency(lowMidCutoffFreq);
    
    auto midHighCutoffFreq = midHighCrossover->get();
    AP2.setCutoffFrequency(midHighCutoffFreq);
    LP2.setCutoffFrequency(midHighCutoffFreq);
    HP2.setCutoffFrequency(midHighCutoffFreq);
    
    auto fb0Block = juce::dsp::AudioBlock<float>(filterBuffers[0]);
    auto fb1Block = juce::dsp::AudioBlock<float>(filterBuffers[1]);
    auto fb2Block = juce::dsp::AudioBlock<float>(filterBuffers[2]);

    /*
     Contains context information that is passed into an algorithm's process method.

     This context is intended for use in situations where a single block is being used for both 
     the input and output, so it will return the same object for both its getInputBlock() and getOutputBlock() methods.
     **/
    auto fb0Ctx = juce::dsp::ProcessContextReplacing<float>(fb0Block);
    auto fb1Ctx = juce::dsp::ProcessContextReplacing<float>(fb1Block);
    auto fb2Ctx = juce::dsp::ProcessContextReplacing<float>(fb2Block);
    
    //processing the audio
    LP1.process(fb0Ctx);
    AP2.process(fb0Ctx);
    
    HP1.process(fb1Ctx);
    filterBuffers[2]=filterBuffers[1];
    LP2.process(fb1Ctx);
    
    HP2.process(fb2Ctx);

    //==============================================================================
    for (int i = 0; i < numBand; ++i) {
        pluginLinkedList.processBlock(buffer, midiMessages);
    }

    //==============================================================================
    auto numSamples = buffer.getNumSamples();
    auto numChannels = buffer.getNumChannels();
    
    //clear the final buffer before feeding the input to it
    buffer.clear();
    
    //Another lambda function to add each channel of the processed buffers into the output buffer
    auto addFilterBand = [nc = numChannels, ns = numSamples](auto& inputBuffer, const auto& source) {
        for (auto i = 0; i < nc; ++i) {
            inputBuffer.addFrom(i, 0, source, i, 0, ns);
        }
    };

    // sum buffer up
    addFilterBand(buffer, filterBuffers[0]);
    addFilterBand(buffer, filterBuffers[1]);
    addFilterBand(buffer, filterBuffers[2]);
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

juce::AudioProcessorValueTreeState::ParameterLayout RecursionTestAudioProcessor::createParameterlayout()
{
    APVTS::ParameterLayout layout;
    
    using namespace juce;
    using namespace Params;
    const auto& params = GetParams();

    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Low_Mid_Crossover_Freq),params.at(Names::Low_Mid_Crossover_Freq),NormalisableRange<float>(20,999, 1, 1), 400));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Mid_High_Crossover_Freq),params.at(Names::Mid_High_Crossover_Freq),NormalisableRange<float>(1000,20000, 1, 1), 2000));
    
    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RecursionTestAudioProcessor();
}

void RecursionTestAudioProcessor::__prepareFilters(double sampleRate, int samplesPerBlock) {
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    spec.sampleRate = sampleRate;
    
    // TODO change to filter array
    HP1.prepare(spec);
    LP1.prepare(spec);
    AP2.prepare(spec);
    LP2.prepare(spec);
    HP2.prepare(spec);
    
    for(auto& buffer: filterBuffers)
    {
        buffer.setSize(spec.numChannels, samplesPerBlock);
    }
}
