#include "MeterPanel.h"

MeterPanel::MeterPanel()
    : currentFrequency (0.0f)
    , currentTargetFrequency (0.0f)
    , currentDetectedNote ("--")
    , currentTargetNote ("--")
    , currentStringNumber (-1)
    , currentCentsDeviation (0.0f)
    , smoothedCents (0.0f)
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
}

void MeterPanel::timerCallback()
{
    smoothedCents = smoothedCents + SMOOTHING_FACTOR * (currentCentsDeviation - smoothedCents);
    repaint();
}

void MeterPanel::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    g.setColour (Colors::bgDark.withAlpha (0.5f));
    g.fillRect (bounds);
    
    // Разделяем на три зоны
    auto topArea = bounds.removeFromTop (85);      // Верхняя зона: целевая информация
    auto middleArea = bounds.removeFromTop (120);  // Средняя зона: шкала
    auto bottomArea = bounds;                       // Нижняя зона: текущая частота
    
    // === 1. ВЕРХНЯЯ ЗОНА ===
    float yOffset = 5;
    
    // Строка "String X" (увеличенный шрифт)
    if (currentStringNumber > 0)
    {
        g.setFont (juce::Font (15.0f, juce::Font::bold));  // Увеличено с 11 до 15
        g.setColour (Colors::brassLight);
        g.drawText ("STRING " + juce::String (currentStringNumber), 
                    topArea.getX(), 
                    topArea.getY() + yOffset,
                    topArea.getWidth(),
                    20,
                    juce::Justification::centred);
        yOffset += 24;
    }
    
    // Название эталонной ноты
    g.setColour (Colors::brassHighlight);
    g.setFont (juce::Font (28.0f, juce::Font::bold));  // Увеличено с 26 до 28
    g.drawText (currentTargetNote, 
                topArea.getX(), 
                topArea.getY() + yOffset,
                topArea.getWidth(),
                38,
                juce::Justification::centred);
    yOffset += 42;
    
    // Частота эталонной ноты
    g.setColour (Colors::textLight);
    g.setFont (juce::Font (16.0f, juce::Font::bold));
    juce::String targetFreqText = juce::String (currentTargetFrequency, 1) + " Hz";
    g.drawText (targetFreqText, 
                topArea.getX(), 
                topArea.getY() + yOffset,
                topArea.getWidth(),
                22,
                juce::Justification::centred);
    
    // === 2. СРЕДНЯЯ ЗОНА: шкала и стрелка ===
    drawMeter (g, middleArea);
    
    // === 3. НИЖНЯЯ ЗОНА: текущая частота и нота ===
    yOffset = 10;
    
    // Текущая частота (крупно)
    g.setColour (Colors::brassHighlight);
    g.setFont (juce::Font (34.0f, juce::Font::bold));  // Увеличено с 32 до 34
    
    juce::String detectedText;
    if (currentFrequency <= 0.0f)
        detectedText = "---";
    else
        detectedText = juce::String (currentFrequency, 1);
    
    g.drawText (detectedText, 
                bottomArea.getX(), 
                bottomArea.getY() + yOffset,
                bottomArea.getWidth(),
                48,
                juce::Justification::centred);
    yOffset += 52;
    
    // Единица измерения "Hz"
    g.setColour (Colors::textLight);
    g.setFont (juce::Font (14.0f, juce::Font::bold));
    g.drawText ("Hz", 
                bottomArea.getX(), 
                bottomArea.getY() + yOffset,
                bottomArea.getWidth(),
                20,
                juce::Justification::centred);
    yOffset += 24;
    
    // Текущая детектированная нота
    g.setFont (juce::Font (18.0f, juce::Font::bold));
    g.setColour (Colors::brassMid);
    g.drawText (currentDetectedNote, 
                bottomArea.getX(), 
                bottomArea.getY() + yOffset,
                bottomArea.getWidth(),
                25,
                juce::Justification::centred);
}

