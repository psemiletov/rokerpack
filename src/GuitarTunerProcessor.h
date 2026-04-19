#pragma once

#include <JuceHeader.h>
#include "GuitarPitchDetector.h"  // ← изменить
#include "GateDetector.h"

// Константа для количества струн
constexpr int NUM_GUITAR_STRINGS = 6;

class GuitarTunerAudioProcessor : public juce::AudioProcessor
{
public:
    GuitarTunerAudioProcessor();
    ~GuitarTunerAudioProcessor() override;

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
    juce::String getDetectedNote() const { return pitchDetector->getCurrentNoteName(); }
    juce::String getTargetNote() const { juce::ScopedLock lock (stringDataLock); return targetNote; }
    int getStringNumber() const { return stringNumber.load(); }
    float getCentsDeviation() const { return centsDeviation.load(); }
    bool isSignalActive() const { return signalActive.load(); }

private:
    int findClosestString (float frequency) const;
    float calculateCents (float detectedFreq, float targetFreq) const;
    juce::String frequencyToNoteName (float frequency) const;

    std::unique_ptr<GuitarPitchDetector> pitchDetector;  // ← изменено
    GateDetector gateDetector;
    
    double currentSampleRate = 44100.0;
    
    std::atomic<float> targetFrequency;
    std::atomic<float> centsDeviation;
    std::atomic<int> stringNumber;
    std::atomic<bool> signalActive;
    
    juce::String targetNote;
    
    mutable juce::CriticalSection stringDataLock;
    
    struct StringInfo
    {
        float frequency;
        const char* noteName;
    };
    
    static constexpr StringInfo STRINGS[NUM_GUITAR_STRINGS] = {
        { 82.41f,  "E2" },   // 6-я (самая толстая)
        { 110.00f, "A2" },   // 5-я
        { 146.83f, "D3" },   // 4-я
        { 196.00f, "G3" },   // 3-я
        { 246.94f, "B3" },   // 2-я
        { 329.63f, "E4" }    // 1-я (самая тонкая)
    };
};