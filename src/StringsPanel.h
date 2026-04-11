#pragma once
#include <JuceHeader.h>
#include "Colors.h"

class StringsPanel : public juce::Component
{
public:
    StringsPanel();
    ~StringsPanel() override;

    void setActiveString (int stringIndex, bool isManualMode);
    void setManualMode (bool manual, int selectedIndex = -1);
    bool isManualMode() const { return manualMode; }
    int getSelectedString() const { return selectedString; }
    void resetLEDs();

    void mouseDown (const juce::MouseEvent& event) override;

    std::function<void (bool manual, int stringIndex)> onStringSelectionChanged;

protected:
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    struct StringData
    {
        juce::String name;
        float frequency;
        int thickness;
        juce::Rectangle<float> bounds;
        bool ledActive;
    };
    
    void updateStringData();
    int getStringIndexAt (juce::Point<int> position);
    void selectString (int index);
    
    std::vector<StringData> strings;
    bool manualMode;
    int selectedString;
    int activeString;
    
    // Кнопка Auto
    juce::TextButton autoButton;
    
    static constexpr int leftLabelWidth = 70;
    static constexpr int rightLEDSize = 16;
    static constexpr int rowHeight = 45;
    static constexpr int textOffset = 8;
};