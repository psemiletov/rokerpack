#pragma once
#include <JuceHeader.h>

class BronzaLookAndFeel : public juce::LookAndFeel_V4
{
public:
    BronzaLookAndFeel();
    ~BronzaLookAndFeel() override;

    // Переопределяем только отрисовку rotary sliders
    void drawRotarySlider (juce::Graphics& g, 
                          int x, int y, 
                          int width, int height,
                          float sliderPos,
                          float rotaryStartAngle,
                          float rotaryEndAngle,
                          juce::Slider& slider) override;

    // НЕ переопределяем drawLabel (пусть JUCE использует стандартный)

private:
    // Вспомогательные цвета
    juce::Colour bodyColour;
    juce::Colour knobColour;
    juce::Colour knobOutlineColour;
    juce::Colour indicatorColour;
    juce::Colour textColour;
};