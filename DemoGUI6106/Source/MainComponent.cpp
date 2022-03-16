#include "MainComponent.h"

//==============================================================================

MainComponent::MainComponent()
{
    setSize (1150, 700);
    addAndMakeVisible(sidebar);
    addAndMakeVisible(spectrogram);
    addAndMakeVisible(bandchain);
}

MainComponent::~MainComponent()
{
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

}

void MainComponent::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    auto area = getLocalBounds();
   
    
    int sidebarWidth = static_cast<int>(area.getWidth() / 5);
    sidebar.setBounds(area.removeFromRight(sidebarWidth));
    
    int middleHeight = static_cast<int>(area.getHeight() / 2);
    spectrogram.setBounds(area.removeFromTop(middleHeight));
    bandchain.setBounds(area.removeFromBottom(middleHeight));
    
}






//==============================================================================

SideBarView::SideBarView()
{
    setSize(10, 700);
    
    wetSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    wetSlider.setValue(6.5);
    //wetSlider.setRange(),addListener
    addAndMakeVisible(wetSlider);
    
    drySlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    drySlider.setValue(4.0);
    //drySlider....see above
    addAndMakeVisible(drySlider);
}

SideBarView::~SideBarView()
{
    // n/a
}

void SideBarView::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::darkgrey);
    auto area = getLocalBounds();
    
    g.setFont (juce::Font (12.0f));
    g.setColour (juce::Colours::white);
    g.drawText("Group 4 Demo GUI v1.0.2", 3, area.getHeight() - 13, area.getWidth() - 5, 13, juce::Justification::left);
    
    juce::File powerPlaceholder("~/Documents/PlusPlus/MUSI6106/6106Group4/DemoGUI6106/GUIImages/spbi.png");
    juce::Image powerImg = juce::ImageCache::getFromFile(powerPlaceholder);
    int lowerPowerLimit = area.getWidth() - 30;
    g.drawImage(powerImg, 5, 0, lowerPowerLimit, lowerPowerLimit, 0, 0, powerImg.getWidth() - 30, powerImg.getHeight() - 30);
    
    int textYPlace = lowerPowerLimit + static_cast<int>(lowerPowerLimit * 1.3);
    g.drawText("Wet", 55, textYPlace, 20, 13, juce::Justification::left);
    g.drawText("Dry", area.getWidth() - 70, textYPlace, 20, 13, juce::Justification::left);
    
    juce::File settingsPlaceholder("~/Documents/PlusPlus/MUSI6106/6106Group4/DemoGUI6106/GUIImages/settings.png");
    juce::Image settingsImg = juce::ImageCache::getFromFile(settingsPlaceholder);
    g.drawImage(settingsImg, 83, textYPlace + 120, 75, 75, 0, 0, settingsImg.getWidth(), settingsImg.getWidth());
}

void SideBarView::resized()
{
    auto area = getLocalBounds();
    int lowerPowerLimit = area.getWidth() - 30;
    wetSlider.setBounds(55, lowerPowerLimit, 20, static_cast<int>(lowerPowerLimit * 1.3));
    drySlider.setBounds(area.getWidth() - 70, lowerPowerLimit, 20, static_cast<int>(lowerPowerLimit * 1.3));
}

//==============================================================================

VisualizerView::VisualizerView()
{
    setSize(10, 700);
}

VisualizerView::~VisualizerView()
{
    // n/a
}

void VisualizerView::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);
    auto area = getLocalBounds();

    juce::File spectroPlaceholder("~/Documents/PlusPlus/MUSI6106/6106Group4/DemoGUI6106/GUIImages/SPECT.png");
    juce::Image spectroImg = juce::ImageCache::getFromFile(spectroPlaceholder);
    g.drawImage(spectroImg, 10, 10, area.getWidth() - 20, area.getHeight() - 20, 0, 0, spectroImg.getWidth(), spectroImg.getHeight());
}

void VisualizerView::resized()
{
    //
}

//==============================================================================

BandChainView::BandChainView()
{
    setSize(10, 700);
    
    addAndMakeVisible(lowBand);
    addAndMakeVisible(midBand);
    addAndMakeVisible(highBand);
}