void MeterPanel::drawMeter (juce::Graphics& g, juce::Rectangle<int> area)
{
    juce::Rectangle<int> meterBg = area.reduced (20, 10);
    g.setColour (Colors::meterBackground);
    g.fillRect (meterBg);
    
    g.setColour (Colors::brassDark);
    g.drawRect (meterBg, 1.5f);
    
    int numTicks = 11;
    float tickStep = (meterBg.getWidth() - 20) / (numTicks - 1);
    float startX = meterBg.getX() + 10;
    
    // Рисуем метки шкалы
    for (int i = 0; i < numTicks; ++i)
    {
        float x = startX + i * tickStep;
        int centsValue = -50 + i * 10;
        
        g.setColour (Colors::brassMid);
        g.drawLine (x, meterBg.getY() + meterBg.getHeight() - 15, 
                    x, meterBg.getY() + meterBg.getHeight() - 5, 1.0f);
        
        if (centsValue == -50 || centsValue == 0 || centsValue == 50)
        {
            g.setFont (juce::Font (10.0f));
            g.drawText (juce::String (centsValue), 
                        juce::Rectangle<int> (static_cast<int> (x) - 15, 
                                             meterBg.getY() + meterBg.getHeight() - 20, 
                                             30, 15), 
                        juce::Justification::centred);
        }
    }
    
    // === Подсказки по краям шкалы ===
    g.setFont (juce::Font (10.0f, juce::Font::bold));
    
    // Стрелка влево (затягивать)
    g.setColour (Colors::brassHighlight);
    juce::Path leftArrow;
    leftArrow.addArrow (juce::Line<float> (meterBg.getX() + 15, meterBg.getY() + 12, 
                                           meterBg.getX() + 5, meterBg.getY() + 12), 3.0f, 6.0f, 4.0f);
    g.fillPath (leftArrow);
    
    // Метка "TIGHTEN" справа от стрелки (на расстоянии 10 пикселей)
    g.setColour (Colors::brassLight);
    g.drawText ("TIGHTEN", 
                meterBg.getX() + 25, 
                meterBg.getY() + 5,
                70,
                15,
                juce::Justification::centred);
    
    // Стрелка вправо (ослаблять)
    juce::Path rightArrow;
    rightArrow.addArrow (juce::Line<float> (meterBg.getRight() - 15, meterBg.getY() + 12, 
                                            meterBg.getRight() - 5, meterBg.getY() + 12), 3.0f, 6.0f, 4.0f);
    g.fillPath (rightArrow);
    
    // Метка "LOOSEN" слева от стрелки (на расстоянии 10 пикселей)
    g.drawText ("LOOSEN", 
                meterBg.getRight() - 85, 
                meterBg.getY() + 5,
                70,
                15,
                juce::Justification::centred);
    
    // Центральная отметка
    float centerX = startX + 5 * tickStep;
    g.setColour (Colors::brassHighlight);
    g.drawLine (centerX, meterBg.getY() + meterBg.getHeight() - 20,
                centerX, meterBg.getY() + meterBg.getHeight() - 3, 2.0f);
    
    drawNeedle (g, meterBg, smoothedCents);
}


void MeterPanel::drawNeedle (juce::Graphics& g, juce::Rectangle<int> meterArea, float cents)
{
    float clampedCents = juce::jlimit (-MAX_CENTS, MAX_CENTS, cents);
    float normalized = (clampedCents + MAX_CENTS) / (2.0f * MAX_CENTS);
    float needleX = meterArea.getX() + 10 + normalized * (meterArea.getWidth() - 20);
    float needleTop = meterArea.getY() + 10;
    float needleBottom = meterArea.getY() + meterArea.getHeight() - 10;
    
    juce::Path needle;
    needle.addRectangle (needleX - 3.0f, needleTop, 6.0f, needleBottom - needleTop);
    
    juce::ColourGradient needleGradient (
        Colors::brassHighlight,
        needleX, needleTop,
        Colors::brassDark,
        needleX, needleBottom,
        false
    );
    g.setGradientFill (needleGradient);
    g.fillPath (needle);
    
    juce::Path pointer;
    pointer.addTriangle (needleX - 5.0f, needleBottom,
                         needleX + 5.0f, needleBottom,
                         needleX, needleBottom + 8.0f);
    g.setColour (Colors::brassHighlight);
    g.fillPath (pointer);
    
    g.setColour (Colors::brassDark);
    g.fillEllipse (needleX - 5.0f, needleTop - 5.0f, 10.0f, 10.0f);
    g.setColour (Colors::brassLight);
    g.fillEllipse (needleX - 3.0f, needleTop - 3.0f, 6.0f, 6.0f);
    
    if (std::abs (clampedCents) < 2.0f)
    {
        g.setColour (Colors::brassHighlight.withAlpha (0.5f));
        g.fillEllipse (needleX - 12.0f, needleTop - 12.0f, 24.0f, 24.0f);
    }
}

void MeterPanel::resized()
{
    // Нет дочерних компонентов
}