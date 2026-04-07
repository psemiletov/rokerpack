#pragma once

#include <JuceHeader.h>
#include "MistralProcessor.h"
#include "BronzaLookAndFeel.h"

class MistralAudioEditor : public juce::AudioProcessorEditor
{
public:
    MistralAudioEditor (MistralAudioProcessor&);
    ~MistralAudioEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    MistralAudioProcessor& audioProcessor;
    
    juce::Slider rateSlider;
    juce::Slider depthSlider;
    juce::Slider feedbackSlider;
    
    juce::Label rateLabel;
    juce::Label depthLabel;
    juce::Label feedbackLabel;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> rateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> depthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> feedbackAttachment;
    
    BronzaLookAndFeel mistralLookAndFeel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MistralAudioEditor)
};