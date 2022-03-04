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

    const int numPluginMenu = audioProcessor.numPluginMenu;

    // initialize a sequence of plugins
    /*
    pluginMenus = new juce::PopupMenu * [numPluginMenu];
    for (int i = 0; i < numPluginMenu; ++i) {
        pluginMenus[i] = new juce::PopupMenu();
        juce::KnownPluginList::addToMenu(
            *pluginMenus[i], 
            audioProcessor.pluginLists->getTypes(), 
            juce::KnownPluginList::SortMethod::sortAlphabetically
        );
    }*/

    // initialize text buttons
    // look at the lambda callback function in PluginListPopupMenu to see how the plugin is placed
    pluginListPopupMenus = new PluginListPopupMenu*[numPluginMenu];
    for (int i = 0; i < numPluginMenu; ++i) {
        pluginListPopupMenus[i] = new PluginListPopupMenu(i, audioProcessor);
        addAndMakeVisible(*pluginListPopupMenus[i]);
        audioProcessor.pluginLists->addChangeListener(static_cast<juce::ChangeListener*>(pluginListPopupMenus[i]));
    }

    // addAndMakeVisible(button);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 600);

}

RecursionTestAudioProcessorEditor::~RecursionTestAudioProcessorEditor()
{
    const int numPluginMenu = audioProcessor.numPluginMenu;
    for (int i = 0; i < numPluginMenu; ++i) {
        delete pluginListPopupMenus[i];
    }
    delete[] pluginListPopupMenus;
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

    const int numPluginMenu = audioProcessor.numPluginMenu;

    for (int i = 0; i < numPluginMenu; ++i) {
        pluginListPopupMenus[i]->setBounds(0, i * (getHeight() / numPluginMenu), getWidth(), getHeight() / numPluginMenu);
    }
}
