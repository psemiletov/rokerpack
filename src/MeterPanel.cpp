#include "MeterPanel.h"
#include <iostream>

MeterPanel::MeterPanel()
    : currentFrequency (0.0f)
    , currentTargetFrequency (0.0f)
    , currentDetectedNote ("--")
    , currentTargetNote ("--")
    , currentStringNumber (-1)
    , currentCentsDeviation (0.0f)
    , smoothedCents (0.0f)
    , currentSignalLevel (0.0f)
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
    currentSignalLevel = (detectedFrequency > 0.0f) ? 1.0f : 0.0f;
    
    // Отладка
//    std::cout << "MeterPanel: freq=" << detectedFrequency 
  //            << " target=" << targetFrequency 
    //          << " cents=" << centsDeviation << std::endl;
}

void MeterPanel::timerCallback()
{
    smoothedCents = currentCentsDeviation;
    repaint();
}

void MeterPanel::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    g.setColour (Colors::bgDark.withAlpha (0.5f));
    g.fillRect (bounds);
    
    auto topArea = bounds.removeFromTop (85);
    auto middleArea = bounds.removeFromTop (240);
    auto bottomArea = bounds;
    
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
    juce::String targetFreqText = juce::String (currentTargetFrequency, 2) + " Hz";
    g.drawText (targetFreqText, 
                topArea.getX(), 
                topArea.getY() + yOffset,
                topArea.getWidth(),
                22,
                juce::Justification::centred);
    
    //std::cout << "paint: signalLevel=" << currentSignalLevel << " freq=" << currentFrequency << std::endl;
    
    // === 2. СРЕДНЯЯ ЗОНА ===
    if (currentSignalLevel > 0.0f && currentFrequency > 0.0f)
    {
        drawVerticalMeter (g, middleArea);
    }
    else
    {
        juce::Rectangle<int> meterBg = middleArea.reduced (30, 8);
        g.setColour (Colors::meterBackground);
        g.fillRect (meterBg);
        g.setColour (Colors::brassDark);
        g.drawRect (meterBg, 1.5f);
        
        float centerY = meterBg.getCentreY();
        g.setColour (Colors::brassDark.withAlpha (0.5f));
        g.drawLine (meterBg.getX(), centerY, meterBg.getRight(), centerY, 2.0f);
        
        g.setFont (juce::Font (10.0f));
        g.setColour (Colors::brassDark.withAlpha (0.5f));
        g.drawText ("+200", meterBg.getX() - 25, meterBg.getY() + 5, 20, 12, juce::Justification::centredRight);
        g.drawText ("0",   meterBg.getX() - 25, (int)centerY - 6, 20, 12, juce::Justification::centredRight);
        g.drawText ("-200", meterBg.getX() - 25, meterBg.getBottom() - 17, 20, 12, juce::Justification::centredRight);
    }
    
    // === 3. НИЖНЯЯ ЗОНА ===
    float fontSize = 34.0f;
    float textY = bottomArea.getBottom() - fontSize - 20;
    
    if (std::abs(smoothedCents) < 1.0f && currentFrequency > 0.0f)
    {
        g.setColour (juce::Colours::green);
    }
    else
    {
        g.setColour (Colors::brassHighlight);
    }
    
    g.setFont (juce::Font (fontSize, juce::Font::bold));
    
    juce::String detectedText;
    if (currentFrequency <= 0.0f)
        detectedText = "--- Hz";
    else
        detectedText = juce::String (currentFrequency, 2) + " Hz";
    
    g.drawText (detectedText, 
                bottomArea.getX(), 
                (int)textY,
                bottomArea.getWidth(),
                (int)(fontSize + 8),
                juce::Justification::centred);
}

void MeterPanel::drawVerticalMeter (juce::Graphics& g, juce::Rectangle<int> area)
{
   //std::cout << "drawVerticalMeter: cents=" << smoothedCents << std::endl;
   
    juce::Rectangle<int> meterBg = area.reduced (30, 8);
    g.setColour (Colors::meterBackground);
    g.fillRect (meterBg);
    g.setColour (Colors::brassDark);
    g.drawRect (meterBg, 1.5f);
    
    float centerY = meterBg.getCentreY();
    
    float clampedCents = smoothedCents;
    float normalized = std::abs(clampedCents) / MAX_CENTS;
    float barHeight = normalized * (meterBg.getHeight() / 2 - 4);
    
    float absPercent = std::abs(clampedCents) / MAX_CENTS;
    juce::Colour barColor;
    
    if (absPercent < 0.02f)
        barColor = juce::Colours::green;
    else if (absPercent < 0.1f)
        barColor = Colors::brassHighlight;
    else if (absPercent < 0.3f)
        barColor = Colors::tooQuietYellow;
    else
        barColor = Colors::ledRed;
    
    g.setColour (barColor);
    
    // Центральная линия
    if (absPercent < 0.02f)
    {
        g.setColour (juce::Colours::green);
        g.drawLine (meterBg.getX(), centerY, meterBg.getRight(), centerY, 4.0f);
    }
    else
    {
        g.setColour (Colors::brassHighlight);
        g.drawLine (meterBg.getX(), centerY, meterBg.getRight(), centerY, 2.0f);
    }
    
    // Рисуем полоску
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
            meterBg.getCentreX() - 12.0f,
            centerY - 6.0f,
            24.0f,
            12.0f
        );
        g.fillRect (indicator);
    }
    
    // Метки шкалы
    g.setFont (juce::Font (10.0f));
    g.setColour (Colors::brassMid);
    
    g.drawText ("+" + juce::String((int)MAX_CENTS), meterBg.getX() - 25, meterBg.getY() + 5, 20, 12, juce::Justification::centredRight);
    g.drawText ("0", meterBg.getX() - 25, (int)centerY - 6, 20, 12, juce::Justification::centredRight);
    g.drawText ("-" + juce::String((int)MAX_CENTS), meterBg.getX() - 25, meterBg.getBottom() - 17, 20, 12, juce::Justification::centredRight);
}

void MeterPanel::resized()
{
}