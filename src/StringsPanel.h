#pragma once
#include <JuceHeader.h>
#include "Colors.h"

class StringsPanel : public juce::Component
{
public:
    StringsPanel();
    ~StringsPanel() override;

    void setActiveString (int stringIndex);
    void resetLEDs();

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
    
    std::vector<StringData> strings;
    int activeString;
    
    static constexpr int leftLabelWidth = 70;
    static constexpr int rightLEDSize = 16;
    static constexpr int rowHeight = 45;
    static constexpr int textOffset = 8;
};