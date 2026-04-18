#pragma once

#include <JuceHeader.h>
#include "SmartPitchDetector.h"

// Константа для количества струн
constexpr int NUM_BASS_STRINGS = 4;

class BassTunerAudioProcessor : public juce::AudioProcessor
{
public:
    BassTunerAudioProcessor();
    ~BassTunerAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void processBlock (juce::AudioBuffer<double>&, juce::MidiBuffer&) override;

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

    // Геттеры для UI
    float getDetectedFrequency() const { return pitchDetector->getCurrentFrequency(); }
    float getTargetFrequency() const { return targetFrequency.load(); }
    float getCentsDeviation() const { return centsDeviation.load(); }
    juce::String getDetectedNote() const { return pitchDetector->getCurrentNoteName(); }
    juce::String getTargetNote() const { juce::ScopedLock lock (stringDataLock); return targetNote; }
    int getStringNumber() const { return stringNumber.load(); }

private:
    int findClosestString (float frequency) const;
    float calculateCents (float detectedFreq, float targetFreq) const;
    juce::String frequencyToNoteName (float frequency) const;

    std::unique_ptr<SmartPitchDetector> pitchDetector;
    
    double currentSampleRate = 44100.0;
    
    std::atomic<float> targetFrequency;
    std::atomic<float> centsDeviation;
    std::atomic<int> stringNumber;
    
    juce::String detectedNote;
    juce::String targetNote;
    
    mutable juce::CriticalSection stringDataLock;
    
    struct StringInfo
    {
        float frequency;
        const char* noteName;
    };
    
    static constexpr StringInfo STRINGS[NUM_BASS_STRINGS] = {
        { 41.20f,  "E1" },
        { 55.00f,  "A1" },
        { 73.42f,  "D2" },
        { 98.00f,  "G2" }
    };
};