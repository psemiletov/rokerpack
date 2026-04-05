#include "BronzaLookAndFeel.h"

BronzaLookAndFeel::BronzaLookAndFeel()
{
    // Фоновые цвета
    bgDark = juce::Colour (0xFF4A2A12);      // Затемнённая бронза
    bgLight = juce::Colour (0xFF7A4F2A);     // Светлая бронза
    textLight = juce::Colour (0xFFF0D8B0);   // Кремовый текст
    
    // Латунные цвета для кнобов
    brassDark = juce::Colour (0xFF8B6914);    // Тёмная латунь
    brassMid = juce::Colour (0xFFC9A03D);     // Средняя латунь
    brassLight = juce::Colour (0xFFE8C66A);   // Светлая латунь
    brassHighlight = juce::Colour (0xFFF5D97A); // Яркая латунь для стрелки
    
    // Настройка цветов для компонентов JUCE
    setColour (juce::ResizableWindow::backgroundColourId, bgDark);
    setColour (juce::Label::textColourId, textLight);
    setColour (juce::Slider::textBoxTextColourId, textLight);
    setColour (juce::Slider::textBoxBackgroundColourId, bgDark);
    setColour (juce::Slider::textBoxOutlineColourId, brassLight);
    setColour (juce::Slider::rotarySliderFillColourId, brassHighlight);
    setColour (juce::Slider::rotarySliderOutlineColourId, brassDark);
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
    
    // 2. Основной фон кнобы (латунный градиент)
    juce::ColourGradient brassGradient (
        brassLight,
        centreX - radius * 0.3f, centreY - radius * 0.3f,
        brassDark,
        centreX + radius * 0.3f, centreY + radius * 0.3f,
        true
    );
    g.setGradientFill (brassGradient);
    g.fillEllipse (centreX - radius, centreY - radius, radius * 2, radius * 2);
    
    // 3. Контур кнобы
    g.setColour (brassDark);
    g.drawEllipse (centreX - radius, centreY - radius, radius * 2, radius * 2, 1.5f);
    
    // 4. Вращающиеся выступы
    int numRidges = 36;
    float ridgeRadius = radius - 3.0f;
    float ridgeSize = 3.5f;
    
    for (int i = 0; i < numRidges; ++i)
    {
        float ridgeAngle = (float) i / (float) numRidges * juce::MathConstants<float>::twoPi;
        float rotatedRidgeAngle = ridgeAngle + angle;
        
        float ridgeX = centreX + ridgeRadius * std::cos (rotatedRidgeAngle);
        float ridgeY = centreY + ridgeRadius * std::sin (rotatedRidgeAngle);
        
        // Выступы с градиентом
        juce::ColourGradient ridgeGradient (
            brassLight,
            ridgeX - 2, ridgeY - 2,
            brassDark,
            ridgeX + 2, ridgeY + 2,
            true
        );
        g.setGradientFill (ridgeGradient);
        g.fillEllipse (ridgeX - ridgeSize * 0.5f, ridgeY - ridgeSize * 0.5f, ridgeSize, ridgeSize);
        g.setColour (brassDark);
        g.drawEllipse (ridgeX - ridgeSize * 0.5f, ridgeY - ridgeSize * 0.5f, ridgeSize, ridgeSize, 0.5f);
    }
    
    // 5. Внутренний диск
    float innerRadius = radius - 8.0f;
    juce::ColourGradient innerGradient (
        brassDark,
        centreX - innerRadius * 0.3f, centreY - innerRadius * 0.3f,
        brassMid,
        centreX + innerRadius * 0.3f, centreY + innerRadius * 0.3f,
        true
    );
    g.setGradientFill (innerGradient);
    g.fillEllipse (centreX - innerRadius, centreY - innerRadius, innerRadius * 2, innerRadius * 2);
    g.setColour (brassLight);
    g.drawEllipse (centreX - innerRadius, centreY - innerRadius, innerRadius * 2, innerRadius * 2, 1.0f);
    
    // 6. Треугольная стрелка
    juce::Path pointer;
    float pointerLength = innerRadius * 0.6f;
    float pointerWidth = innerRadius * 0.12f;
    
    pointer.addTriangle (0.0f, -innerRadius + 6.0f,
                        -pointerWidth * 0.5f, -innerRadius + pointerLength,
                        pointerWidth * 0.5f, -innerRadius + pointerLength);
    
    juce::AffineTransform transform = juce::AffineTransform::rotation (angle);
    pointer.applyTransform (transform);
    pointer.applyTransform (juce::AffineTransform::translation (centreX, centreY));
    
    g.setColour (brassHighlight);
    g.fillPath (pointer);
    
    // 7. Центральная точка
    g.setColour (brassDark);
    g.fillEllipse (centreX - 4.0f, centreY - 4.0f, 8.0f, 8.0f);
    g.setColour (brassHighlight);
    g.fillEllipse (centreX - 2.0f, centreY - 2.0f, 4.0f, 4.0f);
}