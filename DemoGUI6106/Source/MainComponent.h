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
    juce::Slider wetSlider;
    juce::Slider drySlider;
};

class VisualizerView : public juce::Component
{
public:
    VisualizerView();
    ~VisualizerView() override;
private:
    void paint (juce::Graphics&) override;
    void resized() override;
};

class BandChainView : public juce::Component
{
public:
    BandChainView();
    ~BandChainView() override;
private:
    void paint (juce::Graphics&) override;
    void resized() override;
    //inner class for specific bands
    class BandView : public juce::Component
    {
    public:
        BandView();
        ~BandView() override;
    private:
        void paint (juce::Graphics&) override;
        void resized() override;
        juce::Slider panSlider;
        juce::Slider gainSlider;
        juce::Image powerImg;
    };
    BandView lowBand;
    BandView midBand;
    BandView highBand;
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
    VisualizerView spectrogram;
    BandChainView bandchain;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};




