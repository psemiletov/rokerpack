#pragma once
#include <JuceHeader.h>
#include "Colors.h"

class MeterPanel : public juce::Component,
                   private juce::Timer
{
public:
    MeterPanel();
    ~MeterPanel() override;

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
    void drawVerticalMeter (juce::Graphics& g, juce::Rectangle<int> area);  // ← новое название
    
    float currentFrequency;
    float currentTargetFrequency;
    juce::String currentDetectedNote;
    juce::String currentTargetNote;
    int currentStringNumber;
    float currentCentsDeviation;
    
    float smoothedCents;
    
    static constexpr float MAX_CENTS = 50.0f;
    static constexpr float SMOOTHING_FACTOR = 0.2f;
};