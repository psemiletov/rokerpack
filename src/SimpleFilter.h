#pragma once
#include <cmath>
#include <iostream>

class SimpleLPF
{
public:
    SimpleLPF() = default;
    
    void setSampleRate(float sr)
    {
        sampleRate = sr;
        updateCoeff();
    }
    
    void setCutoff(float freqHz)
    {
        if (cutoff == freqHz) return;
        cutoff = freqHz;
        updateCoeff();
    }
    
    void reset()
    {
        y1 = 0.0f;
    }
    
    float process(float input)
    {
        float output = a0 * input + b1 * y1;
        y1 = output;
        return output;
    }
    
private:
    void updateCoeff()
    {
        if (sampleRate <= 0.0f) return;
        
        // Ограничиваем частоту среза
        float freq = cutoff;
        if (freq > sampleRate * 0.45f) freq = sampleRate * 0.45f;
        if (freq < 1.0f) freq = 1.0f;
        
        // Расчёт коэффициента для low-pass фильтра
        float omega = 2.0f * 3.14159265f * freq / sampleRate;
       // float alpha = omega / (1.0f + omega);
        
       float alpha = 1.0f - exp(-2.0f * 3.14159265f * freq / sampleRate);

       
        a0 = alpha;
        b1 = 1.0f - alpha;
    }
    
    float sampleRate = 44100.0f;
    float cutoff = 13000.0f;
    float a0 = 1.0f;
    float b1 = 0.0f;
    float y1 = 0.0f;
};

class SimpleHPF
{
public:
    SimpleHPF() = default;
    
    void setSampleRate(float sr)
    {
        sampleRate = sr;
        updateCoeff();
    }
    
    void setCutoff(float freqHz)
    {
        if (cutoff == freqHz) return;
        cutoff = freqHz;
        updateCoeff();
    }
    
    void reset()
    {
        x1 = 0.0f;
        y1 = 0.0f;
    }
    
    float process(float input)
    {
        float output = a0 * input + a1 * x1 + b1 * y1;
        x1 = input;
        y1 = output;
        return output;
    }
    
private:
    void updateCoeff()
    {
        if (sampleRate <= 0.0f) return;
        
        // Ограничиваем частоту среза
        float freq = cutoff;
        if (freq > sampleRate * 0.45f) freq = sampleRate * 0.45f;
        if (freq < 1.0f) freq = 1.0f;
        
        // Расчёт коэффициента для high-pass фильтра
        float omega = 2.0f * 3.14159265f * freq / sampleRate;
        float alpha = omega / (1.0f + omega);
        
        a0 = 1.0f / (1.0f + alpha);
        a1 = -a0;
        b1 = (1.0f - alpha) / (1.0f + alpha);
    }
    
    float sampleRate = 44100.0f;
    float cutoff = 200.0f;
    float a0 = 1.0f;
    float a1 = 0.0f;
    float b1 = 0.0f;
    float x1 = 0.0f;
    float y1 = 0.0f;
};