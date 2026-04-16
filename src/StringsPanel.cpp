#include "StringsPanel.h"

StringsPanel::StringsPanel()
    : activeString (-1)
{
    strings = {
        { "E2", 82.41f, 10, {}, false },
        { "A2", 110.00f, 8, {}, false },
        { "D3", 146.83f, 6, {}, false },
        { "G3", 196.00f, 5, {}, false },
        { "B3", 246.94f, 4, {}, false },
        { "E4", 329.63f, 3, {}, false }
    };
}

StringsPanel::~StringsPanel()
{
}

void StringsPanel::setActiveString (int stringIndex)
{
    for (auto& s : strings)
        s.ledActive = false;
    
    if (stringIndex >= 0 && stringIndex < 6)
        strings[stringIndex].ledActive = true;
    
    activeString = stringIndex;
    repaint();
}

void StringsPanel::resetLEDs()
{
    for (auto& s : strings)
        s.ledActive = false;
    repaint();
}

void StringsPanel::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    g.setColour (Colors::bgDark.withAlpha (0.7f));
    g.fillRect (bounds);
    
    for (int i = 0; i < strings.size(); ++i)
    {
        auto& str = strings[i];
        auto rowBounds = str.bounds;
        
        // Текстовая метка слева — увеличенная ширина
        juce::String labelText = str.name + " " + juce::String (str.frequency, 2) + " Hz";
        g.setColour (Colors::textLight);
        g.setFont (juce::Font (14.0f, juce::Font::bold));
        
        // Увеличиваем ширину области для текста на 32 пикселя
        int labelWidth = leftLabelWidth + 32;  // было leftLabelWidth, стало +32
        g.drawText (labelText,
                    rowBounds.getX() + textOffset,
                    rowBounds.getY(),
                    labelWidth - textOffset,
                    rowBounds.getHeight(),
                    juce::Justification::centredLeft);
        
        // Полоса струны — левая граница сдвинута вправо на увеличенную ширину метки
        juce::Rectangle<float> stringRect;
        float stringStartX = rowBounds.getX() + labelWidth + 8.0f;  // ← сдвиг на новую ширину
        stringRect.setX (stringStartX);
        stringRect.setY (rowBounds.getCentreY() - str.thickness * 0.5f);
        stringRect.setWidth (rowBounds.getWidth() - (stringStartX - rowBounds.getX()) - rightLEDSize - textOffset * 2);
        stringRect.setHeight (static_cast<float> (str.thickness));
        
        juce::ColourGradient stringGradient (
            Colors::brassMid,
            stringRect.getX(), stringRect.getY(),
            Colors::brassLight,
            stringRect.getRight(), stringRect.getY(),
            false
        );
        g.setGradientFill (stringGradient);
        g.fillRect (stringRect);
        
        g.setColour (Colors::brassDark);
        g.drawRect (stringRect, 0.5f);
        
        // LED-кружок справа
        juce::Rectangle<float> ledRect (
            rowBounds.getRight() - rightLEDSize - textOffset,
            rowBounds.getCentreY() - rightLEDSize * 0.5f,
            static_cast<float> (rightLEDSize),
            static_cast<float> (rightLEDSize)
        );
        
        if (str.ledActive)
        {
            g.setColour (Colors::ledRed);
            g.fillEllipse (ledRect);
            g.setColour (Colors::ledRed.brighter (0.5f));
            g.drawEllipse (ledRect, 1.0f);
        }
        else
        {
            g.setColour (Colors::ledOff);
            g.fillEllipse (ledRect);
            g.setColour (Colors::brassDark);
            g.drawEllipse (ledRect, 0.5f);
        }
    }
}

void StringsPanel::resized()
{
    updateStringData();
}

void StringsPanel::updateStringData()
{
    auto bounds = getLocalBounds();
    int numStrings = static_cast<int> (strings.size());
    int rowHeightCalculated = bounds.getHeight() / numStrings;
    
    for (int i = 0; i < numStrings; ++i)
    {
        juce::Rectangle<float> rowBounds (
            0.0f,
            static_cast<float> (i * rowHeightCalculated),
            static_cast<float> (bounds.getWidth()),
            static_cast<float> (rowHeightCalculated)
        );
        strings[i].bounds = rowBounds;
    }
}