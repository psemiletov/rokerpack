#include "BassPitchDetector.h"

// Определение частот струн бас-гитары
const BassPitchDetector::StringInfo BassPitchDetector::STRINGS[BassPitchDetector::NUM_BASS_STRINGS] = {
    { 41.20f,  "E1" },   // 4-я (самая толстая)
    { 55.00f,  "A1" },   // 3-я
    { 73.42f,  "D2" },   // 2-я
    { 98.00f,  "G2" }    // 1-я (самая тонкая)
};

BassPitchDetector::BassPitchDetector()
    : sampleRate (44100.0)
    , blockSize (2048)
    , writePosition (0)
    , minLag (0)
    , maxLag (0)
    , silenceThreshold (0.008f)
    , lastFrequency (0.0f)
    , currentLevel (0.0f)
    , minFreq (38.0f)
    , maxFreq (400.0f)
    , samplesSinceLastAnalysis (0)
    , analysisInterval (1024)
    , stableFrequency (0.0f)
    , stableFrames (0)
    , silenceFrames (0)
    , newStringFrames (0)
{
}

BassPitchDetector::~BassPitchDetector()
{
}

void BassPitchDetector::prepare (double newSampleRate, int newBlockSize)
{
    sampleRate = newSampleRate;
    blockSize = newBlockSize;
    
    int minBlockSize = static_cast<int> (sampleRate / minFreq) * 2;
    blockSize = std::max (newBlockSize, minBlockSize);
    blockSize = std::min (blockSize, 4096);
    
    ringBuffer.clear();
    ringBuffer.resize (static_cast<size_t> (blockSize * 2), 0.0f);
    
    analysisBuffer.clear();
    analysisBuffer.resize (static_cast<size_t> (blockSize), 0.0f);
    
    nsdfBuffer.clear();
    nsdfBuffer.resize (static_cast<size_t> (blockSize), 0.0f);
    
    minLag = static_cast<int> (sampleRate / maxFreq);
    maxLag = static_cast<int> (sampleRate / minFreq);
    minLag = std::max (2, minLag);
    maxLag = std::min (blockSize / 2, maxLag);
    
    analysisInterval = static_cast<int> (sampleRate / 30);
    analysisInterval = std::max (512, std::min (2048, analysisInterval));
    
    writePosition = 0;
    samplesSinceLastAnalysis = 0;
    lastFrequency = 0.0f;
    currentLevel = 0.0f;
    stableFrequency = 0.0f;
    stableFrames = 0;
    silenceFrames = 0;
    newStringFrames = 0;
}

void BassPitchDetector::reset()
{
    std::fill (ringBuffer.begin(), ringBuffer.end(), 0.0f);
    writePosition = 0;
    samplesSinceLastAnalysis = 0;
    lastFrequency = 0.0f;
    currentLevel = 0.0f;
    stableFrequency = 0.0f;
    stableFrames = 0;
    silenceFrames = 0;
    newStringFrames = 0;
}

int BassPitchDetector::findClosestString (float frequency) const
{
    if (frequency <= 0.0f)
        return -1;
    
    int closestIndex = 0;
    float minDiff = std::abs (frequency - STRINGS[0].frequency);
    
    for (int i = 1; i < NUM_BASS_STRINGS; ++i)
    {
        float diff = std::abs (frequency - STRINGS[i].frequency);
        if (diff < minDiff)
        {
            minDiff = diff;
            closestIndex = i;
        }
    }
    
    return closestIndex;
}

float BassPitchDetector::parabolicInterpolation (const std::vector<float>& data, int index)
{
    float a = data[static_cast<size_t> (index - 1)];
    float b = data[static_cast<size_t> (index)];
    float c = data[static_cast<size_t> (index + 1)];
    
    float denominator = a - 2.0f * b + c;
    if (std::abs (denominator) < 0.0001f)
        return 0.0f;
    
    return 0.5f * (a - c) / denominator;
}

