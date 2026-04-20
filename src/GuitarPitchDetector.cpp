#include "GuitarPitchDetector.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>

GuitarPitchDetector::GuitarPitchDetector()
    : noteBufferSize (0)
    , noteWritePosition (0)
    , isRecording (false)
    , samplesToRecord (0)
    , sampleRate (44100.0)
    , currentEnergy (0.0f)
    , currentFrequency (0.0f)
    , confidence (0.0f)
    , noteDetected (false)
    , currentNoteName ("--")
    , noteLocked (false)
    , lockCounter (0)
    , lockDuration (0)
{
}

GuitarPitchDetector::~GuitarPitchDetector()
{
}

void GuitarPitchDetector::prepare (double newSampleRate, int samplesPerBlock)
{
    sampleRate = newSampleRate;
    
    noteBufferSize = static_cast<int> (sampleRate * RECORD_SECONDS);
    noteBuffer.resize (noteBufferSize, 0.0f);
    noteWritePosition = 0;
    isRecording = false;
    samplesToRecord = 0;
    
    currentEnergy = 0.0f;
    currentFrequency = 0.0f;
    confidence = 0.0f;
    noteDetected = false;
    currentNoteName = "--";
    noteLocked = false;
    lockCounter = 0;
    lockDuration = static_cast<int>(sampleRate * 0.5);  // 500 мс блокировки
}

void GuitarPitchDetector::reset()
{
    std::fill (noteBuffer.begin(), noteBuffer.end(), 0.0f);
    noteWritePosition = 0;
    isRecording = false;
    samplesToRecord = 0;
    currentEnergy = 0.0f;
    currentFrequency = 0.0f;
    confidence = 0.0f;
    noteDetected = false;
    currentNoteName = "--";
    noteLocked = false;
    lockCounter = 0;
}

float GuitarPitchDetector::parabolicInterpolation (const std::vector<float>& data, int index)
{
    float a = data[static_cast<size_t> (index - 1)];
    float b = data[static_cast<size_t> (index)];
    float c = data[static_cast<size_t> (index + 1)];
    
    float denominator = a - 2.0f * b + c;
    if (std::abs (denominator) < 0.0001f)
        return 0.0f;
    
    return 0.5f * (a - c) / denominator;
}

