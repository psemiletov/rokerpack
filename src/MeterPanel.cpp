#include "MeterPanel.h"

MeterPanel::MeterPanel()
    : currentFrequency (0.0f)
    , currentTargetFrequency (0.0f)
    , currentDetectedNote ("--")
    , currentTargetNote ("--")
    , currentStringNumber (-1)
    , currentCentsDeviation (0.0f)
    , smoothedCents (0.0f)
{
    startTimerHz (30);
}

MeterPanel::~MeterPanel()
{
    stopTimer();
}

void MeterPanel::updateValues (float detectedFrequency, 
                               float targetFrequency,
                               const juce::String& detectedNote,
                               const juce::String& targetNote,
                               int stringNumber,
                               float centsDeviation)
{
    currentFrequency = detectedFrequency;
    currentTargetFrequency = targetFrequency;
    currentDetectedNote = detectedNote;
    currentTargetNote = targetNote;
    currentStringNumber = stringNumber;
    currentCentsDeviation = centsDeviation;
}

void MeterPanel::timerCallback()
{
    smoothedCents = smoothedCents + SMOOTHING_FACTOR * (currentCentsDeviation - smoothedCents);
    repaint();
}

void MeterPanel::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    g.setColour (Colors::bgDark.withAlpha (0.5f));
    g.fillRect (bounds);
    
    // Зоны для высоты 480
    auto topArea = bounds.removeFromTop (85);      // Верхняя зона
    auto middleArea = bounds.removeFromTop (240);  // Средняя зона (шкала)
    auto bottomArea = bounds;                       // Нижняя зона (частота)
    
    // === 1. ВЕРХНЯЯ ЗОНА ===
    float yOffset = 5;
    
    if (currentStringNumber > 0)
    {
        g.setFont (juce::Font (15.0f, juce::Font::bold));
        g.setColour (Colors::brassLight);
        g.drawText ("STRING " + juce::String (currentStringNumber), 
                    topArea.getX(), 
                    topArea.getY() + yOffset,
                    topArea.getWidth(),
                    20,
                    juce::Justification::centred);
        yOffset += 24;
    }
    
    g.setColour (Colors::brassHighlight);
    g.setFont (juce::Font (28.0f, juce::Font::bold));
    g.drawText (currentTargetNote, 
                topArea.getX(), 
                topArea.getY() + yOffset,
                topArea.getWidth(),
                38,
                juce::Justification::centred);
    yOffset += 42;
    
    g.setColour (Colors::textLight);
    g.setFont (juce::Font (16.0f, juce::Font::bold));
    juce::String targetFreqText = juce::String (currentTargetFrequency, 1) + " Hz";
    g.drawText (targetFreqText, 
                topArea.getX(), 
                topArea.getY() + yOffset,
                topArea.getWidth(),
                22,
                juce::Justification::centred);
    
    // === 2. СРЕДНЯЯ ЗОНА ===
    drawVerticalMeter (g, middleArea);
    
    // === 3. НИЖНЯЯ ЗОНА ===
    // Прижимаем частоту к нижней части окна
    float fontSize = 34.0f;
    float textY = bottomArea.getBottom() - fontSize - 20;
    
    g.setColour (Colors::brassHighlight);
    g.setFont (juce::Font (fontSize, juce::Font::bold));
    
    juce::String detectedText;
    if (currentFrequency <= 0.0f)
        detectedText = "--- Hz";
    else
        detectedText = juce::String (currentFrequency, 1) + " Hz";
    
    g.drawText (detectedText, 
                bottomArea.getX(), 
                (int)textY,
                bottomArea.getWidth(),
                (int)(fontSize + 8),
                juce::Justification::centred);
}

void MeterPanel::drawVerticalMeter (juce::Graphics& g, juce::Rectangle<int> area)
{
    juce::Rectangle<int> meterBg = area.reduced (30, 8);
    g.setColour (Colors::meterBackground);
    g.fillRect (meterBg);
    g.setColour (Colors::brassDark);
    g.drawRect (meterBg, 1.5f);
    
    float centerY = meterBg.getCentreY();
    g.setColour (Colors::brassHighlight);
    g.drawLine (meterBg.getX(), centerY, meterBg.getRight(), centerY, 2.0f);
    
    float clampedCents = juce::jlimit(-MAX_CENTS, MAX_CENTS, smoothedCents);
    float normalized = std::abs(clampedCents) / MAX_CENTS;
    float barHeight = normalized * (meterBg.getHeight() / 2 - 4);
    
    juce::Colour barColor;
    float absCents = std::abs(clampedCents);
    if (absCents > 10.0f)
        barColor = Colors::ledRed;
    else if (absCents > 5.0f)
        barColor = Colors::tooQuietYellow;
    else if (absCents > 1.0f)
        barColor = Colors::brassHighlight;
    else
        barColor = juce::Colours::green;
    
    g.setColour (barColor);
    
    if (clampedCents > 0)
    {
        juce::Rectangle<float> bar(
            meterBg.getX() + 5.0f,
            centerY - barHeight,
            meterBg.getWidth() - 10.0f,
            barHeight
        );
        g.fillRect(bar);
    }
    else if (clampedCents < 0)
    {
        juce::Rectangle<float> bar(
            meterBg.getX() + 5.0f,
            centerY,
            meterBg.getWidth() - 10.0f,
            barHeight
        );
        g.fillRect(bar);
    }
    else
    {
        juce::Rectangle<float> indicator(
            meterBg.getCentreX() - 8.0f,
            centerY - 4.0f,
            16.0f,
            8.0f
        );
        g.fillRect(indicator);
    }
    
    g.setFont (juce::Font (10.0f));
    g.setColour (Colors::brassMid);
    
    g.drawText ("+50", meterBg.getX() - 25, meterBg.getY() + 5, 20, 12, juce::Justification::centredRight);
    g.drawText ("0",   meterBg.getX() - 25, (int)centerY - 6, 20, 12, juce::Justification::centredRight);
    g.drawText ("-50", meterBg.getX() - 25, meterBg.getBottom() - 17, 20, 12, juce::Justification::centredRight);
}

void MeterPanel::resized()
{
}