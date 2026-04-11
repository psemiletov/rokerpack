#pragma once
#include <JuceHeader.h>

class PitchDetector
{
public:
    PitchDetector (double sampleRate, int blockSize);
    ~PitchDetector();
    
    void setSampleRate (double sampleRate);
    float getFrequency (const float* buffer, int numSamples);
    int getBlockSize() const { return blockSize; }
    
private:
    float yin (const float* buffer, int startIndex);
    float parabolicInterpolation (float* data, int index);
    
    double sampleRate;
    int blockSize;
    
    std::vector<float> ringBuffer;
    int writePosition;
    
    std::vector<float> diffBuffer;
    std::vector<float> cmndfBuffer;
    
    static constexpr float TOLERANCE = 0.05f;
    static constexpr float DEFAULT_FREQ = 0.0f;
};