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
#include "PluginWrapperComponent.h"

class BandComponent : public juce::Component {
public:
    BandComponent();
    ~BandComponent();

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::OwnedArray<PluginWrapperComponent> pluginWrapperComponents;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BandComponent)
};

#endif // __hdr_BandComponent_h__
