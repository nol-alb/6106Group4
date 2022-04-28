/*
  ==============================================================================

    PluginEditorComponent.h
    Created: 27 Apr 2022 8:15:53pm
    Author:  medioqrity

  ==============================================================================
*/

#ifndef __hdr_PluginWindow_h__
#define __hdr_PluginWindow_h__

#include <JuceHeader.h>

namespace juce {

inline String getFormatSuffix (const AudioProcessor* plugin) {
    const auto format = [plugin]() {
        if (auto* instance = dynamic_cast<const AudioPluginInstance*> (plugin))
            return instance->getPluginDescription().pluginFormatName;

        return String();
    }();

    return format.isNotEmpty() ? (" (" + format + ")") : format;
}

class PluginWindow : public DocumentWindow {
public:
    enum class Type {
        normal = 0,
        generic,
        numTypes
    };

    PluginWindow (AudioProcessorGraph::Node* n, Type t)
       : DocumentWindow (n->getProcessor()->getName() + getFormatSuffix (n->getProcessor()),
                         LookAndFeel::getDefaultLookAndFeel().findColour (ResizableWindow::backgroundColourId),
                         DocumentWindow::closeButton),
         node (n), type (t)
    {
        setSize (400, 300);

        if (auto* ui = createProcessorEditor (*node->getProcessor(), type)) {
            setContentOwned (ui, true);
            setResizable (ui->isResizable(), false);
        }

       #if JUCE_IOS || JUCE_ANDROID
        auto screenBounds = Desktop::getInstance().getDisplays().getTotalBounds (true).toFloat();
        auto scaleFactor = jmin ((screenBounds.getWidth() - 50) / getWidth(), (screenBounds.getHeight() - 50) / getHeight());

        if (scaleFactor < 1.0f)
            setSize ((int) (getWidth() * scaleFactor), (int) (getHeight() * scaleFactor));

        setTopLeftPosition (20, 20);
       #else
        setTopLeftPosition (node->properties.getWithDefault (getLastXProp (type), Random::getSystemRandom().nextInt (500)),
                            node->properties.getWithDefault (getLastYProp (type), Random::getSystemRandom().nextInt (500)));
       #endif

        node->properties.set (getOpenProp (type), true);

        setVisible (false);
    }

    ~PluginWindow() override {
        clearContentComponent();
    }

    void moved() override {
        node->properties.set (getLastXProp (type), getX());
        node->properties.set (getLastYProp (type), getY());
    }

    void closeButtonPressed() override {
        node->properties.set (getOpenProp (type), false);
        setVisible (false);
    }

    static String getLastXProp (Type type)    { return "uiLastX_" + getTypeName (type); }
    static String getLastYProp (Type type)    { return "uiLastY_" + getTypeName (type); }
    static String getOpenProp  (Type type)    { return "uiopen_"  + getTypeName (type); }

    const AudioProcessorGraph::Node::Ptr node;
    const Type type;

    BorderSize<int> getBorderThickness() override
    {
       #if JUCE_IOS || JUCE_ANDROID
        const int border = 10;
        return { border, border, border, border };
       #else
        return DocumentWindow::getBorderThickness();
       #endif
    }

private:
    float getDesktopScaleFactor() const override     { return 1.0f; }

    static AudioProcessorEditor* createProcessorEditor (AudioProcessor& processor,
                                                        PluginWindow::Type type)
    {
        if (type == PluginWindow::Type::normal) {
            if (processor.hasEditor())
                if (auto* ui = processor.createEditorIfNeeded())
                    return ui;

            type = PluginWindow::Type::generic;
        }

        if (type == PluginWindow::Type::generic)  return new GenericAudioProcessorEditor (processor);

        jassertfalse;
        return {};
    }

    static String getTypeName (Type type) {
        switch (type) {
            case Type::normal:     return "Normal";
            case Type::generic:    return "Generic";
            case Type::numTypes:
            default:               return {};
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginWindow)
};

}



#endif // __hdr_PluginWindow_h__
