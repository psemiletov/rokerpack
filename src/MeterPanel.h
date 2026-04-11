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
    void drawMeter (juce::Graphics& g, juce::Rectangle<int> area);
    void drawNeedle (juce::Graphics& g, juce::Rectangle<int> area, float cents);
    
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