/*
  ==============================================================================

    PluginWrapperComponent.h
    Created: 23 Mar 2022 4:26:50am
    Author:  medioq

  ==============================================================================
*/

#ifndef __hdr_PluginComponent_h__
#define __hdr_PluginComponent_h__

#include <JuceHeader.h>
#include "PluginWindow.h"

class BandComponent;

class PluginComponent : public juce::Component {
public:
    using NodePtr = juce::AudioProcessorGraph::Node::Ptr;
    PluginComponent(NodePtr nodePtr, BandComponent* owner);
    ~PluginComponent();

    void paint(juce::Graphics& g) override;
    void resized() override;
private:
    BandComponent* parentBandComponent;

    const float unitUtilRegionWidthRatio = 0.1;
    std::unique_ptr<juce::Button> openEditorButton;
    std::unique_ptr<juce::Button> deleteButton;

    NodePtr pluginNodePtr;
    std::unique_ptr<juce::PluginWindow> pluginWindow;
};

#endif // __hdr_PluginComponent_h__
