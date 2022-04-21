/*
  ==============================================================================

    PluginPluginEditor.cpp
    Created: 23 Mar 2022 4:26:50am
    Author:  medioq

  ==============================================================================
*/

#include "PluginWrapperComponent.h"

void PluginWrapperComponent::userTriedToCloseWindow() {
    if (isOnDesktop()) {
        setVisible(false);
    }
}
