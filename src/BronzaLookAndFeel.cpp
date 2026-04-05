#include "BronzaLookAndFeel.h"

BronzaLookAndFeel::BronzaLookAndFeel()
{
    // Определяем цветовую палитру
    bodyColour = juce::Colour (0xFF2C2C2C);
    knobColour = juce::Colour (0xFF4A4A4A);
    knobOutlineColour = juce::Colour (0xFF1A1A1A);
    indicatorColour = juce::Colour (0xFFFF6B35);
    textColour = juce::Colour (0xFFE0E0E0);
    
    // Устанавливаем цвета
    setColour (juce::Slider::rotarySliderFillColourId, indicatorColour);
    setColour (juce::Slider::rotarySliderOutlineColourId, knobOutlineColour);
    setColour (juce::Label::textColourId, textColour);
    setColour (juce::ResizableWindow::backgroundColourId, bodyColour);
}

BronzaLookAndFeel::~BronzaLookAndFeel()
{
}
/*
void BronzaLookAndFeel::drawRotarySlider (juce::Graphics& g,
                                          int x, int y,
                                          int width, int height,
                                          float sliderPos,
                                          float rotaryStartAngle,
                                          float rotaryEndAngle,
                                          juce::Slider& slider)
{
    // Простейшая отрисовка для проверки
    auto radius = (float) juce::jmin (width / 2, height / 2) - 4.0f;
    auto centreX = (float) x + (float) width * 0.5f;
    auto centreY = (float) y + (float) height * 0.5f;
    auto rx = centreX - radius;
    auto ry = centreY - radius;
    auto rw = radius * 2.0f;
    
    // Вычисляем угол
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    
    // Рисуем фон
    g.setColour (knobColour);
    g.fillEllipse (rx, ry, rw, rw);
    
    // Рисуем контур
    g.setColour (knobOutlineColour);
    g.drawEllipse (rx, ry, rw, rw, 1.5f);
    
    // Рисуем стрелку (линия)
    juce::Path pointer;
    pointer.addLineSegment (juce::Line<float> (centreX, centreY,
                                              centreX + radius * 0.7f * std::cos (angle),
                                              centreY + radius * 0.7f * std::sin (angle)),
                           3.0f);
    g.setColour (indicatorColour);
    g.fillPath (pointer);
    
    // Рисуем центр
    g.setColour (knobOutlineColour);
    g.fillEllipse (centreX - 4.0f, centreY - 4.0f, 8.0f, 8.0f);
}
*/

/*
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
    
    // Рисуем фон
    g.setColour (juce::Colours::lightgrey);
    g.fillEllipse (centreX - radius, centreY - radius, radius * 2, radius * 2);
    
    // === ВРАЩАЮЩИЕСЯ ВЫСТУПЫ ===
    int numRidges = 36;
    float ridgeRadius = radius - 3.0f;
    float ridgeSize = 4.0f;
    
    for (int i = 0; i < numRidges; ++i)
    {
        // Базовый угол выступа (фиксированный)
        float ridgeAngle = (float) i / (float) numRidges * juce::MathConstants<float>::twoPi;
        
        // ДОБАВЛЯЕМ угол поворота ручки к позиции выступа
        float rotatedRidgeAngle = ridgeAngle + angle;
        
        float ridgeX = centreX + ridgeRadius * std::cos (rotatedRidgeAngle);
        float ridgeY = centreY + ridgeRadius * std::sin (rotatedRidgeAngle);
        
        g.setColour (juce::Colours::darkgrey);
        g.fillEllipse (ridgeX - ridgeSize * 0.5f, ridgeY - ridgeSize * 0.5f, ridgeSize, ridgeSize);
    }
    
    // Рисуем контур
    g.setColour (juce::Colours::black);
    g.drawEllipse (centreX - radius, centreY - radius, radius * 2, radius * 2, 2.0f);
    
    // Рисуем стрелку
    juce::Path pointer;
    pointer.addLineSegment (juce::Line<float> (centreX, centreY,
                                              centreX + radius * 0.7f * std::cos (angle),
                                              centreY + radius * 0.7f * std::sin (angle)),
                           3.0f);
    g.setColour (juce::Colours::red);
    g.fillPath (pointer);
    
    // Центр
    g.setColour (juce::Colours::black);
    g.fillEllipse (centreX - 4.0f, centreY - 4.0f, 8.0f, 8.0f);
}
*/

