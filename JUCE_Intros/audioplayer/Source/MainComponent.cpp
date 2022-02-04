#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent() : state(Stopped), openButton("Open"), playButton("Play"), stopButton("Stop")
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (200, 160);
    addAndMakeVisible(&openButton);
    openButton.onClick = [this] { openButtonClicked(); };
    
    playButton.onClick = [this] { playButtonClicked(); };
    addAndMakeVisible(&playButton);
    playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green );
    playButton.setEnabled(true);
    
    stopButton.onClick = [this] { stopButtonClicked(); };
    addAndMakeVisible(&stopButton);
    stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red );
    stopButton.setEnabled(false);

    
    FormatManager.registerBasicFormats();
    transport.addChangeListener(this);
    //TransportStateChanged(this);
    

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (0, 2);
    }
}


MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
    transport.prepareToPlay(samplesPerBlockExpected, sampleRate);
    
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    bufferToFill.clearActiveBufferRegion();
    transport.getNextAudioBlock(bufferToFill);
}
void MainComponent::openButtonClicked()
{
    DBG("clICKED");
    juce::FileChooser chooser ("Choose A Wav or Aiff file", juce::File::getSpecialLocation(juce::File::userDesktopDirectory),"*.wav; *.mp3");
    if (chooser.browseForFileToOpen())
    {
        juce::File myFile;
        // What did the user choose
        myFile = chooser.getResult();
        //read the file
        juce::AudioFormatReader* reader = FormatManager.createReaderFor(myFile);
        if (reader != nullptr)
        {
        //get the file ready to play
        std::unique_ptr<juce::AudioFormatReaderSource> tempSource (new juce::AudioFormatReaderSource(reader, free));
        
        transport.setSource(tempSource.get());
        TransportStateChanged(Stopped);
            
        DBG(reader->getFormatName());
        playSource.reset(tempSource.release());
        }
        
    }
        
    
    
    // juce::FileChooser chooser("Choose A Wav or Aiff file", juce::File::getSpecialLocation(juce::File::userDesktopDirectory),'*.wav','*.aiff');
    
}
void MainComponent::playButtonClicked()
{
    TransportStateChanged(Starting);
    
}
void MainComponent::stopButtonClicked()
{
    TransportStateChanged(Stoping);
}
void MainComponent::TransportStateChanged(TransportState newState)
{
    //MAKE CHANGES TO THE STATE
    if (newState != state)
    {
        state = newState;
        switch (state) {
            case Stopped:
                playButton.setEnabled(true);
                transport.setPosition(0.0);
                break;
            case Playing:
                playButton.setEnabled(true);
                stopButton.setEnabled(false);
                break;

            case Starting:
                stopButton.setEnabled(true);
                playButton.setEnabled(false);
                transport.start();
                break;
            case Stoping:
                playButton.setEnabled(true);
                stopButton.setEnabled(false);
                transport.stop();
                break;
                
                
        }
        //Stop
            //playbutton enables
            //Transport stop
        //starting
            //stopButton enable
            //transport PLay
        //Stopped

    }
}
void MainComponent::changeListenerCallback(juce::ChangeBroadcaster *source){
    if (source == &transport)
    {
        if (transport.isPlaying())
        {
            TransportStateChanged(Playing);
        }
        else
        {
            TransportStateChanged(Stopped);
        }
    }
}
void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    openButton.setBounds(10, 10, getWidth()-20,30);
    playButton.setBounds(10, 50, getWidth()-20,30);
    stopButton.setBounds(10, 100, getWidth()-20,30);
    
    
    
    
}
