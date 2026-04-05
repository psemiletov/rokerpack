#pragma once

#include <JuceHeader.h>
#include "GrelkaProcessor.h"
#include "BronzaLookAndFeel.h"

class GrelkaAudioEditor : public juce::AudioProcessorEditor
{
public:
    GrelkaAudioEditor (GrelkaAudioProcessor&);
    ~GrelkaAudioEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    GrelkaAudioProcessor& audioProcessor;
    
    juce::Slider driveSlider;
    juce::Slider levelSlider;
    juce::Slider lowsSlider;
    juce::Slider trebleSlider;
    
    juce::Label driveLabel;
    juce::Label levelLabel;
    juce::Label lowsLabel;
    juce::Label trebleLabel;
    
    // Attachments для связи слайдеров с параметрами
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> levelAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowsAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> trebleAttachment;
    
    BronzaLookAndFeel grelkaLookAndFeel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GrelkaAudioEditor)
};