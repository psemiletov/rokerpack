#pragma once
#include <JuceHeader.h>
#include <vector>
#include <atomic>
#include <cmath>
#include <algorithm>

class BassPitchDetector
{
public:
    BassPitchDetector();
    ~BassPitchDetector();
    
    void prepare (double sampleRate, int blockSize);
    void reset();
    
    float processSamples (const float* buffer, int numSamples);
    float getLastFrequency() const { return lastFrequency.load(); }
    float getCurrentLevel() const { return currentLevel.load(); }
    void setSilenceThreshold (float threshold) { silenceThreshold = threshold; }

private:
    float detectPitch (const std::vector<float>& buffer);
    float parabolicInterpolation (const std::vector<float>& data, int index);
    int findClosestString (float frequency) const;
    
    std::vector<float> ringBuffer;
    std::vector<float> analysisBuffer;
    std::vector<float> nsdfBuffer;
    
    std::atomic<float> lastFrequency;
    std::atomic<float> currentLevel;
    
    double sampleRate;
    int blockSize;
    int writePosition;
    int minLag;
    int maxLag;
    float silenceThreshold;
    
    float minFreq;
    float maxFreq;
    
    int samplesSinceLastAnalysis;
    int analysisInterval;
    
    // Структура струн для баса
    struct StringInfo
    {
        float frequency;
        const char* noteName;
    };
    
    static constexpr int NUM_BASS_STRINGS = 4;
    static const StringInfo STRINGS[NUM_BASS_STRINGS];
    
    // --- Логика стабилизации ---
    float stableFrequency;
    int stableFrames;
    int silenceFrames;
    int newStringFrames;          // счётчик для новой струны
    const int requiredStableFrames = 4;
    const int maxSilenceFrames = 6;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BassPitchDetector)
};