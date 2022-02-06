/*
  ==============================================================================

    PluginPluginManager.h
    Created: 4 Feb 2022 11:04:10pm
    Author:  medioqrity

  ==============================================================================
*/

#ifndef __hdr_PluginPluginManager_h__
#define __hdr_PluginPluginManager_h__

#include <JuceHeader.h>

/*
A helper class to hide messy external variables.
*/
class PluginManager {
public:
    PluginManager();

private:
    std::vector<juce::PluginDirectoryScanner*> scanners;
};

#endif // __hdr_PluginPluginManager_h__
