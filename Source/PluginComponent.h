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

class PluginComponent : public juce::Component {
public:
    PluginComponent(const juce::String& name);
    ~PluginComponent();

    void paint(juce::Graphics& g) override;
    void resized() override;
private:
    const float utilRegionWidthRatio = 0.2;
    std::unique_ptr<juce::Button> openEditorButton;
    juce::String pluginName;
};

#endif // __hdr_PluginComponent_h__
