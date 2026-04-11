#include "GuitarTunerEditor.h"

//==============================================================================
GuitarTunerAudioEditor::GuitarTunerAudioEditor (GuitarTunerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), isUpdatingUI (false)
{
    // Устанавливаем кастомный LookAndFeel
    setLookAndFeel (&bronzaLookAndFeel);
    
    // Настраиваем колбэк для панели струн
    stringsPanel.onStringSelectionChanged = [this] (bool manual, int stringIndex)
    {
        onStringSelectionChanged (manual, stringIndex);
    };
    
    // Добавляем компоненты
    addAndMakeVisible (meterPanel);
    addAndMakeVisible (stringsPanel);
    
    // Устанавливаем размер окна
    setSize (720, 400);
    
    // Запускаем таймер для обновления UI (30 fps)
    startTimerHz (30);
}

GuitarTunerAudioEditor::~GuitarTunerAudioEditor()
{
    stopTimer();
    setLookAndFeel (nullptr);
}

//==============================================================================
void GuitarTunerAudioEditor::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // === Фон в стиле Bronza ===
    juce::ColourGradient mainGradient (
        Colors::bgDark,
        (float)bounds.getX(), (float)bounds.getY(),
        Colors::bgLight,
        (float)bounds.getX(), (float)bounds.getBottom(),
        false
    );
    g.setGradientFill (mainGradient);
    g.fillAll();
    
    // Металлический блик
    juce::ColourGradient shineGradient (
        Colors::brassLight.withAlpha (0.15f),
        (float)bounds.getX(), (float)bounds.getY() + bounds.getHeight() * 0.1f,
        juce::Colour (0x00D4A84C),
        (float)bounds.getX(), (float)bounds.getY() + bounds.getHeight() * 0.35f,
        false
    );
    g.setGradientFill (shineGradient);
    g.fillAll();
    
    // === Рамка ===
    g.setColour (Colors::brassMid);
    g.drawRect (bounds, 2.0f);
    g.setColour (Colors::brassLight.withAlpha (0.3f));
    g.drawRect (bounds.reduced (2), 1.0f);
    
    // === Заголовок ===
    juce::Rectangle<int> titleArea (bounds.getX(), bounds.getY(), bounds.getWidth(), 60);
    
    // Тень
    g.setColour (juce::Colours::black.withAlpha (0.6f));
    g.setFont (juce::Font (28.0f, juce::Font::bold));
    g.drawFittedText ("Guitar Tuner", 
                     titleArea.getX() + 2, 
                     titleArea.getY() + 2,
                     titleArea.getWidth(),
                     45,
                     juce::Justification::centredTop, 
                     1);
    
    // Основной текст
    g.setColour (Colors::brassHighlight);
    g.drawFittedText ("Guitar Tuner", 
                     titleArea.getX(), 
                     titleArea.getY(),
                     titleArea.getWidth(),
                     45,
                     juce::Justification::centredTop, 
                     1);
    
    // Подпись
    g.setColour (Colors::textLight.withAlpha (0.7f));
    g.setFont (juce::Font (12.0f, juce::Font::italic));
    g.drawFittedText ("by Peter Semiletov", 
                     bounds.getX(), 
                     titleArea.getY() + 38,
                     bounds.getWidth(),
                     18,
                     juce::Justification::centredTop, 
                     1);
    
    // Декоративная линия
    g.setColour (Colors::brassMid.withAlpha (0.6f));
    g.drawLine (40.0f, 58.0f, (float)bounds.getWidth() - 40, 58.0f, 1.5f);
}

//==============================================================================
void GuitarTunerAudioEditor::resized()
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

//==============================================================================
void GuitarTunerAudioEditor::timerCallback()
{
    updateUIFromProcessor();
}

void GuitarTunerAudioEditor::updateUIFromProcessor()
{
   
    std::cout << "updateUIFromProcessor called, detectedFreq=" << audioProcessor.getDetectedFrequency() << std::endl;
    
    
   
   if (isUpdatingUI)
        return;
    
    isUpdatingUI = true;
    
    float detectedFreq = audioProcessor.getDetectedFrequency();
    float targetFreq = audioProcessor.getTargetFrequency();
    juce::String detectedNote = audioProcessor.getDetectedNote();
    juce::String targetNote = audioProcessor.getTargetNote();
    int stringNum = audioProcessor.getStringNumber();
    float cents = audioProcessor.getCentsDeviation();
    
    // Обновляем MeterPanel
    meterPanel.updateValues (detectedFreq, targetFreq, detectedNote, targetNote,
                            stringNum, cents);
    
    // Обновляем StringsPanel
    if (detectedFreq <= 0.0f)
    {
        stringsPanel.resetLEDs();
    }
    else
    {
        int activeStringIndex = -1;
        float targetFreqLocal = audioProcessor.getTargetFrequency();
        
        // Эталонные частоты струн (0 = 6-я, 5 = 1-я)
        const float stringFreqs[6] = { 82.41f, 110.00f, 146.83f, 196.00f, 246.94f, 329.63f };
        
        for (int i = 0; i < 6; ++i)
        {
            if (std::abs (targetFreqLocal - stringFreqs[i]) < 0.5f)
            {
                activeStringIndex = i;
                break;
            }
        }
        
        stringsPanel.setActiveString (activeStringIndex, stringsPanel.isManualMode());
    }
    
    isUpdatingUI = false;
}

void GuitarTunerAudioEditor::onStringSelectionChanged (bool manualMode, int stringIndex)
{
    audioProcessor.setManualMode (manualMode, stringIndex);
    stringsPanel.setManualMode (manualMode, stringIndex);
}