#pragma once

#include <JuceHeader.h>

class SideBarView : public juce::Component
{
public:
    SideBarView();
    ~SideBarView() override;
private:
    void paint (juce::Graphics&) override;
    void resized() override;
};

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::Component
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    // Your private member variables go here...
    SideBarView sidebar;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};




