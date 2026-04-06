#pragma once

#include <JuceHeader.h>
#include "TembrProcessor.h"
#include "BronzaLookAndFeel.h"

class TembrAudioEditor : public juce::AudioProcessorEditor
{
public:
    TembrAudioEditor (TembrAudioProcessor&);
    ~TembrAudioEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    TembrAudioProcessor& audioProcessor;
    
    juce::Slider lowsSlider;
    juce::Slider trebleSlider;
    
    juce::Label lowsLabel;
    juce::Label trebleLabel;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowsAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> trebleAttachment;
    
    BronzaLookAndFeel tembrLookAndFeel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TembrAudioEditor)
};