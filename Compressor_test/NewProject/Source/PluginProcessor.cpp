/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
NewProjectAudioProcessor::NewProjectAudioProcessor()
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
    attack = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Attack"));
    jassert(attack != nullptr);
    
    release = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Release"));
    jassert(release != nullptr);
    
    threshold = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Threshold"));
    jassert(threshold != nullptr);
    ratio = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("Ratio"));
    jassert(ratio != nullptr);
    bypassed = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("Bypassed"));
    jassert(bypassed != nullptr);
    
    
    
}

NewProjectAudioProcessor::~NewProjectAudioProcessor()
{
}

//==============================================================================
const juce::String NewProjectAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool NewProjectAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool NewProjectAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool NewProjectAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double NewProjectAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int NewProjectAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int NewProjectAudioProcessor::getCurrentProgram()
{
    return 0;
}

void NewProjectAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String NewProjectAudioProcessor::getProgramName (int index)
{
    return {};
}

void NewProjectAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void NewProjectAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    spec.sampleRate = sampleRate;
    
    compressor.prepare(spec);
    
    
}

void NewProjectAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool NewProjectAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void NewProjectAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    
    compressor.setAttack(attack->get());
    compressor.setRelease(release->get());
    compressor.setThreshold(threshold->get());
    compressor.setRatio( ratio->getCurrentChoiceName().getFloatValue() );
    
    auto block  = juce::dsp::AudioBlock<float>(buffer);
    auto context = juce::dsp::ProcessContextReplacing<float>(block);
    
    context.isBypassed = bypassed->get();
    
    compressor.process(context);
    
    
}

//==============================================================================
bool NewProjectAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

// Create a generic audioprocessorEditor to reflect all the sliders and changes

juce::AudioProcessorEditor* NewProjectAudioProcessor::createEditor()
{
//    return new NewProjectAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
    
}

//==============================================================================
void NewProjectAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
    
}

void NewProjectAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if( tree.isValid() )
    {
        apvts.replaceState(tree);
    }
}

//Adding the necessary AudioParameters from the audioProcessorValueTreeStaye, with the method we defined called create ParameterLayout() = laypit.add basically adds to the value tree state

juce::AudioProcessorValueTreeState::ParameterLayout NewProjectAudioProcessor::createParameterlayout()
{
    APVTS::ParameterLayout layout;
    
    using namespace juce;
    layout.add(std::make_unique<AudioParameterFloat>("Threshold", "Threshold", NormalisableRange<float>(-60, 12, 1, 1),0));
    
    auto attackReleaseRange = NormalisableRange<float>(5, 500, 1, 1);
    layout.add(std::make_unique<AudioParameterFloat>("Attack", "Attack",attackReleaseRange,50));
    layout.add(std::make_unique<AudioParameterFloat>("Release", "Release",attackReleaseRange,50));
    
    layout.add(std::make_unique<AudioParameterBool>("Bypassed", "Bypassed", false));
    
    
    auto choices = std::vector<double>{1, 1.5, 2, 3, 4, 5, 6, 7, 8, 10, 15, 20, 50, 100 };
    juce::StringArray sa;
    for ( auto choice : choices )
    {
        sa.add( juce::String(choice, 1));
    }
    
    layout.add(std::make_unique<AudioParameterChoice>("Ratio", "Ratio", sa,3));
    
    
    
    return layout;
    
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NewProjectAudioProcessor();
}
