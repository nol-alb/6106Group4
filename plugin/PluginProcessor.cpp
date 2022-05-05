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
    using namespace Params;
    const auto& params = GetParams();
    
    //Lambda function to help in code you don't have to keep retyping
    /*
     auto y = [] (auto first, auto second)
     {
         return first + second;
     };
     **/
    auto floatHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName)
    {
        param = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(paramName)));
        jassert(param != nullptr);
        
        
    };
    
    auto choiceHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName)
    {
        param = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(params.at(paramName)));
        jassert(param != nullptr);
        
    };
    auto boolHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName)
    {
        param = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(paramName)));
        jassert(param != nullptr);
        
    };
    
    floatHelper(lowBandComp.attack,     Names::Attack_Low_Band);
    floatHelper(lowBandComp.release,    Names::Release_Low_Band);
    floatHelper(lowBandComp.threshold, Names::Threshold_Low_Band);
    floatHelper(midBandComp.attack,     Names::Attack_Mid_Band);
    floatHelper(midBandComp.release,    Names::Release_Mid_Band);
    floatHelper(midBandComp.threshold,  Names::Threshold_Mid_Band);
    floatHelper(highBandComp.attack,    Names::Attack_High_Band);
    floatHelper(highBandComp.release,   Names::Release_High_Band);
    floatHelper(highBandComp.threshold,Names::Threshold_High_Band);
    
    
    
    choiceHelper(lowBandComp.ratio, Names::Ratio_Low_Band);
    choiceHelper(midBandComp.ratio, Names::Ratio_Mid_Band);
    choiceHelper(highBandComp.ratio, Names::Ratio_High_Band);
    
    boolHelper(lowBandComp.bypassed, Names::Bypassed_Low_Band);
    boolHelper(midBandComp.bypassed, Names::Bypassed_Mid_Band);
    boolHelper(highBandComp.bypassed, Names::Bypassed_High_Band);
    
    floatHelper(lowMidCrossover,Names::Low_Mid_Crossover_Freq);
    floatHelper(midHighCrossover, Names::Mid_High_Crossover_Freq);
    
    
    LP1.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    HP1.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    
    AP2.setType(juce::dsp::LinkwitzRileyFilterType::allpass);
    
    LP2.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    HP2.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
//    invAP1.setType(juce::dsp::LinkwitzRileyFilterType::allpass);
//    invAP2.setType(juce::dsp::LinkwitzRileyFilterType::allpass);

    
    
    
    
    
    
//    compressor.attack = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Attack"));
//    jassert(compressor.attack != nullptr);
//
//    compressor.release = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Release"));
//    jassert(compressor.release != nullptr);
//
//    compressor.threshold = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Threshold"));
//    jassert(compressor.threshold != nullptr);
//    compressor.ratio = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("Ratio"));
//    jassert(compressor.ratio != nullptr);
//    compressor.bypassed = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("Bypassed"));
//    jassert(compressor.bypassed != nullptr);
    
    
    
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
    
    //Make sure you prepare each of the dsp functionality with the spec!
    for (auto& comp: compressors)
        comp.prepare(spec);
    
//    compressor.prepare(spec);
    HP1.prepare(spec);
    LP1.prepare(spec);
    
    AP2.prepare(spec);
    
    LP2.prepare(spec);
    HP2.prepare(spec);
    
//    invAP1.prepare(spec);
//    invAP2.prepare(spec);
//
//    invAPBuffer.setSize(spec.numChannels, samplesPerBlock);
//

    
    
    for( auto& buffer: filterBuffers )
    {
        buffer.setSize(spec.numChannels, samplesPerBlock);
        
    }
    
    
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
    
    
    
    //Compressor Functionality
    
    for (auto& compressor: compressors)
        compressor.updateCompressorSettings();
    
    
//
//    compressor.process(buffer);
    
    //Filter Functionality
    
    for ( auto& fb : filterBuffers )
    {
        fb = buffer;
        
    }