float BassPitchDetector::detectPitch (const std::vector<float>& buffer)
{
    for (int tau = minLag; tau <= maxLag; ++tau)
    {
        float numerator = 0.0f;
        float denominator = 0.0f;
        int limit = blockSize - tau;
        
        for (int i = 0; i < limit; ++i)
        {
            numerator += buffer[i] * buffer[i + tau];
            denominator += buffer[i] * buffer[i] + buffer[i + tau] * buffer[i + tau];
        }
        
        if (denominator > 0.0f)
            nsdfBuffer[tau] = 2.0f * numerator / denominator;
        else
            nsdfBuffer[tau] = 0.0f;
    }
    
    int bestLag = minLag;
    float maxValue = nsdfBuffer[minLag];
    
    for (int tau = minLag + 1; tau <= maxLag; ++tau)
    {
        if (nsdfBuffer[tau] > maxValue)
        {
            maxValue = nsdfBuffer[tau];
            bestLag = tau;
        }
    }
    
    if (maxValue < 0.45f)
        return 0.0f;
    
    float interpolatedLag = static_cast<float> (bestLag);
    if (bestLag > minLag && bestLag < maxLag)
        interpolatedLag += parabolicInterpolation (nsdfBuffer, bestLag);
    
    if (interpolatedLag > 0.0f)
    {
        float frequency = static_cast<float> (sampleRate) / interpolatedLag;
        
        if (frequency < minFreq)
            return 0.0f;
        if (frequency > maxFreq)
        {
            float subFreq = frequency / 2.0f;
            if (subFreq >= minFreq && subFreq <= maxFreq)
                return subFreq;
            return 0.0f;
        }
        
        return frequency;
    }
    
    return 0.0f;
}



float BassPitchDetector::processSamples (const float* buffer, int numSamples)
{
    if (buffer == nullptr || numSamples <= 0)
        return lastFrequency.load();
    
    for (int i = 0; i < numSamples; ++i)
    {
        ringBuffer[static_cast<size_t> (writePosition)] = buffer[i];
        ++writePosition;
        if (writePosition >= static_cast<int> (ringBuffer.size()))
            writePosition = 0;
    }
    
    samplesSinceLastAnalysis += numSamples;
    
    if (samplesSinceLastAnalysis >= analysisInterval)
    {
        samplesSinceLastAnalysis = 0;
        
        if (writePosition < blockSize)
        {
            int available = static_cast<int> (ringBuffer.size()) - writePosition;
            if (available < blockSize)
                return lastFrequency.load();
        }
        
        int readPos = writePosition - blockSize;
        if (readPos < 0)
            readPos += static_cast<int> (ringBuffer.size());
        
        float maxAmplitude = 0.0f;
        for (int i = 0; i < blockSize; ++i)
        {
            analysisBuffer[static_cast<size_t> (i)] = ringBuffer[static_cast<size_t> (readPos)];
            float absVal = std::abs (analysisBuffer[static_cast<size_t> (i)]);
            if (absVal > maxAmplitude)
                maxAmplitude = absVal;
            ++readPos;
            if (readPos >= static_cast<int> (ringBuffer.size()))
                readPos = 0;
        }
        
        currentLevel = maxAmplitude;
        
        // Тишина - сброс всего
        if (maxAmplitude < silenceThreshold)
        {
            silenceFrames++;
            if (silenceFrames > maxSilenceFrames)
            {
                stableFrequency = 0.0f;
                lastFrequency = 0.0f;
                stableFrames = 0;
                newStringFrames = 0;
            }
            return lastFrequency.load();
        }
        
        silenceFrames = 0;
        
        float rawFrequency = detectPitch (analysisBuffer);
        
        if (rawFrequency <= 0.0f)
            return lastFrequency.load();
        
        int closestStringIndex = findClosestString (rawFrequency);
        
        // Если нет стабильной частоты - инициализируем
        if (stableFrequency <= 0.0f && closestStringIndex >= 0)
        {
            stableFrequency = rawFrequency;
            lastFrequency = rawFrequency;
            stableFrames = requiredStableFrames;
            return lastFrequency.load();
        }
        
        // Проверяем, нужно ли переключиться на другую струну
        int currentStringIndex = findClosestString (stableFrequency);
        bool isDifferentString = (closestStringIndex != currentStringIndex);
        
        if (isDifferentString)
        {
            // Другая струна - накапливаем подтверждения
            newStringFrames++;
            if (newStringFrames >= 3)
            {
                // Переключаемся
                stableFrequency = rawFrequency;
                lastFrequency = rawFrequency;
                stableFrames = requiredStableFrames;
                newStringFrames = 0;
            }
            else
            {
                // Пока не переключаемся, показываем старую частоту
                lastFrequency = stableFrequency;
            }
            return lastFrequency.load();
        }
        
        // Та же струна - сбрасываем счётчик новой струны
        newStringFrames = 0;
        
        // Проверяем, стабильна ли частота
        bool isStable = std::abs(rawFrequency - stableFrequency) < 7.0f;
        
        if (isStable)
        {
            stableFrames++;
            if (stableFrames >= requiredStableFrames)
            {
                // Плавное обновление
                stableFrequency = stableFrequency * 0.7f + rawFrequency * 0.3f;
                lastFrequency = stableFrequency;
                stableFrames = requiredStableFrames;
            }
            else
            {
                lastFrequency = stableFrequency;
            }
        }
        else
        {
            // Нестабильно - сбрасываем счётчик, но показываем старую частоту
            stableFrames = 0;
            lastFrequency = stableFrequency;
        }
    }
    
    return lastFrequency.load();
}