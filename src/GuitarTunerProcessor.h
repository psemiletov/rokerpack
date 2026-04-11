#pragma once

#include <JuceHeader.h>
#include "PitchDetector.h"
#include "GateDetector.h"

class GuitarTunerAudioProcessor : public juce::AudioProcessor
{
public:
    GuitarTunerAudioProcessor();
    ~GuitarTunerAudioProcessor() override;

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
    juce::String getDetectedNote() const { juce::ScopedLock lock (stringDataLock); return detectedNote; }
    juce::String getTargetNote() const { juce::ScopedLock lock (stringDataLock); return targetNote; }
    int getStringNumber() const { return stringNumber; }
    float getCentsDeviation() const { return centsDeviation.load(); }
    bool isSignalActive() const { return signalActive.load(); }

private:
   
   GateDetector gateDetector;
   std::atomic<bool> signalActive;
   
    int findClosestString (float frequency) const;
    float calculateCents (float detectedFreq, float targetFreq) const;
    juce::String frequencyToNoteName (float frequency) const;

    std::unique_ptr<PitchDetector> pitchDetector;
    
    double currentSampleRate;
    int currentBlockSize;
    
    std::atomic<float> detectedFrequency;
    std::atomic<float> targetFrequency;
    std::atomic<float> centsDeviation;
    
    juce::String detectedNote;
    juce::String targetNote;
    int stringNumber;
    
    mutable juce::CriticalSection stringDataLock;
    
    struct StringInfo
    {
        float frequency;
        const char* noteName;
    };
    
    static constexpr StringInfo STRINGS[6] = {
        { 82.41f,  "E2" },
        { 110.00f, "A2" },
        { 146.83f, "D3" },
        { 196.00f, "G3" },
        { 246.94f, "B3" },
        { 329.63f, "E4" }
    };
};