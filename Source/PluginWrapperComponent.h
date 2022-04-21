/*
  ==============================================================================

    PluginWrapperComponent.h
    Created: 23 Mar 2022 4:26:50am
    Author:  medioq

  ==============================================================================
*/

#ifndef __hdr_PluginWrapperComponent_h__
#define __hdr_PluginWrapperComponent_h__

#include <JuceHeader.h>

class PluginWrapperComponent : public juce::Component {
public:
    void userTriedToCloseWindow() override;
};

#endif // __hdr_PluginWrapperComponent_h__
