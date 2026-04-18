#include "BassStringsPanel.h"
#include "Colors.h"  // ← добавить эту строку

BassStringsPanel::BassStringsPanel()
    : activeString (-1)
{
    strings = {
        { "E1", 41.20f, 12, {}, false },
        { "A1", 55.00f, 10, {}, false },
        { "D2", 73.42f, 8, {}, false },
        { "G2", 98.00f, 6, {}, false }
    };
}

BassStringsPanel::~BassStringsPanel()
{
}

void BassStringsPanel::setActiveString (int stringIndex)
{
    for (auto& s : strings)
        s.ledActive = false;
    
    if (stringIndex >= 0 && stringIndex < 4)
        strings[stringIndex].ledActive = true;
    
    activeString = stringIndex;
    repaint();
}

void BassStringsPanel::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    g.setColour (Colors::bgDark.withAlpha (0.7f));
    g.fillRect (bounds);
    
    for (int i = 0; i < 4; ++i)
    {
        auto& str = strings[i];
        auto rowBounds = str.bounds;
        
        juce::String labelText = str.name + " " + juce::String (str.frequency, 2) + " Hz";
        g.setColour (Colors::textLight);
        g.setFont (juce::Font (14.0f, juce::Font::bold));
        g.drawText (labelText,
                    rowBounds.getX() + textOffset,
                    rowBounds.getY(),
                    leftLabelWidth - textOffset,
                    rowBounds.getHeight(),
                    juce::Justification::centredLeft);
        
        juce::Rectangle<float> stringRect;
        stringRect.setX (rowBounds.getX() + leftLabelWidth);
        stringRect.setY (rowBounds.getCentreY() - str.thickness * 0.5f);
        stringRect.setWidth (rowBounds.getWidth() - leftLabelWidth - rightLEDSize - textOffset * 2);
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

void BassStringsPanel::resized()
{
    updateStringData();
}

void BassStringsPanel::updateStringData()
{
    auto bounds = getLocalBounds();
    int rowHeightCalculated = bounds.getHeight() / 4;
    
    for (int i = 0; i < 4; ++i)
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