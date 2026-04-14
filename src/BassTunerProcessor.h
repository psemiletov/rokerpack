#pragma once

#include <JuceHeader.h>
#include "PitchDetector.h"

// Константа для количества струн (согласована с BassStringsPanel)

class BassTunerAudioProcessor : public juce::AudioProcessor
{
public:
    BassTunerAudioProcessor();
    ~BassTunerAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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
    float getDetectedFrequency() const { return detectedFrequency.load(); }
    float getTargetFrequency() const { return targetFrequency.load(); }
    float getCentsDeviation() const { return centsDeviation.load(); }
    
    juce::String getDetectedNote() const { juce::ScopedLock lock (stringDataLock); return detectedNote; }
    juce::String getTargetNote() const { juce::ScopedLock lock (stringDataLock); return targetNote; }
    int getStringNumber() const { return stringNumber.load(); }

private:
    int findClosestString (float frequency) const;
    float calculateCents (float detectedFreq, float targetFreq) const;
    juce::String frequencyToNoteName (float frequency) const;

    std::unique_ptr<PitchDetector> pitchDetector;
    
    double currentSampleRate;
    
    std::atomic<float> detectedFrequency;
    std::atomic<float> targetFrequency;
    std::atomic<float> centsDeviation;
    std::atomic<int> stringNumber;  // ← теперь atomic
    
    juce::String detectedNote;
    juce::String targetNote;
    
    mutable juce::CriticalSection stringDataLock;
    
    struct StringInfo
    {
        float frequency;
        const char* noteName;
    };
    
    // 4 струны бас-гитары (E1, A1, D2, G2)
    static constexpr StringInfo STRINGS[4] = {
        { 41.20f,  "E1" },   // 4-я (самая толстая)
        { 55.00f,  "A1" },   // 3-я
        { 73.42f,  "D2" },   // 2-я
        { 98.00f,  "G2" }    // 1-я (самая тонкая)
    };
};