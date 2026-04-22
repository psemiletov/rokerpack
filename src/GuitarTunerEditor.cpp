#include "GuitarTunerEditor.h"

GuitarTunerAudioEditor::GuitarTunerAudioEditor (GuitarTunerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), isUpdatingUI (false)
{
    setLookAndFeel (&bronzaLookAndFeel);
    
    addAndMakeVisible (meterPanel);
    addAndMakeVisible (stringsPanel);
    
    // === КНОПКА ПОДСКАЗКИ ===
    helpButton.setButtonText ("?");
    helpButton.setColour (juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    helpButton.setColour (juce::TextButton::textColourOffId, Colors::brassLight);
    helpButton.setColour (juce::TextButton::textColourOnId, Colors::brassHighlight);
    helpButton.addListener (this);
    addAndMakeVisible (helpButton);
    
    setSize (DEFAULT_WIDTH, DEFAULT_HEIGHT);
    startTimerHz (30);
}

GuitarTunerAudioEditor::~GuitarTunerAudioEditor()
{
    stopTimer();
    setLookAndFeel (nullptr);
}

void GuitarTunerAudioEditor::buttonClicked (juce::Button* button)
{
    if (button == &helpButton)
    {
        juce::AlertWindow::showMessageBoxAsync (
            juce::MessageBoxIconType::InfoIcon,
            "Tuning Tip",
            "For best results:\n"
            "1. Tune from thinnest string (E4) to thickest (E2).\n"
            "This eliminates residual vibrations from lower strings.\n"
            "2. If tuning from E2 to E4, mute already tuned strings\n"
            "with your hand before tuning the next string."
        );
    }
}

void GuitarTunerAudioEditor::paint (juce::Graphics& g)
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
    g.drawFittedText ("Guitar Tuner", 
                     titleArea.getX() + 2, 
                     titleArea.getY() + 2,
                     titleArea.getWidth(),
                     45,
                     juce::Justification::centredTop, 
                     1);
    
    g.setColour (Colors::brassHighlight);
    g.drawFittedText ("Guitar Tuner", 
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

void GuitarTunerAudioEditor::resized()
{
    auto bounds = getLocalBounds();
    bounds = bounds.reduced (10, 10);
    bounds.removeFromTop (50);
    
    // Позиционируем кнопку "?" в правом верхнем углу
    helpButton.setBounds (bounds.getRight() - 25, bounds.getY() - 45, 22, 22);
    
    // Делим поровну
    int halfWidth = bounds.getWidth() / 2;
    
    auto leftArea = bounds.removeFromLeft (halfWidth);
    auto rightArea = bounds;
    
    meterPanel.setBounds (leftArea.reduced (5, 5));
    stringsPanel.setBounds (rightArea.reduced (5, 5));
}

void GuitarTunerAudioEditor::timerCallback()
{
    updateUIFromProcessor();
}

void GuitarTunerAudioEditor::updateUIFromProcessor()
{
    if (isUpdatingUI)
        return;
    
    isUpdatingUI = true;
    
    bool hasSignal = audioProcessor.isSignalActive();
    
    if (!hasSignal)
    {
        meterPanel.updateValues (0.0f, 0.0f, "--", "--", -1, 0.0f);
        stringsPanel.resetLEDs();
    }
    else
    {
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
            const float stringFreqs[6] = { 82.41f, 110.00f, 146.83f, 196.00f, 246.94f, 329.63f };
            
            for (int i = 0; i < 6; ++i)
            {
                if (std::abs (targetFreqLocal - stringFreqs[i]) < 0.5f)
                {
                    activeStringIndex = i;
                    break;
                }
            }
            
            stringsPanel.setActiveString (activeStringIndex);
        }
    }
    
    isUpdatingUI = false;
}