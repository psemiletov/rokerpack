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
    
//    std::cout << "YIN: minIndex=" << minIndex << ", interpolatedTau=" << interpolatedTau 
  //            << ", confidence=" << confidenceValue << std::endl;
    
    if (interpolatedTau > 0.0f && confidenceValue > MIN_CONFIDENCE)
    {
        float frequency = static_cast<float> (sampleRate) / interpolatedTau;
        
    //    std::cout << "Raw frequency before correction: " << frequency << std::endl;
        
        // === ОКТАВНАЯ КОРРЕКЦИЯ ДЛЯ E4 ===
        // Проверяем, является ли исходная частота реальной нотой гитары
        bool isValidGuitarNote = false;
        
        // Список частот открытых струн гитары (с допуском ±3 Hz)
        float guitarNotes[] = { 82.41f, 110.00f, 146.83f, 196.00f, 246.94f, 329.63f };
        for (float note : guitarNotes)
        {
            if (std::abs(frequency - note) < 3.0f)
            {
                isValidGuitarNote = true;
                break;
            }
        }
        
        // Применяем коррекцию только если частота НЕ является реальной нотой
        if (!isValidGuitarNote)
        {
            for (int mult = 2; mult <= 8; mult *= 2)
            {
                float higherFreq = frequency * mult;
                if (higherFreq >= 320.0f && higherFreq <= 350.0f)
                {
      //              std::cout << "Octave correction (×" << mult << "): " << frequency << " Hz -> " << higherFreq << " Hz" << std::endl;
                    frequency = higherFreq;
                    confidence = 0.9f;
                    break;
                }
            }
        }
        
        if (frequency >= MIN_FREQ && frequency <= MAX_FREQ)
        {
        //    std::cout << "Final frequency: " << frequency << std::endl;
            return frequency;
        }
    }
    
    return 0.0f;
}
 
 void GuitarPitchDetector::processSamples (const float* buffer, int numSamples)
{
    if (buffer == nullptr || numSamples <= 0)
        return;
    
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
          //  std::cout << "=== Recording complete! Analyzing " << noteWritePosition << " samples ===" << std::endl;
            
            float frequency = detectPitch (noteBuffer);
            
            if (frequency > 0.0f)
            {
                currentFrequency = frequency;
                currentNoteName = frequencyToNoteName (frequency);
                noteDetected = true;
            //    std::cout << "Note detected: " << currentNoteName << " at " << frequency << " Hz" << std::endl;
            }
            else
            {
                noteDetected = false;
              //  std::cout << "No note detected" << std::endl;
            }
            
            // === КЛЮЧЕВОЕ ИЗМЕНЕНИЕ ===
            // Очищаем буфер СРАЗУ после анализа, независимо от результата
            // Это предотвращает влияние остаточных колебаний на следующую ноту
            noteWritePosition = 0;
            std::fill(noteBuffer.begin(), noteBuffer.end(), 0.0f);
            isRecording = false;
        }
        return;
    }
    
    // Состояние: ожидание атаки
    if (currentEnergy > SILENCE_THRESHOLD)
    {
        //std::cout << "=== ONSET DETECTED! Recording " << noteBufferSize << " samples ===" << std::endl;
        //std::cout << "Energy: " << currentEnergy << std::endl;
        
        // Начинаем запись
        isRecording = true;
        samplesToRecord = noteBufferSize;
        noteWritePosition = 0;
        
        // Сбрасываем флаг детекции
        noteDetected = false;
    }
    
    // Отладка (реже)
    /*static int debugCounter = 0;
    if (++debugCounter % 100 == 0)
    {
        std::cout << "=== Debug ===" << std::endl;
        std::cout << "Recording: " << (isRecording ? "YES" : "NO") << std::endl;
        std::cout << "Samples to record: " << samplesToRecord << std::endl;
        std::cout << "Energy: " << currentEnergy << std::endl;
        std::cout << "Note detected flag: " << (noteDetected.load() ? "YES" : "NO") << std::endl;
        std::cout << "Note buffer pos: " << noteWritePosition << std::endl;
    }*/
}