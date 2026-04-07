#pragma once
#include <cmath>

class VintageEQ
{
public:
    VintageEQ() = default;
    
    void setSampleRate(double sr)
    {
        sampleRate = sr;
        updateLowCoeffs();
        updateHighCoeffs();
    }
    
    void setLowsGain(float gainDB)
    {
        lowsGain = gainDB;
        updateLowCoeffs();
    }
    
    void setTrebleGain(float gainDB)
    {
        trebleGain = gainDB;
        updateHighCoeffs();
    }
    
    void reset()
    {
        lowX1 = 0.0f; lowX2 = 0.0f;
        lowY1 = 0.0f; lowY2 = 0.0f;
        highX1 = 0.0f; highX2 = 0.0f;
        highY1 = 0.0f; highY2 = 0.0f;
    }
    
    float process(float input)
    {
        // Low shelf filter (Bass)
        float lowOutput = lowB0 * input + lowB1 * lowX1 + lowB2 * lowX2
                        - lowA1 * lowY1 - lowA2 * lowY2;
        
        lowX2 = lowX1;
        lowX1 = input;
        lowY2 = lowY1;
        lowY1 = lowOutput;
        
        // High shelf filter (Treble) - последовательно
        float highOutput = highB0 * lowOutput + highB1 * highX1 + highB2 * highX2
                         - highA1 * highY1 - highA2 * highY2;
        
        highX2 = highX1;
        highX1 = lowOutput;
        highY2 = highY1;
        highY1 = highOutput;
        
        return highOutput;
    }
    
private:
    void updateLowCoeffs()
    {
        // Low shelf filter at 100 Hz, Q=0.7
       // float w0 = 2.0f * float(M_PI) * 100.0f / float(sampleRate);
       
        float w0 = 2.0f * float(M_PI) * 120.0f / float(sampleRate);
        
        float A = pow(10.0f, lowsGain / 40.0f);
        float alpha = sin(w0) / (2.0f * 0.7f);
        float cosw0 = cos(w0);
        
        float b0 = A * ((A + 1) - (A - 1) * cosw0 + 2 * sqrt(A) * alpha);
        float b1 = 2 * A * ((A - 1) - (A + 1) * cosw0);
        float b2 = A * ((A + 1) - (A - 1) * cosw0 - 2 * sqrt(A) * alpha);
        float a0 = (A + 1) + (A - 1) * cosw0 + 2 * sqrt(A) * alpha;
        float a1 = -2 * ((A - 1) + (A + 1) * cosw0);
        float a2 = (A + 1) + (A - 1) * cosw0 - 2 * sqrt(A) * alpha;
        
        lowB0 = b0 / a0;
        lowB1 = b1 / a0;
        lowB2 = b2 / a0;
        lowA1 = a1 / a0;
        lowA2 = a2 / a0;
    }
    
    void updateHighCoeffs()
    {
        // High shelf filter at 10 kHz, Q=0.7
        float w0 = 2.0f * float(M_PI) * 4000.0f / float(sampleRate);
        
        float A = pow(10.0f, trebleGain / 40.0f);
        float alpha = sin(w0) / (2.0f * 0.7f);
        float cosw0 = cos(w0);
        
        // Правильные формулы для High Shelf (из RBJ Cookbook)
        float b0 = A * ((A + 1) + (A - 1) * cosw0 + 2 * sqrt(A) * alpha);
        float b1 = -2 * A * ((A - 1) + (A + 1) * cosw0);
        float b2 = A * ((A + 1) + (A - 1) * cosw0 - 2 * sqrt(A) * alpha);
        float a0 = (A + 1) - (A - 1) * cosw0 + 2 * sqrt(A) * alpha;
        float a1 = 2 * ((A - 1) - (A + 1) * cosw0);
        float a2 = (A + 1) - (A - 1) * cosw0 - 2 * sqrt(A) * alpha;
        
        highB0 = b0 / a0;
        highB1 = b1 / a0;
        highB2 = b2 / a0;
        highA1 = a1 / a0;
        highA2 = a2 / a0;
        
        // Отладка
        static float lastGain = -999;
        if (lastGain != trebleGain)
        {
            std::cout << "Treble gain: " << trebleGain << " dB, A: " << A << std::endl;
            std::cout << "High shelf coeffs: b0=" << highB0 << ", b1=" << highB1 << ", b2=" << highB2 << std::endl;
            lastGain = trebleGain;
        }
    }
    
    double sampleRate = 44100.0;
    float lowsGain = 0.0f;
    float trebleGain = 0.0f;
    
    // Low shelf coefficients
    float lowB0 = 1.0f, lowB1 = 0.0f, lowB2 = 0.0f;
    float lowA1 = 0.0f, lowA2 = 0.0f;
    float lowX1 = 0.0f, lowX2 = 0.0f, lowY1 = 0.0f, lowY2 = 0.0f;
    
    // High shelf coefficients
    float highB0 = 1.0f, highB1 = 0.0f, highB2 = 0.0f;
    float highA1 = 0.0f, highA2 = 0.0f;
    float highX1 = 0.0f, highX2 = 0.0f, highY1 = 0.0f, highY2 = 0.0f;
};