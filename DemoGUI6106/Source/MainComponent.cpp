#include "MainComponent.h"

//==============================================================================

MainComponent::MainComponent()
{
    setSize (1150, 700);
    //setCentrePosition(10, 10);
    addAndMakeVisible(sidebar);
}

MainComponent::~MainComponent()
{
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setFont (juce::Font (26.0f));
    g.setColour (juce::Colours::white);
    g.drawText ("Hello World!", getLocalBounds(), juce::Justification::centred, true);
}

void MainComponent::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    
    auto area = getLocalBounds();
    int sidebarWidth = 200;
    sidebar.setBounds(area.removeFromRight(sidebarWidth));
}


//==============================================================================

SideBarView::SideBarView()
{
    setSize(10, 700);
}

SideBarView::~SideBarView()
{
    // n/a
}

void SideBarView::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::white);

    g.setFont (juce::Font (16.0f));
    g.setColour (juce::Colours::black);
    g.drawText ("H!", getLocalBounds(), juce::Justification::centred, true);
}

void SideBarView::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}
