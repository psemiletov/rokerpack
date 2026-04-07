#pragma once

#include <JuceHeader.h>

class MistralAudioProcessor : public juce::AudioProcessor
{
public:
    MistralAudioProcessor();
    ~MistralAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
    void processBlock (juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

private:
    double sampleRate = 44100.0;
    float phase = 0.0f;
    
    int maxDelaySamples = 2205;
    juce::AudioBuffer<float> delayBuffer;
    int writePosition = 0;
    
    // Low-Pass фильтр для обратной связи
    float fbLpfState = 0.0f;
    float fbLpfCoeff = 0.0f;
    void updateFbLpfCoeff();
    float processFbLpf(float input);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MistralAudioProcessor)
};