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
    
    static constexpr int DEFAULT_WIDTH = 540;
    static constexpr int DEFAULT_HEIGHT = 370;

private:
    MetallugaAudioProcessor& audioProcessor;
    
    juce::Slider gateSlider;
    juce::Slider driveSlider;
    juce::Slider levelSlider;
    
    juce::Label gateLabel;
    juce::Label driveLabel;
    juce::Label levelLabel;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> levelAttachment;
    
    BronzaLookAndFeel metallugaLookAndFeel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MetallugaAudioEditor)
};