//    invAPBuffer = buffer;
    
    
    auto lowMidCutoffFreq = lowMidCrossover->get();
    LP1.setCutoffFrequency(lowMidCutoffFreq);
    HP1.setCutoffFrequency(lowMidCutoffFreq);
    
//    invAP1.setCutoffFrequency(lowMidcutoffFreq);
    
    auto midHighCutoffFreq = midHighCrossover->get();
    AP2.setCutoffFrequency(midHighCutoffFreq);
    LP2.setCutoffFrequency(midHighCutoffFreq);
    HP2.setCutoffFrequency(midHighCutoffFreq);
//    invAP2.setCutoffFrequency(midHighCutoffFreq);
    
    
    
    auto fb0Block = juce::dsp::AudioBlock<float>(filterBuffers[0]);
    auto fb1Block = juce::dsp::AudioBlock<float>(filterBuffers[1]);
    auto fb2Block = juce::dsp::AudioBlock<float>(filterBuffers[2]);
    

    /*
     Contains context information that is passed into an algorithm's process method.

     This context is intended for use in situations where a single block is being used for both the input and output, so it will return the same object for both its getInputBlock() and getOutputBlock() methods.
     **/
    auto fb0Ctx = juce::dsp::ProcessContextReplacing<float>(fb0Block);
    auto fb1Ctx = juce::dsp::ProcessContextReplacing<float>(fb1Block);
    auto fb2Ctx = juce::dsp::ProcessContextReplacing<float>(fb2Block);
    
    //processing the audio
    if(bypass == 0){
        LP1.process(fb0Ctx);
        AP2.process(fb0Ctx);
        
        HP1.process(fb1Ctx);
        filterBuffers[2]=filterBuffers[1];
        LP2.process(fb1Ctx);
        
        HP2.process(fb2Ctx);
        
        for (size_t i= 0; i <filterBuffers.size(); ++i )
        {
            compressors[i].process(filterBuffers[i]);
            
        }
    }
//
//    auto invAPBlock = juce::dsp::AudioBlock<float>(invAPBuffer);
//    auto invAPCtx = juce::dsp::ProcessContextReplacing<float>(invAPBlock);
//
//    invAP1.process(invAPCtx);
//    invAP2.process(invAPCtx);

    
    
    //Input buffer is split and processed seperately, now we need to add them up together to ease our processing
    
    auto numSamples = buffer.getNumSamples();
    auto numChannels = buffer.getNumChannels();
    
    //Bypassing the filter functionality if each compressor is bypassed
//    if(compressor.bypassed->get() )
//        return;
//    apBuffer = buffer;
//
//    auto apBlock = juce::dsp::AudioBlock<float>(apBuffer);
//
//    auto apContext = juce::dsp::ProcessContextReplacing<float>(apBlock);
//    AP.process(apContext);
//
    
    //clear the final buffer before feeding the input to it
    if(bypass == 0)
        buffer.clear();
    
    //Another lambda function to add each channel of the processed buffers into the output buffer
    
    auto addFilterBand = [nc = numChannels, ns = numSamples](auto& inputBuffer, const auto& source)
    {
        for ( auto i= 0; i<nc; ++i)
        {
            inputBuffer.addFrom(i,0, source, i, 0, ns);
        }
    };
//    if( !compressor.bypassed->get() )
//    {
//        addFilterBand(buffer, filterBuffers[0]);
//        addFilterBand(buffer, filterBuffers[1]);
//    }
//    else
//    {
//        addFilterBand(buffer, apBuffer);
//    }
//
    //ADD TESTS
    //if( !compressor.bypassed->get() )
    if(bypass == 0){
        addFilterBand(buffer, filterBuffers[0]);
        addFilterBand(buffer, filterBuffers[1]);
        addFilterBand(buffer, filterBuffers[2]);
    }
    //
    
    
    
    /*

    // do something with buffer

    juce::File file("test.wav");
    file.deleteFile();

    juce::WavAudioFormat format;
    std::unique_ptr<juce::AudioFormatWriter> writer;

    writer.reset(format.createWriterFor(new juce::FileOutputStream(file),
      44100,
      buffer.getNumChannels(),
      24,
      {},
      0));

    if (writer != nullptr)
      writer->writeFromAudioSampleBuffer(buffer, 0, buffer.getNumSamples());
    
    */
