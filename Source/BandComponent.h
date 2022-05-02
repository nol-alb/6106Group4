/*
  ==============================================================================

    BandWrapper.h
    Created: 21 Apr 2022 3:31:37pm
    Author:  medioqrity

  ==============================================================================
*/

#ifndef __hdr_BandComponent_h__
#define __hdr_BandComponent_h__

#include <JuceHeader.h>
#include "PluginComponent.h"
#include "PluginProcessor.h"

class BandComponent : public juce::Component {
public:
    BandComponent(PluginLinkedList*);
    ~BandComponent();

    void paint(juce::Graphics& g) override;
    void resized() override;

    void removePluginComponent(const PluginComponent* pluginComponent);

private:
    //==============================================================================
    juce::OwnedArray<PluginComponent> pluginComponents;
    PluginLinkedList* pluginLinkedList;
    juce::Component pluginComponentContainer;
    juce::Viewport pluginComponentViewport;

    //==============================================================================
    juce::TextButton addPluginButton{"Append Plugin"};
    juce::PopupMenu pluginMenu;

    //==============================================================================
    RecursionTestAudioProcessor audioProcessor;

    //==============================================================================
    const float pluginHeightRatio = 1.f / 10.f;
    const float buttonHeightRatio = 1.f / 10.f;

    //==============================================================================
    void __updatePluginWrapperComponents();
    void __initializeAddPluginButton();
    void __refresh();
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BandComponent)
};

#endif // __hdr_BandComponent_h__
