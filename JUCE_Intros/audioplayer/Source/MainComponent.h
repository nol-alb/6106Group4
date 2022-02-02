#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent,
                       public juce::ChangeListener
                        
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    //==============================================================================
    // Your private member variables go here...
    enum TransportState
    // good to have some states that help in deciding the flow of the audio player
    {
        Stopped,
        Starting,
        Stoping,
        Playing,
        
        
    };
    TransportState state;
    
    void openButtonClicked();
    void playButtonClicked();
    void stopButtonClicked();
    void TransportStateChanged(TransportState newState);
    void changeListenerCallback (juce::ChangeBroadcaster *source) override;
    
    juce::AudioTransportSource transport;
    juce::TextButton openButton;
    juce::AudioFormatManager FormatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> playSource;
    juce::TextButton playButton;
    juce::TextButton stopButton;
    
    
    


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
