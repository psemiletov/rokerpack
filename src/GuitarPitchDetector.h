#pragma once
#include <JuceHeader.h>
#include <vector>
#include <atomic>
#include <cmath>

class GuitarPitchDetector
{
public:
    GuitarPitchDetector();
    ~GuitarPitchDetector();
    
    void prepare (double sampleRate, int samplesPerBlock);
    void reset();
    
    void processSamples (const float* buffer, int numSamples);
    
    float getCurrentFrequency() const { return currentFrequency.load(); }
    float getConfidence() const { return confidence.load(); }
    bool isNoteDetected() const { return noteDetected.load(); }
    juce::String getCurrentNoteName() const { return currentNoteName; }
    void clearNoteDetected() { noteDetected = false; }

private:
    float detectPitch (const std::vector<float>& buffer);
    float parabolicInterpolation (const std::vector<float>& data, int index);
    juce::String frequencyToNoteName (float frequency);
    
    // Буфер для записи ноты
    std::vector<float> noteBuffer;
    int noteBufferSize;
    int noteWritePosition;
    bool isRecording;
    int samplesToRecord;
    
    // Результаты
    std::atomic<float> currentFrequency;
    std::atomic<float> confidence;
    std::atomic<bool> noteDetected;
    juce::String currentNoteName;
    
    // Параметры
    double sampleRate;
    float currentEnergy;
    
    // Блокировка после детекции
    std::atomic<bool> noteLocked;
    int lockCounter;
    int lockDuration;
    
    // Константы для гитары
    static constexpr float MIN_FREQ = 80.0f;       // E2 (82.41 Hz)
    static constexpr float MAX_FREQ = 350.0f;      // E4 (329.63 Hz)
    static constexpr float MIN_CONFIDENCE = 0.5f;
    static constexpr float SILENCE_THRESHOLD = 0.0001f;
    static constexpr float RECORD_SECONDS = 0.5f;  // 500 мс для гитары
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarPitchDetector)
};