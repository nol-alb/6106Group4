/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginInclude.h"

//==============================================================================
RecursionTestAudioProcessorEditor::RecursionTestAudioProcessorEditor (RecursionTestAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    /*
    // THIS PART OF CODE SHOULD BE SPLTIED OUT
    juce::PropertiesFile::Options opt;
    opt.millisecondsBeforeSaving = 2000;
    opt.storageFormat = juce::PropertiesFile::storeAsXML;
    opt.applicationName = juce::String(ProjectInfo::projectName);
    opt.filenameSuffix = juce::String("xml");
    opt.folderName = juce::String(ProjectInfo::projectName);

    juce::PropertiesFile* propertiesFile = new juce::PropertiesFile(opt.getDefaultFile(), opt);

    // initialize pluginListComponent using audioProcessor
    pluginListComponent = new juce::PluginListComponent(
        *audioProcessor.audioPluginFormatManager, 
        *audioProcessor.pluginLists, 
        audioProcessor.deadVSTFiles, 
        propertiesFile
    );
    addAndMakeVisible(pluginListComponent);
    */

    // initialize band component for each band
    for (int i = 0; i < audioProcessor.numBand; ++i) {
        addAndMakeVisible(bands.add(new BandComponent()));
    }

    // initialize sliders for band split frequencies
    for (int i = 0; i < 2; ++i) {
        addAndMakeVisible(sliders.add(new juce::Slider()));
        sliders.getLast()->setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    }

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 600);
    setResizable(true, true);
}

RecursionTestAudioProcessorEditor::~RecursionTestAudioProcessorEditor() {
    bands.clear();
}

//==============================================================================
void RecursionTestAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void RecursionTestAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto bounds = getLocalBounds();
    auto w = bounds.getWidth();
    auto h = bounds.getHeight();
    float w_each_band = static_cast<float>(w) / audioProcessor.numBand;
    float w_each_slider = static_cast<float>(w) / 2;

    // use flex box to store bands & sliders
    juce::FlexBox splitFreqSliderFlexBox;
    juce::FlexBox bandsFlexBox;

    for (auto* bandComponent : bands) {
        juce::FlexItem currentBandComponentFlexItem(w_each_band, 0.8f * h, *bandComponent);
        bandsFlexBox.items.add(currentBandComponentFlexItem);
    }

    for (auto* slider : sliders) {
        slider->setSliderStyle (juce::Slider::SliderStyle::LinearHorizontal);
        juce::FlexItem currentSliderFlexItem(w_each_slider, 0.2f * h, *slider);
        splitFreqSliderFlexBox.items.add(currentSliderFlexItem);
    }

    splitFreqSliderFlexBox.performLayout(bounds.removeFromTop(0.2f * h));
    bandsFlexBox.performLayout(bounds);
}
