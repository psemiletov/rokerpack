#include "BassPitchDetector.h"

BassPitchDetector::BassPitchDetector()
    : sampleRate (44100.0)
    , blockSize (2048)
    , writePosition (0)
    , minLag (0)
    , maxLag (0)
    , silenceThreshold (0.01f)
    , minFreq (40.0f)
    , maxFreq (400.0f)
    , samplesSinceLastAnalysis (0)
    , analysisInterval (1024)
    , lastFrequency (0.0f)
{
}

BassPitchDetector::~BassPitchDetector()
{
}

void BassPitchDetector::prepare (double newSampleRate, int newBlockSize)
{
    sampleRate = newSampleRate;
    blockSize = newBlockSize;
    
    // Минимальный размер для баса (для E1)
    int minBlockSize = static_cast<int> (sampleRate / minFreq) * 2;
    if (blockSize < minBlockSize)
        blockSize = minBlockSize;
    if (blockSize > 4096)
        blockSize = 4096;
    
    ringBuffer.clear();
    ringBuffer.resize (blockSize * 2, 0.0f);
    
    analysisBuffer.clear();
    analysisBuffer.resize (blockSize, 0.0f);
    
    minLag = static_cast<int> (sampleRate / maxFreq);
    maxLag = static_cast<int> (sampleRate / minFreq);
    
    if (minLag < 20) minLag = 20;
    if (maxLag > blockSize / 2) maxLag = blockSize / 2;
    if (maxLag <= minLag) maxLag = minLag + 1;
    
    correlationBuffer.clear();
    correlationBuffer.resize (maxLag - minLag + 1, 0.0f);
    
    // Анализируем примерно 30-40 раз в секунду
//    analysisInterval = static_cast<int> (sampleRate / 35);
    
    analysisInterval = static_cast<int> (sampleRate / 20);
   
    if (analysisInterval < 512) analysisInterval = 512;
    if (analysisInterval > 2048) analysisInterval = 2048;
    
    writePosition = 0;
    samplesSinceLastAnalysis = 0;
    lastFrequency = 0.0f;
}

void BassPitchDetector::reset()
{
    for (size_t i = 0; i < ringBuffer.size(); ++i)
        ringBuffer[i] = 0.0f;
    writePosition = 0;
    samplesSinceLastAnalysis = 0;
    lastFrequency = 0.0f;
}

float BassPitchDetector::processSamples (const float* buffer, int numSamples)
{
    if (buffer == nullptr || numSamples <= 0)
        return lastFrequency.load();
    
    // Копируем новые сэмплы в кольцевой буфер
    for (int i = 0; i < numSamples; ++i)
    {
        ringBuffer[writePosition] = buffer[i];
        ++writePosition;
        if (writePosition >= (int)ringBuffer.size())
            writePosition = 0;
    }
    
    samplesSinceLastAnalysis += numSamples;
    
    // Анализируем только когда накопилось достаточно сэмплов
    if (samplesSinceLastAnalysis >= analysisInterval)
    {
        samplesSinceLastAnalysis = 0;
        
        // Проверяем, достаточно ли данных в буфере
        if (writePosition < blockSize)
        {
            int available = (int)ringBuffer.size() - writePosition;
            if (available < blockSize)
                return lastFrequency.load();
        }
        
        // Копируем блок для анализа
        int readPos = writePosition - blockSize;
        if (readPos < 0)
            readPos += (int)ringBuffer.size();
        
        float maxAmplitude = 0.0f;
        for (int i = 0; i < blockSize; ++i)
        {
            analysisBuffer[i] = ringBuffer[readPos];
            float absVal = std::abs (analysisBuffer[i]);
            if (absVal > maxAmplitude)
                maxAmplitude = absVal;
            ++readPos;
            if (readPos >= (int)ringBuffer.size())
                readPos = 0;
        }
        
        if (maxAmplitude < silenceThreshold)
        {
            lastFrequency = 0.0f;
            return 0.0f;
        }
        
        float frequency = detectPitch (analysisBuffer);
        
        if (frequency >= minFreq && frequency <= maxFreq)
            lastFrequency = frequency;
        else
            lastFrequency = 0.0f;
    }
    
    return lastFrequency.load();
}

float BassPitchDetector::detectPitch (const std::vector<float>& buffer)
{
    int lagRange = maxLag - minLag + 1;
    float maxCorrelation = -1.0f;
    int bestLag = minLag;
    
    for (int lag = minLag; lag <= maxLag; ++lag)
    {
        float sum = 0.0f;
        int limit = blockSize - lag;
        
        for (int i = 0; i < limit; ++i)
        {
            sum += buffer[i] * buffer[i + lag];
        }
        
        int idx = lag - minLag;
        correlationBuffer[idx] = sum;
        
        if (sum > maxCorrelation)
        {
            maxCorrelation = sum;
            bestLag = lag;
        }
    }
    
    int idx = bestLag - minLag;
    float interpolatedLag = (float)bestLag;
    
    if (idx > 0 && idx < lagRange - 1)
    {
        float a = correlationBuffer[idx - 1];
        float b = correlationBuffer[idx];
        float c = correlationBuffer[idx + 1];
        
        float denominator = a - 2.0f * b + c;
        if (denominator != 0.0f)
            interpolatedLag += 0.5f * (a - c) / denominator;
    }
    
    if (interpolatedLag > 0.0f)
        return (float)(sampleRate / interpolatedLag);
    
    return 0.0f;
}

float BassPitchDetector::parabolicInterpolation (const std::vector<float>& data, int index)
{
    float a = data[index - 1];
    float b = data[index];
    float c = data[index + 1];
    
    float denominator = a - 2.0f * b + c;
    if (denominator == 0.0f)
        return 0.0f;
    
    return 0.5f * (a - c) / denominator;
}