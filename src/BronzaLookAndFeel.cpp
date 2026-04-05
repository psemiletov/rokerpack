#include "BronzaLookAndFeel.h"

BronzaLookAndFeel::BronzaLookAndFeel()
{
    // Бронзовая палитра с основным цветом #472B15
    bronzeDark = juce::Colour (0xFF3A2210);     // Темнее основного
    bronzeMain = juce::Colour (0xFF472B15);    // Основной цвет #472B15
    bronzeLight = juce::Colour (0xFF5C3A1F);   // Светлее основного
    bronzeHighlight = juce::Colour (0xFF7A4F2A); // Яркий блик
    textLight = juce::Colour (0xFFF0D8B0);     // Кремовый текст
    
    // Настройка цветов для различных компонентов JUCE
    setColour (juce::ResizableWindow::backgroundColourId, bronzeDark);
    setColour (juce::Label::textColourId, textLight);
    setColour (juce::Slider::textBoxTextColourId, textLight);
    setColour (juce::Slider::textBoxBackgroundColourId, bronzeDark);
    setColour (juce::Slider::textBoxOutlineColourId, bronzeLight);
    setColour (juce::Slider::rotarySliderFillColourId, bronzeHighlight);
    setColour (juce::Slider::rotarySliderOutlineColourId, bronzeDark);
}

BronzaLookAndFeel::~BronzaLookAndFeel()
{
}

void BronzaLookAndFeel::drawRotarySlider (juce::Graphics& g,
                                          int x, int y,
                                          int width, int height,
                                          float sliderPos,
                                          float rotaryStartAngle,
                                          float rotaryEndAngle,
                                          juce::Slider& slider)
{
    auto radius = (float) juce::jmin (width / 2, height / 2) - 4.0f;
    auto centreX = (float) x + (float) width * 0.5f;
    auto centreY = (float) y + (float) height * 0.5f;
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    
    // 1. Тень
    g.setColour (juce::Colours::black.withAlpha (0.4f));
    g.fillEllipse (centreX - radius + 2, centreY - radius + 2, radius * 2, radius * 2);
    
    // 2. Основной фон кнобы
    g.setColour (bronzeMain);
    g.fillEllipse (centreX - radius, centreY - radius, radius * 2, radius * 2);
    g.setColour (bronzeDark);
    g.drawEllipse (centreX - radius, centreY - radius, radius * 2, radius * 2, 1.5f);
    
    // 3. Вращающиеся выступы
    int numRidges = 36;
    float ridgeRadius = radius - 3.0f;
    float ridgeSize = 3.5f;
    
    for (int i = 0; i < numRidges; ++i)
    {
        float ridgeAngle = (float) i / (float) numRidges * juce::MathConstants<float>::twoPi;
        float rotatedRidgeAngle = ridgeAngle + angle;
        
        float ridgeX = centreX + ridgeRadius * std::cos (rotatedRidgeAngle);
        float ridgeY = centreY + ridgeRadius * std::sin (rotatedRidgeAngle);
        
        g.setColour (bronzeLight);
        g.fillEllipse (ridgeX - ridgeSize * 0.5f, ridgeY - ridgeSize * 0.5f, ridgeSize, ridgeSize);
        g.setColour (bronzeDark);
        g.drawEllipse (ridgeX - ridgeSize * 0.5f, ridgeY - ridgeSize * 0.5f, ridgeSize, ridgeSize, 0.5f);
    }
    
    // 4. Внутренний диск
    float innerRadius = radius - 8.0f;
    g.setColour (bronzeDark);
    g.fillEllipse (centreX - innerRadius, centreY - innerRadius, innerRadius * 2, innerRadius * 2);
    g.setColour (bronzeLight);
    g.drawEllipse (centreX - innerRadius, centreY - innerRadius, innerRadius * 2, innerRadius * 2, 1.0f);
    
    // 5. Треугольная стрелка
    juce::Path pointer;
    float pointerLength = innerRadius * 0.6f;
    float pointerWidth = innerRadius * 0.12f;
    
    pointer.addTriangle (0.0f, -innerRadius + 6.0f,
                        -pointerWidth * 0.5f, -innerRadius + pointerLength,
                        pointerWidth * 0.5f, -innerRadius + pointerLength);
    
    juce::AffineTransform transform = juce::AffineTransform::rotation (angle);
    pointer.applyTransform (transform);
    pointer.applyTransform (juce::AffineTransform::translation (centreX, centreY));
    
    g.setColour (bronzeHighlight);
    g.fillPath (pointer);
    
    // 6. Центральная точка
    g.setColour (bronzeDark);
    g.fillEllipse (centreX - 4.0f, centreY - 4.0f, 8.0f, 8.0f);
    g.setColour (bronzeHighlight);
    g.fillEllipse (centreX - 2.0f, centreY - 2.0f, 4.0f, 4.0f);
}