BandChainView::~BandChainView()
{
    // n/a
}

void BandChainView::paint (juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    auto area = getLocalBounds();

    g.setFont (juce::Font (12.0f));
    g.setColour (juce::Colours::white);

    g.drawVerticalLine(area.getWidth() / 3, 0, area.getHeight());
    g.drawVerticalLine(2 * area.getWidth() / 3, 0, area.getHeight());
}

void BandChainView::resized()
{
    auto area = getLocalBounds();
    
    
    lowBand.setBounds(area.removeFromLeft(area.getWidth() / 3));
    
    highBand.setBounds(area.removeFromRight(static_cast<int>(area.getWidth() * 0.495)));
    
    midBand.setBounds(area.getWidth(), 0, area.getWidth() - 10, area.getHeight());
}





BandChainView::BandView::BandView()
{
    setSize(1000, 350);
    
    panSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    panSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    panSlider.setValue(5.0);
    panSlider.setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, juce::Colours::black);
    panSlider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::black);
    addAndMakeVisible(panSlider);
    
    gainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    gainSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    gainSlider.setValue(7.0);
    gainSlider.setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, juce::Colours::black);
    gainSlider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::black);
    addAndMakeVisible(gainSlider);
    
}

BandChainView::BandView::~BandView()
{
    //
}

void BandChainView::BandView::paint (juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    auto area = getLocalBounds();

    juce::File powerPlaceholder("~/Documents/PlusPlus/MUSI6106/6106Group4/DemoGUI6106/GUIImages/spbi_alt.png");
    powerImg = juce::ImageCache::getFromFile(powerPlaceholder);
    g.drawImage(powerImg, area.getWidth() / 2 - 20, 9, 40, 40, 0, 0, powerImg.getWidth(), powerImg.getHeight());
    
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(10.0F));
    g.drawText("P", 45, 1, 20, 12, juce::Justification::centred);
    g.drawText("G", area.getWidth() - 60, 1, 20, 12, juce::Justification::centred);
    
    g.setColour(juce::Colours::lightgrey);
    g.drawHorizontalLine(60, 10, area.getWidth() - 10);
    
    std::string ops[] = {"Reverb", "Compression", "Filter", "Delay"};
    int pluginRepSize = static_cast<int>(area.getHeight() - 75) / 3;
    const int AMOUNT_OF_PLUGINS = 3;
    bool noneBefore = false;
    for (int i = 0; i < AMOUNT_OF_PLUGINS; i++) {
        if (i == 0) {
            noneBefore = false;
        }
        int ypos = 66 + ((3 + pluginRepSize) * i);
        g.setColour(juce::Colours::lightgrey);
        g.fillRoundedRectangle(10, ypos, area.getWidth() - 20, pluginRepSize, 5);
        
        g.setColour(juce::Colours::black);
        g.setFont(juce::Font(16.0F));
        
        bool pluginLoaded = (rand() % 10 + 1) <= 5;
        std::string boxTitle;
        if (pluginLoaded && !noneBefore) {
            boxTitle = ops[rand() % 4];
            g.drawRect((area.getWidth() / 2) + 80, ypos + (pluginRepSize * 0.42), 40, 20);
            g.drawText("view", (area.getWidth() / 2) + 86, ypos + (pluginRepSize * 0.45), 60, 14, juce::Justification::left);
        } else {
            boxTitle = "NO PLUGIN";
            noneBefore = true;
        }
        
        g.setFont(juce::Font(24.0F));
        g.drawText(boxTitle, 25, ypos + (pluginRepSize * 0.39), 150, 25, juce::Justification::left);
        
        g.drawRect((area.getWidth() / 2) + 30, ypos + (pluginRepSize * 0.42), 40, 20);
        g.setFont(juce::Font(16.0F));
        g.drawText("load", (area.getWidth() / 2) + 36, ypos + (pluginRepSize * 0.45), 60, 14, juce::Justification::left);
        
        
    }
}

void BandChainView::BandView::resized()
{
    auto area = getLocalBounds();
    
    panSlider.setBounds(30, 8, 50, 50);
    gainSlider.setBounds(area.getWidth() - 75, 10, 50, 50);
}