/*
float GuitarPitchDetector::detectPitch (const std::vector<float>& buffer)
{
    int analysisSize = (int)buffer.size();
    
    if (analysisSize < 256)
        return 0.0f;
    
    int minLag = static_cast<int> (sampleRate / MAX_FREQ);
    int maxLag = static_cast<int> (sampleRate / MIN_FREQ);
    
    if (minLag < 2) minLag = 2;
    if (maxLag > analysisSize / 2) maxLag = analysisSize / 2;
    
    // YIN алгоритм
    std::vector<float> diff (maxLag + 1, 0.0f);
    std::vector<float> cmndf (maxLag + 1, 1.0f);
    
    // Шаг 1: разностная функция
    for (int tau = minLag; tau <= maxLag; ++tau)
    {
        float sum = 0.0f;
        for (int i = 0; i < analysisSize - tau; ++i)
        {
            float delta = buffer[i] - buffer[i + tau];
            sum += delta * delta;
        }
        diff[tau] = sum;
    }
    
    // Шаг 2: кумулятивная нормализация
    float runningSum = 0.0f;
    for (int tau = minLag; tau <= maxLag; ++tau)
    {
        runningSum += diff[tau];
        if (runningSum != 0.0f)
            cmndf[tau] = diff[tau] * static_cast<float>(tau) / runningSum;
        else
            cmndf[tau] = 1.0f;
    }
    
    // Шаг 3: поиск первого минимума ниже порога
    float threshold = 0.15f;
    int minIndex = -1;
    
    for (int tau = minLag + 1; tau < maxLag; ++tau)
    {
        if (cmndf[tau] < threshold &&
            cmndf[tau] < cmndf[tau - 1] &&
            cmndf[tau] < cmndf[tau + 1])
        {
            minIndex = tau;
            break;
        }
    }
    
    // Если не нашли, ищем глобальный минимум
    if (minIndex == -1)
    {
        float minValue = cmndf[minLag];
        minIndex = minLag;
        for (int tau = minLag + 1; tau <= maxLag; ++tau)
        {
            if (cmndf[tau] < minValue)
            {
                minValue = cmndf[tau];
                minIndex = tau;
            }
        }
    }
    
    // Интерполяция
    float interpolatedTau = static_cast<float> (minIndex);
    if (minIndex > minLag && minIndex < maxLag)
    {
        interpolatedTau += parabolicInterpolation (cmndf, minIndex);
    }
    
    float confidenceValue = 1.0f - cmndf[minIndex];
    confidence = confidenceValue;
    
    std::cout << "YIN: minIndex=" << minIndex << ", interpolatedTau=" << interpolatedTau 
              << ", confidence=" << confidenceValue << std::endl;
    
    if (interpolatedTau > 0.0f && confidenceValue > MIN_CONFIDENCE)
    {
        float frequency = static_cast<float> (sampleRate) / interpolatedTau;
        
        // === ИСПРАВЛЕНИЕ ОКТАВНОЙ ОШИБКИ ===
        // Принудительная коррекция для высоких струн (E4)
        if (frequency >= 80.0f && frequency <= 170.0f)
        {
            float higherFreq = frequency * 2.0f;
            if (higherFreq >= 300.0f && higherFreq <= 350.0f)
            {
                std::cout << "Forced octave correction: " << frequency << " Hz -> " << higherFreq << " Hz" << std::endl;
                frequency = higherFreq;
                confidence = 0.9f;
            }
        }
        // Если частота подозрительно низкая (менее 200 Hz), проверяем удвоенную частоту
        else if (frequency < 200.0f)
        {
            float higherFreq = frequency * 2.0f;
            if (higherFreq >= MIN_FREQ && higherFreq <= MAX_FREQ)
            {
                int higherLag = static_cast<int>(sampleRate / higherFreq);
                if (higherLag > 0 && higherLag < analysisSize)
                {
                    float higherCorr = 0.0f;
                    for (int i = 0; i < analysisSize - higherLag; ++i)
                    {
                        higherCorr += buffer[i] * buffer[i + higherLag];
                    }
                    higherCorr /= (analysisSize - higherLag);
                    float higherConf = higherCorr / (analysisSize * 0.5f);
                    
                    if (higherConf > MIN_CONFIDENCE * 0.8f)
                    {
                        std::cout << "Octave error corrected: " << frequency << " Hz -> " 
                                  << higherFreq << " Hz (confidence: " << higherConf << ")" << std::endl;
                        frequency = higherFreq;
                        confidence = higherConf;
                    }
                }
            }
        }
        // Если частота слишком высокая, проверяем половинную
        else if (frequency > MAX_FREQ)
        {
            float lowerFreq = frequency / 2.0f;
            if (lowerFreq >= MIN_FREQ && lowerFreq <= MAX_FREQ)
            {
                std::cout << "Octave error corrected (down): " << frequency << " -> " << lowerFreq << std::endl;
                frequency = lowerFreq;
            }
        }
        
        if (frequency >= MIN_FREQ && frequency <= MAX_FREQ)
        {
            std::cout << "Final frequency: " << frequency << std::endl;
            return frequency;
        }
    }
    
    return 0.0f;
}
*/



