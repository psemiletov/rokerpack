#include "BassTunerEditor.h"
#include "BassStringsPanel.h"

// Частоты струн бас-гитары
constexpr float BASS_STRING_FREQS[NUM_BASS_STRINGS] = { 41.20f, 55.00f, 73.42f, 98.00f };
constexpr float FREQ_TOLERANCE = 0.5f;
constexpr float SILENCE_THRESHOLD = 0.5f;

BassTunerAudioEditor::BassTunerAudioEditor (BassTunerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setLookAndFeel (&bronzaLookAndFeel);
    
    addAndMakeVisible (meterPanel);
    addAndMakeVisible (stringsPanel);
    
    setSize (DEFAULT_WIDTH, DEFAULT_HEIGHT);
//    startTimerHz (30);
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
/*
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
*/

void BassTunerAudioEditor::resized()
{
    auto bounds = getLocalBounds();
    bounds = bounds.reduced (10, 10);
    bounds.removeFromTop (50);
    
    auto leftArea = bounds.removeFromLeft (bounds.getWidth() * 0.45f);
    auto rightArea = bounds;
    
    leftArea = leftArea.reduced (5, 5);
    rightArea = rightArea.reduced (5, 5);
    
    meterPanel.setBounds (leftArea);
    stringsPanel.setBounds (rightArea);
}
void BassTunerAudioEditor::timerCallback()
{
    float detectedFreq = audioProcessor.getDetectedFrequency();
    float targetFreq = audioProcessor.getTargetFrequency();
    juce::String detectedNote = audioProcessor.getDetectedNote();
    juce::String targetNote = audioProcessor.getTargetNote();
    int stringNum = audioProcessor.getStringNumber();
    float cents = audioProcessor.getCentsDeviation();
    
    meterPanel.updateValues (detectedFreq, targetFreq, detectedNote, targetNote,
                            stringNum, cents);
    
    // Обновляем подсветку струн
    if (detectedFreq < SILENCE_THRESHOLD)
    {
        stringsPanel.setActiveString(-1);
    }
    else
    {
        int activeStringIndex = -1;
        
        for (int i = 0; i < NUM_BASS_STRINGS; ++i)
        {
            if (std::abs (targetFreq - BASS_STRING_FREQS[i]) < FREQ_TOLERANCE)
            {
                activeStringIndex = i;
                break;
            }
        }
        
        stringsPanel.setActiveString (activeStringIndex);
    }
}