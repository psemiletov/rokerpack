#pragma once
#include <JuceHeader.h>
#include "GuitarTunerProcessor.h"
#include "StringsPanel.h"
#include "MeterPanel.h"
#include "BronzaLookAndFeel.h"

class GuitarTunerAudioEditor : public juce::AudioProcessorEditor,
                               private juce::Timer
{
public:
    GuitarTunerAudioEditor (GuitarTunerAudioProcessor&);
    ~GuitarTunerAudioEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void updateUIFromProcessor();
    
    GuitarTunerAudioProcessor& audioProcessor;
    
    MeterPanel meterPanel;
    StringsPanel stringsPanel;
    BronzaLookAndFeel bronzaLookAndFeel;
    
    bool isUpdatingUI;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarTunerAudioEditor)
};

static constexpr int DEFAULT_WIDTH = 720;
static constexpr int DEFAULT_HEIGHT = 480;  // было 400