#pragma once
#include <JuceHeader.h>

class BronzaLookAndFeel : public juce::LookAndFeel_V4
{
public:
    BronzaLookAndFeel();
    ~BronzaLookAndFeel() override;

    void drawRotarySlider (juce::Graphics& g, 
                          int x, int y, 
                          int width, int height,
                          float sliderPos,
                          float rotaryStartAngle,
                          float rotaryEndAngle,
                          juce::Slider& slider) override;

private:
    // Цвета для фона и акцентов
    juce::Colour bgDark;
    juce::Colour bgLight;
    juce::Colour textLight;
    
    // Латунные цвета для кнобов
    juce::Colour brassDark;
    juce::Colour brassMid;
    juce::Colour brassLight;
    juce::Colour brassHighlight;
};