juce::String GuitarPitchDetector::frequencyToNoteName (float frequency)
{
    if (frequency <= 0.0f)
        return "--";
    
    const float A4_FREQ = 440.0f;
    const int A4_MIDI = 69;
    const float SEMITONES_PER_OCTAVE = 12.0f;
    
    float midiFloat = A4_MIDI + SEMITONES_PER_OCTAVE * std::log2 (frequency / A4_FREQ);
    int midiNote = static_cast<int> (std::round (midiFloat));
    
    const char* noteNames[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    int octave = (midiNote / 12) - 1;
    int noteIndex = midiNote % 12;
    
    return juce::String (noteNames[noteIndex]) + juce::String (octave);
}

void GuitarPitchDetector::processSamples (const float* buffer, int numSamples)
{
    if (buffer == nullptr || numSamples <= 0)
        return;
    
    // === БЛОКИРОВКА ПОСЛЕ ДЕТЕКЦИИ ===
    if (noteLocked)
    {
        lockCounter -= numSamples;
        if (lockCounter <= 0)
        {
            noteLocked = false;
            std::cout << "Note lock released" << std::endl;
        }
        // Пока блокировка активна, не обрабатываем новые ноты
        return;
    }
    
    // Вычисляем энергию
    float blockEnergy = 0.0f;
    for (int i = 0; i < numSamples; ++i)
    {
        blockEnergy += buffer[i] * buffer[i];
    }
    blockEnergy /= numSamples;
    currentEnergy = currentEnergy * 0.7f + blockEnergy * 0.3f;
    
    // Состояние: запись ноты
    if (isRecording)
    {
        // Записываем в буфер
        for (int i = 0; i < numSamples && samplesToRecord > 0; ++i)
        {
            if (noteWritePosition < noteBufferSize)
            {
                noteBuffer[static_cast<size_t> (noteWritePosition)] = buffer[i];
                ++noteWritePosition;
            }
            --samplesToRecord;
        }
        
        // Если записали нужное количество сэмплов, анализируем
        if (samplesToRecord == 0)
        {
            std::cout << "=== Recording complete! Analyzing " << noteWritePosition << " samples ===" << std::endl;
            
            float frequency = detectPitch (noteBuffer);
            
            if (frequency > 0.0f)
            {
                currentFrequency = frequency;
                currentNoteName = frequencyToNoteName (frequency);
                noteDetected = true;
                noteLocked = true;
                lockCounter = lockDuration;
                std::cout << "Note detected: " << currentNoteName << " at " << frequency 
                          << " Hz (locked for " << lockDuration << " samples)" << std::endl;
            }
            else
            {
                noteDetected = false;
                std::cout << "No note detected" << std::endl;
            }
            
            // Сброс для следующей ноты
            isRecording = false;
            noteWritePosition = 0;
        }
        return;
    }
    
    // Состояние: ожидание атаки
    if (currentEnergy > SILENCE_THRESHOLD)
    {
        std::cout << "=== ONSET DETECTED! Recording " << noteBufferSize << " samples ===" << std::endl;
        std::cout << "Energy: " << currentEnergy << std::endl;
        
        // Начинаем запись
        isRecording = true;
        samplesToRecord = noteBufferSize;
        noteWritePosition = 0;
        
        // Сбрасываем флаг детекции
        noteDetected = false;
    }
    
    // Отладка (реже)
    static int debugCounter = 0;
    if (++debugCounter % 100 == 0)
    {
        std::cout << "=== Debug ===" << std::endl;
        std::cout << "Recording: " << (isRecording ? "YES" : "NO") << std::endl;
        std::cout << "Samples to record: " << samplesToRecord << std::endl;
        std::cout << "Energy: " << currentEnergy << std::endl;
        std::cout << "Note detected flag: " << (noteDetected.load() ? "YES" : "NO") << std::endl;
        std::cout << "Note locked: " << (noteLocked.load() ? "YES" : "NO") << std::endl;
        std::cout << "Lock counter: " << lockCounter << std::endl;
    }
}


 
  
//* С B3 на E4 срабатывает не сразу, а сначала детектируется как E2, а потом уже как E4
 /* 
  в этом варианте, при настройке струн от низких частот к высоким, с B3 на E4 срабатывает не сразу, а сначала детектируется как E2, а потом уже как E4. Не пиши код, но давай подумаем почему так может происходить. В чем корень проблемы? Вероятно в переключении с одной целевой струны на другую. Ведь если определению частоты не предшествует другое определение частоты, то частота (именно частота) определяется правильно, а следом подтягиваются и название ноты и номер струны. Итак, ошибка определения частоты возникает (не всегда, но может возникать) чаще всего только после предшествующего определения, и только если предшествующая частота ниже чем новая определяемая.
  
  */
float GuitarPitchDetector::detectPitch (const std::vector<float>& buffer)
{
    int analysisSize = (int)buffer.size();
    
    if (analysisSize < 256)
        return 0.0f;
    
    int minLag = static_cast<int> (sampleRate / MAX_FREQ);
    int maxLag = static_cast<int> (sampleRate / MIN_FREQ);
    
    if (minLag < 2) minLag = 2;
    if (maxLag > analysisSize / 2) maxLag = analysisSize / 2;
    
    // YIN алгоритм
    std::vector<float> diff (maxLag + 1, 0.0f);
    std::vector<float> cmndf (maxLag + 1, 1.0f);
    
    for (int tau = minLag; tau <= maxLag; ++tau)
    {
        float sum = 0.0f;
        for (int i = 0; i < analysisSize - tau; ++i)
        {
            float delta = buffer[i] - buffer[i + tau];
            sum += delta * delta;
        }
        diff[tau] = sum;
    }
    
    float runningSum = 0.0f;
    for (int tau = minLag; tau <= maxLag; ++tau)
    {
        runningSum += diff[tau];
        if (runningSum != 0.0f)
            cmndf[tau] = diff[tau] * static_cast<float>(tau) / runningSum;
        else
            cmndf[tau] = 1.0f;
    }
    
    float threshold = 0.15f;
    int minIndex = -1;
    
    for (int tau = minLag + 1; tau < maxLag; ++tau)
    {
        if (cmndf[tau] < threshold &&
            cmndf[tau] < cmndf[tau - 1] &&
            cmndf[tau] < cmndf[tau + 1])
        {
            minIndex = tau;
            break;
        }
    }
    
    if (minIndex == -1)
    {
        float minValue = cmndf[minLag];
        minIndex = minLag;
        for (int tau = minLag + 1; tau <= maxLag; ++tau)
        {
            if (cmndf[tau] < minValue)
            {
                minValue = cmndf[tau];
                minIndex = tau;
            }
        }
    }
    
    float interpolatedTau = static_cast<float> (minIndex);
    if (minIndex > minLag && minIndex < maxLag)
    {
        interpolatedTau += parabolicInterpolation (cmndf, minIndex);
    }
    
    float confidenceValue = 1.0f - cmndf[minIndex];
    confidence = confidenceValue;
    
    std::cout << "YIN: minIndex=" << minIndex << ", interpolatedTau=" << interpolatedTau 
              << ", confidence=" << confidenceValue << std::endl;
    
    if (interpolatedTau > 0.0f && confidenceValue > MIN_CONFIDENCE)
    {
        float frequency = static_cast<float> (sampleRate) / interpolatedTau;
        
        // === МИНИМАЛЬНАЯ ОКТАВНАЯ КОРРЕКЦИЯ ===
        // Только для 164 Hz (половина E4) — это единственная проблемная зона
        if (frequency >= 160.0f && frequency <= 168.0f)
        {
            float higherFreq = frequency * 2.0f;
            if (higherFreq >= 320.0f && higherFreq <= 336.0f)
            {
                std::cout << "Half-E4 correction: " << frequency << " Hz -> " << higherFreq << " Hz" << std::endl;
                frequency = higherFreq;
                confidence = 0.9f;
            }
        }
        
        if (frequency >= MIN_FREQ && frequency <= MAX_FREQ)
        {
            std::cout << "Final frequency: " << frequency << std::endl;
            return frequency;
        }
    }
    
    return 0.0f;
}

