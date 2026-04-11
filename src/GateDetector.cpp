#include "GateDetector.h"
#include <cmath>

GateDetector::GateDetector()
    : thresholdDB (-50.0f)
    , rmsDB (-100.0f)
{
}

void GateDetector::processBlock (const float* buffer, int numSamples)
{
    if (buffer == nullptr || numSamples <= 0)
    {
        rmsDB = -100.0f;
        return;
    }
    
    double sumSquares = 0.0;
    for (int i = 0; i < numSamples; ++i)
    {
        sumSquares += static_cast<double> (buffer[i]) * static_cast<double> (buffer[i]);
    }
    
    double rmsLinear = std::sqrt (sumSquares / static_cast<double> (numSamples));
    
    if (rmsLinear <= 0.00001f)
        rmsDB = -100.0f;
    else
        rmsDB = 20.0f * std::log10 (static_cast<float> (rmsLinear));
}

float GateDetector::linearToDB (float linear)
{
    if (linear <= 0.00001f)
        return -100.0f;
    return 20.0f * std::log10 (linear);
}