#pragma once
#include <JuceHeader.h>

class GateDetector
{
public:
    GateDetector();
    
    void processBlock (const float* buffer, int numSamples);
    bool isSignalPresent() const { return rmsDB > thresholdDB; }
    void setThresholdDB (float thresholdDB) { this->thresholdDB = thresholdDB; }

private:
    float linearToDB (float linear);
    
    float thresholdDB;
    float rmsDB;
};