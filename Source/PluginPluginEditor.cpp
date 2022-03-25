/*
  ==============================================================================

    PluginPluginEditor.cpp
    Created: 23 Mar 2022 4:26:50am
    Author:  medioq

  ==============================================================================
*/

#include "PluginPluginEditor.h"

void VSTPluginEditorWrapper::userTriedToCloseWindow() {
    if (isOnDesktop()) {
        setVisible(false);
    }
}
