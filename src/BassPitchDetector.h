#pragma once
#include <JuceHeader.h>
#include <vector>
#include <atomic>

class BassPitchDetector
{
public:
    BassPitchDetector();
    ~BassPitchDetector();
    
    void prepare (double sampleRate, int blockSize);
    void reset();
    
    float processSamples (const float* buffer, int numSamples);
    float getLastFrequency() const { return lastFrequency.load(); }
    void setSilenceThreshold (float threshold) { silenceThreshold = threshold; }

private:
    float detectPitch (const std::vector<float>& buffer);
    float parabolicInterpolation (const std::vector<float>& data, int index);
    
    std::vector<float> ringBuffer;
    std::vector<float> analysisBuffer;
    std::vector<float> correlationBuffer;
    
    std::atomic<float> lastFrequency;
    
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
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BassPitchDetector)
};