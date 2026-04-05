#pragma once

#include <JuceHeader.h>
#include "dsp.h"
#include "fx-resofilter.h"

class BronzaAudioProcessor : public juce::AudioProcessor
{
public:
    BronzaAudioProcessor();
    ~BronzaAudioProcessor() override;

    // Основные аудио-методы
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
    
    // Добавляем перегрузку для double (чтобы убрать предупреждение)
    void processBlock (juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages) override;

    // GUI
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    // Метаданные плагина
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    double getTailLengthSeconds() const override;

    // Программы
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    // State management
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Геттеры для GUI
    juce::AudioParameterFloat* getLevelParam() const { return levelParam; }
    juce::AudioParameterFloat* getIntensityParam() const { return intensityParam; }

private:
    // Аудио параметры
    juce::AudioParameterFloat* levelParam;
    juce::AudioParameterFloat* intensityParam;

    // Добавленные переменные-члены (были только в .cpp)
    float fLevel = 29.0f / 48.0f;    // Значение по умолчанию 0.604
    float fIntensity = 0.87f;         // Значение по умолчанию
    float sampleRate = 44100.0f;
    bool dBTableInitialized = false;

    // Исправление: обычные объекты, а не unique_ptr
    CResoFilter lp[2];  // Левый и правый Low-pass
    CResoFilter hp[2];  // Левый и правый High-pass

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BronzaAudioProcessor)
};