#pragma once
#include <JuceHeader.h>
#include <atomic>

// Need by GuitarTuner
class GateDetector
{
public:
    GateDetector();
    
    void processBlock (const float* buffer, int numSamples);
    bool isSignalPresent() const { return rmsDB.load() > thresholdDB; }
    void setThresholdDB (float db) { thresholdDB = db; }

private:
    float linearToDB (float linear);
    
    float thresholdDB;
    std::atomic<float> rmsDB;  // ← atomic для потокобезопасности
};