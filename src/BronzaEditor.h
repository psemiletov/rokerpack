#pragma once

#include <JuceHeader.h>
#include "BronzaProcessor.h"
#include "BronzaLookAndFeel.h"

class BronzaAudioEditor : public juce::AudioProcessorEditor, private juce::Slider::Listener
{
public:
    BronzaAudioEditor (BronzaAudioProcessor&);
    ~BronzaAudioEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void sliderValueChanged (juce::Slider* slider) override;

    BronzaAudioProcessor& audioProcessor;
    
    juce::Slider levelSlider;
    juce::Slider intensitySlider;
    
    juce::Label levelLabel;
    juce::Label intensityLabel;
    
    BronzaLookAndFeel bronzaLookAndFeel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BronzaAudioEditor)
};