#pragma once
#include <JuceHeader.h>
#include "GuitarTunerProcessor.h"
#include "StringsPanel.h"
#include "MeterPanel.h"
#include "BronzaLookAndFeel.h"

//==============================================================================
/**
 * Редактор плагина Guitar Tuner.
 * Содержит два основных компонента:
 * - MeterPanel (слева) — индикатор, метки, порог
 * - StringsPanel (справа) — 6 струн с LED и ручным выбором
 */
class GuitarTunerAudioEditor : public juce::AudioProcessorEditor,
                               private juce::Timer
{
public:
    GuitarTunerAudioEditor (GuitarTunerAudioProcessor&);
    ~GuitarTunerAudioEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    void timerCallback() override;
    void updateUIFromProcessor();
    
    // Обработчик событий от панели струн
    void onStringSelectionChanged (bool manualMode, int stringIndex);
    
    //==============================================================================
    GuitarTunerAudioProcessor& audioProcessor;
    
    // UI компоненты
    MeterPanel meterPanel;
    StringsPanel stringsPanel;
    
    // LookAndFeel
    BronzaLookAndFeel bronzaLookAndFeel;
    
    // Для предотвращения рекурсивных вызовов
    bool isUpdatingUI;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarTunerAudioEditor)
};