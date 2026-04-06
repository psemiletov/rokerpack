#pragma once

#include <JuceHeader.h>
#include "MetallugaProcessor.h"
#include "BronzaLookAndFeel.h"

class MetallugaAudioEditor : public juce::AudioProcessorEditor
{
public:
    MetallugaAudioEditor (MetallugaAudioProcessor&);
    ~MetallugaAudioEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    MetallugaAudioProcessor& audioProcessor;
    
    juce::Slider driveSlider;
    juce::Slider levelSlider;
    juce::Slider weightSlider;
    juce::Slider resoSlider;
    juce::Slider warmthSlider;
    
    juce::Label driveLabel;
    juce::Label levelLabel;
    juce::Label weightLabel;
    juce::Label resoLabel;
    juce::Label warmthLabel;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> levelAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> weightAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> resoAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> warmthAttachment;
    
    BronzaLookAndFeel metallugaLookAndFeel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MetallugaAudioEditor)
};