/*


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
    
    // 1. Фон
    g.setColour (juce::Colours::lightgrey);
    g.fillEllipse (centreX - radius, centreY - radius, radius * 2, radius * 2);
    
    // 2. Выступы (вращающиеся)
    int numRidges = 36;
    float ridgeRadius = radius - 3.0f;
    float ridgeSize = 4.0f;
    
    for (int i = 0; i < numRidges; ++i)
    {
        float ridgeAngle = (float) i / (float) numRidges * juce::MathConstants<float>::twoPi;
        float rotatedRidgeAngle = ridgeAngle + angle;
        
        float ridgeX = centreX + ridgeRadius * std::cos (rotatedRidgeAngle);
        float ridgeY = centreY + ridgeRadius * std::sin (rotatedRidgeAngle);
        
        g.setColour (juce::Colours::darkgrey);
        g.fillEllipse (ridgeX - ridgeSize * 0.5f, ridgeY - ridgeSize * 0.5f, ridgeSize, ridgeSize);
    }
    
    // 3. Контур
    g.setColour (juce::Colours::black);
    g.drawEllipse (centreX - radius, centreY - radius, radius * 2, radius * 2, 2.0f);
    
    // 4. СТРЕЛКА (поверх выступов)
    juce::Path pointer;
    pointer.addLineSegment (juce::Line<float> (centreX, centreY,
                                              centreX + radius * 0.8f * std::cos (angle),
                                              centreY + radius * 0.8f * std::sin (angle)),
                           4.0f);
    g.setColour (juce::Colours::red);
    g.fillPath (pointer);
    
    // 5. Центр (поверх стрелки)
    g.setColour (juce::Colours::black);
    g.fillEllipse (centreX - 5.0f, centreY - 5.0f, 10.0f, 10.0f);
    g.setColour (juce::Colours::lightgrey);
    g.fillEllipse (centreX - 3.0f, centreY - 3.0f, 6.0f, 6.0f);
}
*/

/*
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
    g.setColour (juce::Colours::black.withAlpha (0.3f));
    g.fillEllipse (centreX - radius + 2, centreY - radius + 2, radius * 2, radius * 2);
    
    // 2. Основной фон
    g.setColour (juce::Colours::lightgrey);
    g.fillEllipse (centreX - radius, centreY - radius, radius * 2, radius * 2);
    g.setColour (juce::Colours::black);
    g.drawEllipse (centreX - radius, centreY - radius, radius * 2, radius * 2, 1.5f);
    
    // 3. Выступы
    int numRidges = 36;
    float ridgeRadius = radius - 3.0f;
    float ridgeSize = 3.5f;
    
    for (int i = 0; i < numRidges; ++i)
    {
        float ridgeAngle = (float) i / (float) numRidges * juce::MathConstants<float>::twoPi;
        float rotatedRidgeAngle = ridgeAngle + angle;
        
        float ridgeX = centreX + ridgeRadius * std::cos (rotatedRidgeAngle);
        float ridgeY = centreY + ridgeRadius * std::sin (rotatedRidgeAngle);
        
        g.setColour (juce::Colours::grey);
        g.fillEllipse (ridgeX - ridgeSize * 0.5f, ridgeY - ridgeSize * 0.5f, ridgeSize, ridgeSize);
    }
    
    // 4. Внутренний диск
    float innerRadius = radius - 8.0f;
    g.setColour (juce::Colours::dimgrey);
    g.fillEllipse (centreX - innerRadius, centreY - innerRadius, innerRadius * 2, innerRadius * 2);
    g.setColour (juce::Colours::black);
    g.drawEllipse (centreX - innerRadius, centreY - innerRadius, innerRadius * 2, innerRadius * 2, 1.0f);
    
    // 5. Треугольная стрелка (исправленная версия без translate)
    juce::Path pointer;
    float pointerLength = innerRadius * 0.6f;
    float pointerWidth = innerRadius * 0.12f;
    
    // Создаём треугольник с центром в (0,0)
    pointer.addTriangle (0.0f, -innerRadius + 6.0f,
                        -pointerWidth * 0.5f, -innerRadius + pointerLength,
                        pointerWidth * 0.5f, -innerRadius + pointerLength);
    
    // Поворачиваем
    juce::AffineTransform transform = juce::AffineTransform::rotation (angle);
    pointer.applyTransform (transform);
    
    // Перемещаем в нужную позицию
    pointer.applyTransform (juce::AffineTransform::translation (centreX, centreY));
    
    g.setColour (juce::Colours::orangered);
    g.fillPath (pointer);
    
    // 6. Центральная точка
    g.setColour (juce::Colours::black);
    g.fillEllipse (centreX - 4.0f, centreY - 4.0f, 8.0f, 8.0f);
    g.setColour (juce::Colours::lightgrey);
    g.fillEllipse (centreX - 2.0f, centreY - 2.0f, 4.0f, 4.0f);
}
*/

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
    g.setColour (juce::Colours::black.withAlpha (0.3f));
    g.fillEllipse (centreX - radius + 2, centreY - radius + 2, radius * 2, radius * 2);
    
    // 2. Основной фон
    g.setColour (juce::Colours::lightgrey);
    g.fillEllipse (centreX - radius, centreY - radius, radius * 2, radius * 2);
    g.setColour (juce::Colours::black);
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
        
        g.setColour (juce::Colours::grey);
        g.fillEllipse (ridgeX - ridgeSize * 0.5f, ridgeY - ridgeSize * 0.5f, ridgeSize, ridgeSize);
    }
    
    // 4. Внутренний диск
    float innerRadius = radius - 8.0f;
    g.setColour (juce::Colours::dimgrey);
    g.fillEllipse (centreX - innerRadius, centreY - innerRadius, innerRadius * 2, innerRadius * 2);
    g.setColour (juce::Colours::black);
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
    
    g.setColour (juce::Colours::orangered);
    g.fillPath (pointer);
    
    // 6. Центральная точка
    g.setColour (juce::Colours::black);
    g.fillEllipse (centreX - 4.0f, centreY - 4.0f, 8.0f, 8.0f);
    g.setColour (juce::Colours::lightgrey);
    g.fillEllipse (centreX - 2.0f, centreY - 2.0f, 4.0f, 4.0f);
}