#pragma once
#include <JuceHeader.h>

//constexpr int NUM_BASS_STRINGS = 4;

class BassStringsPanel : public juce::Component
{
public:
    BassStringsPanel();
    ~BassStringsPanel() override;

    void setActiveString (int stringIndex);
    
    void paint (juce::Graphics&) override;
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
    
    std::vector<StringData> strings;
    int activeString;
    
    const int textOffset = 8;
    const int leftLabelWidth = 80;
    const int rightLEDSize = 16;
    
    void updateStringData();
};