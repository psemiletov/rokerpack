#include "StringsPanel.h"

StringsPanel::StringsPanel()
    : manualMode (false)
    , selectedString (-1)
    , activeString (-1)
{
    strings = {
        { "E2", 82.41f, 10, {}, false },
        { "A2", 110.00f, 8, {}, false },
        { "D3", 146.83f, 6, {}, false },
        { "G3", 196.00f, 5, {}, false },
        { "B3", 246.94f, 4, {}, false },
        { "E4", 329.63f, 3, {}, false }
    };
    
    // Настройка кнопки Auto
    autoButton.setButtonText ("AUTO");
    autoButton.setClickingTogglesState (true);
    autoButton.setToggleState (true, juce::dontSendNotification);
    autoButton.onClick = [this]()
    {
        if (autoButton.getToggleState())
        {
            // Включаем автоматический режим
            if (onStringSelectionChanged)
                onStringSelectionChanged (false, -1);
        }
    };
    addAndMakeVisible (autoButton);
}

StringsPanel::~StringsPanel()
{
}

void StringsPanel::setActiveString (int stringIndex, bool isManualMode)
{
    for (auto& s : strings)
        s.ledActive = false;
    
    if (isManualMode && selectedString >= 0)
    {
        if (stringIndex == selectedString)
            strings[selectedString].ledActive = true;
    }
    else if (!isManualMode && stringIndex >= 0 && stringIndex < 6)
    {
        strings[stringIndex].ledActive = true;
    }
    
    activeString = stringIndex;
    repaint();
}

void StringsPanel::setManualMode (bool manual, int selectedIndex)
{
    manualMode = manual;
    autoButton.setToggleState (!manual, juce::dontSendNotification);
    
    if (manual && selectedIndex >= 0 && selectedIndex < 6)
        selectedString = selectedIndex;
    else if (!manual)
        selectedString = -1;
    
    for (auto& s : strings)
        s.ledActive = false;
    
    repaint();
}

void StringsPanel::resetLEDs()
{
    for (auto& s : strings)
        s.ledActive = false;
    repaint();
}

void StringsPanel::mouseDown (const juce::MouseEvent& event)
{
    int index = getStringIndexAt (event.getPosition());
    if (index >= 0)
        selectString (index);
}

void StringsPanel::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // Вычитаем область кнопки из bounds для строк
    auto buttonArea = bounds.removeFromBottom (40);
    
    // Фон панели
    g.setColour (Colors::bgDark.withAlpha (0.7f));
    g.fillRect (bounds);
    
    // Рисуем каждую струну
    for (int i = 0; i < strings.size(); ++i)
    {
        auto& str = strings[i];
        auto rowBounds = str.bounds;
        
        if (manualMode && i == selectedString)
        {
            g.setColour (Colors::brassLight);
            g.drawRect (rowBounds, 2.0f);
        }
        
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
    
    // Рисуем кнопку
    autoButton.setBounds (buttonArea.reduced (buttonArea.getWidth() / 2 - 40, 5));
}

void StringsPanel::resized()
{
    auto bounds = getLocalBounds();
    auto buttonArea = bounds.removeFromBottom (40);
    auto stringsBounds = bounds;
    
    int numStrings = static_cast<int> (strings.size());
    int rowHeightCalculated = stringsBounds.getHeight() / numStrings;
    
    for (int i = 0; i < numStrings; ++i)
    {
        juce::Rectangle<float> rowBounds (
            0.0f,
            static_cast<float> (i * rowHeightCalculated),
            static_cast<float> (stringsBounds.getWidth()),
            static_cast<float> (rowHeightCalculated)
        );
        strings[i].bounds = rowBounds;
    }
    
    autoButton.setBounds (buttonArea.reduced (buttonArea.getWidth() / 2 - 40, 5));
}

int StringsPanel::getStringIndexAt (juce::Point<int> position)
{
    for (int i = 0; i < strings.size(); ++i)
    {
        if (strings[i].bounds.contains (position.toFloat()))
            return i;
    }
    return -1;
}

void StringsPanel::selectString (int index)
{
    if (index < 0 || index >= 6)
        return;
    
    if (manualMode && selectedString == index)
    {
        if (onStringSelectionChanged)
            onStringSelectionChanged (false, -1);
    }
    else
    {
        if (onStringSelectionChanged)
            onStringSelectionChanged (true, index);
    }
}