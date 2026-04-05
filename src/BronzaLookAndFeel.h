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
    // Бронзовая цветовая палитра (основной цвет #472B15)
    juce::Colour bronzeDark;      // Тёмная бронза (фон)
    juce::Colour bronzeMain;      // Основная бронза #472B15
    juce::Colour bronzeLight;     // Светлая бронза
    juce::Colour bronzeHighlight; // Яркий бронзовый блик
    juce::Colour textLight;       // Светлый текст
};