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

    // one vector for each popup menu
    for (int i = 0; i < audioProcessor.numBand; ++i) {
        pluginListPopupMenus.emplace_back(new std::list<PluginListPopupMenu*>);
    }

    // initialize text buttons
    // look at the lambda callback function in PluginListPopupMenu to see how the plugin is placed
    // new PluginListPopupMenu(i, audioProcessor)
    // addAndMakeVisible(*pluginListPopupMenus[i]);
    // audioProcessor.pluginLists->addChangeListener(static_cast<juce::ChangeListener*>(pluginListPopupMenus[i]));

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 600);

}

RecursionTestAudioProcessorEditor::~RecursionTestAudioProcessorEditor()
{
    for (auto popupMenuList : pluginListPopupMenus) {
        while (!popupMenuList->empty()) delete popupMenuList->front(), popupMenuList->pop_front();
    }
    while (!pluginListPopupMenus.empty()) delete pluginListPopupMenus.back(), pluginListPopupMenus.pop_back();
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

    // int popupMenuListIndex = 0;
    // for (auto popupMenuList : pluginListPopupMenus) {
    //     for (auto popupMenu : *popupMenuList) {
    //         popupMenu->setBounds();
    //     }
    //     ++popupMenuListIndex;
    // }
}
