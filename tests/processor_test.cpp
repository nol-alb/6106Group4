/**
 * @file processor_test.cpp
 * @copyright Copyright 2020 tobanteAudio.
 */

#include "catch2/catch.hpp"

#include "pluginProcessor.h"

//JUCE processor test
TEST_CASE("processor: Name", "[demo][processor]")
{
    auto processor = NewProjectAudioProcessor {};
    REQUIRE(processor.getName() == juce::String {"Juce6 Demo"});
}

TEST_CASE("processor: HasEditor", "[demo][processor]")
{
    auto processor = NewProjectAudioProcessor {};
    REQUIRE(processor.hasEditor() == true);
}

TEST_CASE("processor: CurrentProgram", "[demo][processor]")
{
    auto processor = NewProjectAudioProcessor {};
    REQUIRE(processor.getCurrentProgram() == 0);
    REQUIRE(processor.getProgramName(1) == juce::String {});
}

TEST_CASE("processor: ChangeProgram", "[demo][processor]")
{
    auto processor = NewProjectAudioProcessor {};
    REQUIRE(processor.getNumPrograms() == 1);
    REQUIRE(processor.getCurrentProgram() == 0);

    processor.changeProgramName(1, "");
    processor.setCurrentProgram(2);

    // nothing should have changed. programs are not used.
    REQUIRE(processor.getNumPrograms() == 1);
    REQUIRE(processor.getCurrentProgram() == 0);
}

TEST_CASE("processor: BusesLayoutSupportMono", "[demo][processor]")
{
    auto processor     = NewProjectAudioProcessor {};
    auto layout        = juce::AudioProcessor::BusesLayout {};
    layout.inputBuses  = juce::AudioChannelSet::mono();
    layout.outputBuses = juce::AudioChannelSet::mono();

    REQUIRE(processor.isBusesLayoutSupported(layout) == true);
}

TEST_CASE("processor: BusesLayoutSupportStereo", "[demo][processor]")
{
    auto processor     = NewProjectAudioProcessor {};
    auto layout        = juce::AudioProcessor::BusesLayout {};
    layout.inputBuses  = juce::AudioChannelSet::stereo();
    layout.outputBuses = juce::AudioChannelSet::stereo();

    REQUIRE(processor.isBusesLayoutSupported(layout) == true);
}

TEST_CASE("processor: ZeroInput", "[demo][processor]")
{
    constexpr auto numChannels = 1;
    constexpr auto numSamples  = 128;

    auto midi   = juce::MidiBuffer {};
    auto buffer = juce::AudioBuffer<float> {numChannels, numSamples};

    // fill buffer with all 1.0
    for (auto i = 0; i < numChannels; i++)
    {
        for (auto j = 0; j < numSamples; j++) { buffer.setSample(i, j, 0.0f); }
    }

    auto processor = NewProjectAudioProcessor {};
    //processor.GetAPVTS().getRawParameterValue("gain")->store(0.0f);
    processor.prepareToPlay(44'100.0, numSamples);
    processor.processBlock(buffer, midi);

    // buffer should be silent
    for (auto i = 0; i < numChannels; i++)
    {
        for (auto j = 0; j < numSamples; j++)
        {
            auto const sample = buffer.getSample(i, j);
            REQUIRE(sample == 0.0f);
        }
    }

    processor.releaseResources();
}

TEST_CASE("processor: DCGainCheck", "[demo][processor]")
{
    constexpr auto numChannels = 1;
    constexpr auto numSamples  = 128;

    auto midi   = juce::MidiBuffer {};
    auto buffer = juce::AudioBuffer<float> {numChannels, numSamples};
    //int count = 0;

    // fill buffer with all 1.0
    for (auto i = 0; i < numChannels; i++)
    {
        for (auto j = 0; j < numSamples; j++) { buffer.setSample(i, j, 1.0f); }
    }

    auto processor = NewProjectAudioProcessor {};
    //processor.GetAPVTS().getRawParameterValue("gain")->store(0.0f);
    processor.prepareToPlay(44'100.0, numSamples);
    
    processor.processBlock(buffer, midi);
    

    // buffer should be silent
    for (auto i = 0; i < numChannels; i++)
    {
        for (auto j = 0; j < numSamples; j++)
        {
            auto const sample = buffer.getSample(i, j);
            REQUIRE(sample <= 1.06f);
            //REQUIRE(sample >= 0.1f);
            //count++;
        }
    }

    processor.releaseResources();
}

TEST_CASE("processor: Bypass", "[demo][processor]")
{
    constexpr auto numChannels = 1;
    constexpr auto numSamples  = 128;

    auto midi   = juce::MidiBuffer {};
    auto buffer = juce::AudioBuffer<float> {numChannels, numSamples};
    //int count = 0;

    // fill buffer with all 1.0
    for (auto i = 0; i < numChannels; i++)
    {
        for (auto j = 0; j < numSamples; j++) { buffer.setSample(i, j, 1.0f); }
    }

    auto processor = NewProjectAudioProcessor {};
    //processor.GetAPVTS().getRawParameterValue("gain")->store(0.0f);
    processor.bypass = 1;
    processor.prepareToPlay(44'100.0, numSamples);
    
    processor.processBlock(buffer, midi);
    

    // buffer should be silent
    for (auto i = 0; i < numChannels; i++)
    {
        for (auto j = 0; j < numSamples; j++)
        {
            auto const sample = buffer.getSample(i, j);
            REQUIRE(sample == 1.0f);
            //REQUIRE(sample >= 0.1f);
            //count++;
        }
    }

    processor.releaseResources();
}

/*
//createParameterlayout
TEST_CASE("processor: Layout", "[demo][processor]")
{
    constexpr auto numChannels = 1;
    constexpr auto numSamples  = 128;


    auto processor = NewProjectAudioProcessor {};
    //processor.GetAPVTS().getRawParameterValue("gain")->store(0.0f);
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    layout = processor.isBusesLayoutSupported();
    bool lay = processor.isBusesLayoutSupported(layout);

    REQUIRE(lay == true);

    processor.releaseResources();
}


TEST_CASE("processor: Compressor", "[demo][processor]")
{
    constexpr auto numChannels = 1;
    constexpr auto numSamples  = 128;

    auto midi   = juce::MidiBuffer {};
    auto buffer = juce::AudioBuffer<float> {numChannels, numSamples};
    //int count = 0;

    // fill buffer with all 1.0
    for (auto i = 0; i < numChannels; i++)
    {
        for (auto j = 0; j < numSamples; j++) { buffer.setSample(i, j, 1.0f); }
    }

    auto processor = NewProjectAudioProcessor {};
    //processor.GetAPVTS().getRawParameterValue("gain")->store(0.0f);
    processor.prepareToPlay(44'100.0, numSamples);
    
    processor.processBlock(buffer, midi);
    

    REQUIRE(processor.compressors.attack->get()==50);
    
    processor.releaseResources();
}
*/

/*
TEST_CASE("processor: ValueTreeDefaultState", "[demo][processor]")
{
    auto processor = NewProjectAudioProcessor {};
    auto& compressor      = processor.compressors();
    REQUIRE(&compressor.processor == &processor);
    REQUIRE(compressor.getRawParameterValue("attack")->load() == 50);
    //REQUIRE(vts.getRawParameterValue("width")->load() == 1.0f);
    //REQUIRE(vts.getRawParameterValue("room_size")->load() == 0.5f);
    //REQUIRE(vts.getRawParameterValue("damping")->load() == 0.5f);
}
*/