//    if (compressor.bypassed->get() )
//    {
//        for ( auto ch=0; ch< numChannels; ++ch)
//        {
//            juce::FloatVectorOperations::multiply(apBuffer.getWritePointer(ch), -1.f, numSamples);
//
//        }
//        addFilterBand(buffer, apBuffer);
//
//    }
//
    //Changing the code to work through a modularized compressor struct - 23/2/22
    
//    compressor.setAttack(attack->get());
//    compressor.setRelease(release->get());
//    compressor.setThreshold(threshold->get());
//    compressor.setRatio( ratio->getCurrentChoiceName().getFloatValue() );
//
//    auto block  = juce::dsp::AudioBlock<float>(buffer);
//    auto context = juce::dsp::ProcessContextReplacing<float>(block);
//
//    context.isBypassed = bypassed->get();
//
//    compressor.process(context);
    
    
    
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
    using namespace Params;
    const auto& params = GetParams();
    
    //std::string::at can be used to extract characters by characters from a given string.
    
    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Threshold_Low_Band),params.at(Names::Threshold_Low_Band), NormalisableRange<float>(-60, 12, 1, 1),0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Threshold_Mid_Band),params.at(Names::Threshold_Mid_Band), NormalisableRange<float>(-60, 12, 1, 1),0));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Threshold_High_Band),params.at(Names::Threshold_High_Band), NormalisableRange<float>(-60, 12, 1, 1),0));

    
    auto attackReleaseRange = NormalisableRange<float>(5, 500, 1, 1);
    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Attack_Low_Band),params.at(Names::Attack_Low_Band),attackReleaseRange,50));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Attack_Mid_Band),params.at(Names::Attack_Mid_Band),attackReleaseRange,50));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Attack_High_Band),params.at(Names::Attack_High_Band),attackReleaseRange,50));
    
    
    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Release_Low_Band),params.at(Names::Release_Low_Band),attackReleaseRange,50));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Release_Mid_Band),params.at(Names::Release_Mid_Band),attackReleaseRange,50));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Release_High_Band),params.at(Names::Release_High_Band),attackReleaseRange,50));
    layout.add(std::make_unique<AudioParameterBool>(params.at(Names::Bypassed_Low_Band),params.at(Names::Bypassed_Low_Band),false));
    layout.add(std::make_unique<AudioParameterBool>(params.at(Names::Bypassed_Mid_Band),params.at(Names::Bypassed_Mid_Band),false));
    layout.add(std::make_unique<AudioParameterBool>(params.at(Names::Bypassed_High_Band),params.at(Names::Bypassed_High_Band),false));
    
    auto choices = std::vector<double>{1, 1.5, 2, 3, 4, 5, 6, 7, 8, 10, 15, 20, 50, 100 };
    juce::StringArray sa;
    for ( auto choice : choices )
    {
        sa.add( juce::String(choice, 1));
    }
    
    layout.add(std::make_unique<AudioParameterChoice>(params.at(Names::Ratio_Low_Band),params.at(Names::Ratio_Low_Band), sa,3));
    layout.add(std::make_unique<AudioParameterChoice>(params.at(Names::Ratio_Mid_Band),params.at(Names::Ratio_Mid_Band), sa,3));
    layout.add(std::make_unique<AudioParameterChoice>(params.at(Names::Ratio_High_Band),params.at(Names::Ratio_High_Band), sa,3));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Low_Mid_Crossover_Freq),params.at(Names::Low_Mid_Crossover_Freq),NormalisableRange<float>(20,999, 1, 1),400));
    layout.add(std::make_unique<AudioParameterFloat>(params.at(Names::Mid_High_Crossover_Freq),params.at(Names::Mid_High_Crossover_Freq),NormalisableRange<float>(1000,20000, 1, 1),2000));
    
    
    
    
    return layout;
    
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NewProjectAudioProcessor();
}
