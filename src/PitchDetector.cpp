#include "PitchDetector.h"
#include <cmath>
#include <algorithm>
#include <iostream>

PitchDetector::PitchDetector (double sampleRate, int blockSize)
    : sampleRate (sampleRate), blockSize (blockSize), writePosition (0)
{
    ringBuffer.resize (blockSize * 2, 0.0f);
    diffBuffer.resize (blockSize);
    cmndfBuffer.resize (blockSize);
}

PitchDetector::~PitchDetector()
{
}

void PitchDetector::setSampleRate (double newSampleRate)
{
    sampleRate = newSampleRate;
}

float PitchDetector::getFrequency (const float* buffer, int numSamples)
{
    if (buffer == nullptr || numSamples <= 0)
        return DEFAULT_FREQ;
    
    for (int i = 0; i < numSamples; ++i)
    {
        ringBuffer[writePosition] = buffer[i];
        writePosition = (writePosition + 1) % ringBuffer.size();
    }
    
    if (writePosition < blockSize && ringBuffer.size() - writePosition < blockSize)
        return DEFAULT_FREQ;
    
    std::vector<float> analysisBuffer (blockSize);
    int readPos = writePosition - blockSize;
    if (readPos < 0)
        readPos += ringBuffer.size();
    
    for (int i = 0; i < blockSize; ++i)
    {
        analysisBuffer[i] = ringBuffer[readPos];
        readPos = (readPos + 1) % ringBuffer.size();
    }
    
    float period = yin (analysisBuffer.data(), 0);
    
    if (period <= 0.0f)
        return DEFAULT_FREQ;
    
    float frequency = static_cast<float> (sampleRate) / period;
    
    if (frequency < 40.0f || frequency > 1000.0f)
        return DEFAULT_FREQ;
    
    return frequency;
}

float PitchDetector::yin (const float* buffer, int startIndex)
{
    for (int tau = 0; tau < blockSize; ++tau)
    {
        float sum = 0.0f;
        for (int i = 0; i < blockSize - tau; ++i)
        {
            float delta = buffer[startIndex + i] - buffer[startIndex + i + tau];
            sum += delta * delta;
        }
        diffBuffer[tau] = sum;
    }
    
    cmndfBuffer[0] = 1.0f;
    float runningSum = 0.0f;
    
    for (int tau = 1; tau < blockSize; ++tau)
    {
        runningSum += diffBuffer[tau];
        if (runningSum == 0.0f)
            cmndfBuffer[tau] = 1.0f;
        else
            cmndfBuffer[tau] = diffBuffer[tau] * tau / runningSum;
    }
    
    int minIndex = -1;
    for (int tau = 2; tau < blockSize - 1; ++tau)
    {
        if (cmndfBuffer[tau] < TOLERANCE &&
            cmndfBuffer[tau] < cmndfBuffer[tau - 1] &&
            cmndfBuffer[tau] < cmndfBuffer[tau + 1])
        {
            minIndex = tau;
            break;
        }
    }
    
    if (minIndex == -1)
    {
        float minValue = cmndfBuffer[1];
        minIndex = 1;
        for (int tau = 2; tau < blockSize; ++tau)
        {
            if (cmndfBuffer[tau] < minValue)
            {
                minValue = cmndfBuffer[tau];
                minIndex = tau;
            }
        }
    }
    
    float interpolatedTau = static_cast<float> (minIndex);
    if (minIndex > 0 && minIndex < blockSize - 1)
    {
        interpolatedTau += parabolicInterpolation (cmndfBuffer.data(), minIndex);
    }
    
    return interpolatedTau;
}

float PitchDetector::parabolicInterpolation (float* data, int index)
{
    float a = data[index - 1];
    float b = data[index];
    float c = data[index + 1];
    
    float denominator = a - 2.0f * b + c;
    if (denominator == 0.0f)
        return 0.0f;
    
    return 0.5f * (a - c) / denominator;
}