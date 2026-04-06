#pragma once

#include <JuceHeader.h>
#include "CharmProcessor.h"
#include "BronzaLookAndFeel.h"

class CharmAudioEditor : public juce::AudioProcessorEditor
{
public:
    CharmAudioEditor (CharmAudioProcessor&);
    ~CharmAudioEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    CharmAudioProcessor& audioProcessor;
    
    juce::Slider charmSlider;
    juce::Label charmLabel;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> charmAttachment;
    
    BronzaLookAndFeel charmLookAndFeel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CharmAudioEditor)
};