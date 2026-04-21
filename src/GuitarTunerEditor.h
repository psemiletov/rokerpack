#pragma once
#include <JuceHeader.h>
#include "GuitarTunerProcessor.h"
#include "StringsPanel.h"
#include "GuitarMeterPanel.h"
#include "BronzaLookAndFeel.h"

class GuitarTunerAudioEditor : public juce::AudioProcessorEditor,
                               private juce::Timer,
                               private juce::Button::Listener
{
public:
    GuitarTunerAudioEditor (GuitarTunerAudioProcessor&);
    ~GuitarTunerAudioEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void updateUIFromProcessor();
    void buttonClicked (juce::Button* button) override;
    
    GuitarTunerAudioProcessor& audioProcessor;
    
    GuitarMeterPanel meterPanel;
    StringsPanel stringsPanel;
    BronzaLookAndFeel bronzaLookAndFeel;
    
    juce::TextButton helpButton;
    
    bool isUpdatingUI;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarTunerAudioEditor)
};

static constexpr int DEFAULT_WIDTH = 720;
static constexpr int DEFAULT_HEIGHT = 480;