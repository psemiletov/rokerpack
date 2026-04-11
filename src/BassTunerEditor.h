#pragma once
#include <JuceHeader.h>
#include "BassTunerProcessor.h"
#include "BassStringsPanel.h"
#include "MeterPanel.h"
#include "BronzaLookAndFeel.h"

class BassTunerAudioEditor : public juce::AudioProcessorEditor,
                             private juce::Timer
{
public:
    BassTunerAudioEditor (BassTunerAudioProcessor&);
    ~BassTunerAudioEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void updateUIFromProcessor();
    
    BassTunerAudioProcessor& audioProcessor;
    
    MeterPanel meterPanel;
    BassStringsPanel stringsPanel;
    BronzaLookAndFeel bronzaLookAndFeel;
    
    bool isUpdatingUI;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BassTunerAudioEditor)
};