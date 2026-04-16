#pragma once

#include <JuceHeader.h>
#include "dsp.h"
#include "VintageEQ.h"  // Новый файл

class TembrAudioProcessor : public juce::AudioProcessor
{
public:
    TembrAudioProcessor();
    ~TembrAudioProcessor() override;

    void prepareToPlay (double sRate, int samplesPerBlock) override;
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
    float sampleRate = 44100.0f;
    bool dBTableInitialized = false;

    // Винтажный трёхполосный эквалайзер
    VintageEQ eqL;
    VintageEQ eqR;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TembrAudioProcessor)
};