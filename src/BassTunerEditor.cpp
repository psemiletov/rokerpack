#include "BassTunerEditor.h"

BassTunerAudioEditor::BassTunerAudioEditor (BassTunerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), isUpdatingUI (false)
{
    setLookAndFeel (&bronzaLookAndFeel);
    
    addAndMakeVisible (meterPanel);
    addAndMakeVisible (stringsPanel);
    
    setSize (720, 380);
    startTimerHz (30);
}

BassTunerAudioEditor::~BassTunerAudioEditor()
{
    stopTimer();
    setLookAndFeel (nullptr);
}

void BassTunerAudioEditor::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    juce::ColourGradient mainGradient (
        Colors::bgDark,
        (float)bounds.getX(), (float)bounds.getY(),
        Colors::bgLight,
        (float)bounds.getX(), (float)bounds.getBottom(),
        false
    );
    g.setGradientFill (mainGradient);
    g.fillAll();
    
    juce::ColourGradient shineGradient (
        Colors::brassLight.withAlpha (0.15f),
        (float)bounds.getX(), (float)bounds.getY() + bounds.getHeight() * 0.1f,
        juce::Colour (0x00D4A84C),
        (float)bounds.getX(), (float)bounds.getY() + bounds.getHeight() * 0.35f,
        false
    );
    g.setGradientFill (shineGradient);
    g.fillAll();
    
    g.setColour (Colors::brassMid);
    g.drawRect (bounds, 2.0f);
    g.setColour (Colors::brassLight.withAlpha (0.3f));
    g.drawRect (bounds.reduced (2), 1.0f);
    
    juce::Rectangle<int> titleArea (bounds.getX(), bounds.getY(), bounds.getWidth(), 60);
    
    g.setColour (juce::Colours::black.withAlpha (0.6f));
    g.setFont (juce::Font (28.0f, juce::Font::bold));
    g.drawFittedText ("Bass Tuner", 
                     titleArea.getX() + 2, 
                     titleArea.getY() + 2,
                     titleArea.getWidth(),
                     45,
                     juce::Justification::centredTop, 
                     1);
    
    g.setColour (Colors::brassHighlight);
    g.drawFittedText ("Bass Tuner", 
                     titleArea.getX(), 
                     titleArea.getY(),
                     titleArea.getWidth(),
                     45,
                     juce::Justification::centredTop, 
                     1);
    
    g.setColour (Colors::textLight.withAlpha (0.7f));
    g.setFont (juce::Font (12.0f, juce::Font::italic));
    g.drawFittedText ("by Peter Semiletov", 
                     bounds.getX(), 
                     titleArea.getY() + 38,
                     bounds.getWidth(),
                     18,
                     juce::Justification::centredTop, 
                     1);
    
    g.setColour (Colors::brassMid.withAlpha (0.6f));
    g.drawLine (40.0f, 58.0f, (float)bounds.getWidth() - 40, 58.0f, 1.5f);
}

void BassTunerAudioEditor::resized()
{
    auto bounds = getLocalBounds();
    bounds = bounds.reduced (10, 10);
    bounds.removeFromTop (50);
    
    auto leftArea = bounds.removeFromLeft (bounds.getWidth() * 0.55f);
    auto rightArea = bounds;
    
    leftArea = leftArea.reduced (5, 5);
    rightArea = rightArea.reduced (5, 5);
    
    meterPanel.setBounds (leftArea);
    stringsPanel.setBounds (rightArea);
}

void BassTunerAudioEditor::timerCallback()
{
    updateUIFromProcessor();
}

void BassTunerAudioEditor::updateUIFromProcessor()
{
    if (isUpdatingUI)
        return;
    
    isUpdatingUI = true;
    
    float detectedFreq = audioProcessor.getDetectedFrequency();
    float targetFreq = audioProcessor.getTargetFrequency();
    juce::String detectedNote = audioProcessor.getDetectedNote();
    juce::String targetNote = audioProcessor.getTargetNote();
    int stringNum = audioProcessor.getStringNumber();
    float cents = audioProcessor.getCentsDeviation();
    
    meterPanel.updateValues (detectedFreq, targetFreq, detectedNote, targetNote,
                            stringNum, cents);
    
    if (detectedFreq <= 0.0f)
    {
        stringsPanel.resetLEDs();
    }
    else
    {
        int activeStringIndex = -1;
        float targetFreqLocal = audioProcessor.getTargetFrequency();
        
        const float stringFreqs[4] = { 41.20f, 55.00f, 73.42f, 98.00f };
        
        for (int i = 0; i < 4; ++i)
        {
            if (std::abs (targetFreqLocal - stringFreqs[i]) < 0.5f)
            {
                activeStringIndex = i;
                break;
            }
        }
        
        stringsPanel.setActiveString (activeStringIndex);
    }
    
    isUpdatingUI = false;
}