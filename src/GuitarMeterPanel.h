#pragma once
#include <JuceHeader.h>
#include "Colors.h"

class GuitarMeterPanel : public juce::Component,
                   private juce::Timer
{
public:
    GuitarMeterPanel();
    ~GuitarMeterPanel() override;

    void updateValues (float detectedFrequency, 
                       float targetFrequency,
                       const juce::String& detectedNote,
                       const juce::String& targetNote,
                       int stringNumber,
                       float centsDeviation);
    
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    void timerCallback() override;
    void drawVerticalMeter (juce::Graphics& g, juce::Rectangle<int> area);
    
    float currentFrequency;
    float currentTargetFrequency;
    juce::String currentDetectedNote;
    juce::String currentTargetNote;
    int currentStringNumber;
    float currentCentsDeviation;
    
    float smoothedCents;
    float currentSignalLevel;  // ← добавить
    
    static constexpr float MAX_CENTS = 200.0f;  // было 50.0f
    //static constexpr float MAX_CENTS = 50.0f;
    static constexpr float SMOOTHING_FACTOR = 0.2f;
};