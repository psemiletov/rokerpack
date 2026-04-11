#include "GuitarTunerProcessor.h"
#include "GuitarTunerEditor.h"
#include <cmath>
#include <iostream>

GuitarTunerAudioProcessor::GuitarTunerAudioProcessor()
    : AudioProcessor (BusesProperties().withInput  ("Input",  juce::AudioChannelSet::mono(), true)
                                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
    , currentSampleRate (44100.0)
    , currentBlockSize (512)
    , manualMode (false)
    , selectedString (-1)
    , detectedFrequency (0.0f)
    , targetFrequency (0.0f)
    , centsDeviation (0.0f)
    , detectedNote ("--")
    , targetNote ("--")
    , stringNumber (-1)
{
    std::cout << "GuitarTunerAudioProcessor created" << std::endl;
}

GuitarTunerAudioProcessor::~GuitarTunerAudioProcessor()
{
    std::cout << "GuitarTunerAudioProcessor destroyed" << std::endl;
}

const juce::String GuitarTunerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GuitarTunerAudioProcessor::acceptsMidi() const
{
    return false;
}

bool GuitarTunerAudioProcessor::producesMidi() const
{
    return false;
}

double GuitarTunerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GuitarTunerAudioProcessor::getNumPrograms()
{
    return 1;
}

int GuitarTunerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GuitarTunerAudioProcessor::setCurrentProgram (int index)
{
    (void) index;
}

const juce::String GuitarTunerAudioProcessor::getProgramName (int index)
{
    (void) index;
    return {};
}

void GuitarTunerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    (void) index;
    (void) newName;
}

void GuitarTunerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlock;
    
    int analysisBlockSize = juce::jmin (2048, samplesPerBlock * 2);
    pitchDetector = std::make_unique<PitchDetector> (sampleRate, analysisBlockSize);
    
    detectedFrequency = 0.0f;
    
    {
        juce::ScopedLock lock (stringDataLock);
        detectedNote = "--";
        targetNote = "--";
        stringNumber = -1;
    }
    
    std::cout << "prepareToPlay: sampleRate=" << sampleRate << ", samplesPerBlock=" << samplesPerBlock 
              << ", analysisBlockSize=" << analysisBlockSize << std::endl;
}

void GuitarTunerAudioProcessor::releaseResources()
{
    pitchDetector.reset();
    std::cout << "releaseResources" << std::endl;
}

void GuitarTunerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    
    int numSamples = buffer.getNumSamples();
    
    if (numSamples > 0 && buffer.getNumChannels() > 0)
    {
        const float* channelData = buffer.getReadPointer (0);
        
        // Вычисляем RMS для отладки
        double sumSquares = 0.0;
        float peak = 0.0f;
        for (int i = 0; i < numSamples; ++i)
        {
            float absSample = std::abs (channelData[i]);
            sumSquares += channelData[i] * channelData[i];
            if (absSample > peak)
                peak = absSample;
        }
        float rms = std::sqrt (static_cast<float> (sumSquares / numSamples));
        
        // Отладочный вывод каждые 100 блоков
        static int debugCounter = 0;
        if (++debugCounter >= 100)
        {
            debugCounter = 0;
            std::cout << "RMS: " << rms << ", Peak: " << peak << std::endl;
        }
        
        // Всегда пытаемся определить частоту
        float frequency = pitchDetector->getFrequency (channelData, numSamples);
        
        // Отладочный вывод частоты каждые 100 блоков
        static int freqCounter = 0;
        if (++freqCounter >= 100)
        {
            freqCounter = 0;
            std::cout << "Detected frequency: " << frequency << " Hz" << std::endl;
        }
        
        if (frequency > 0.0f)
        {
            detectedFrequency = frequency;
            
            if (manualMode && selectedString >= 0)
            {
                targetFrequency = STRINGS[selectedString].frequency;
                targetNote = STRINGS[selectedString].noteName;
                stringNumber = 6 - selectedString;
            }
            else
            {
                int closestString = findClosestString (frequency);
                if (closestString >= 0)
                {
                    targetFrequency = STRINGS[closestString].frequency;
                    targetNote = STRINGS[closestString].noteName;
                    stringNumber = 6 - closestString;
                    selectedString = closestString;
                }
            }
            
            centsDeviation = calculateCents (detectedFrequency, targetFrequency);
            
            {
                juce::ScopedLock lock (stringDataLock);
                detectedNote = frequencyToNoteName (frequency);
            }
            
            // Отладка: показываем что определили
            static int noteCounter = 0;
            if (++noteCounter >= 100)
            {
                noteCounter = 0;
                std::cout << "Note: " << detectedNote << ", Target: " << targetNote 
                          << ", Cents: " << centsDeviation << std::endl;
            }
        }
        else
        {
            detectedFrequency = 0.0f;
            centsDeviation = 0.0f;
            
            {
                juce::ScopedLock lock (stringDataLock);
                detectedNote = "--";
            }
        }
    }
    
    // Пропускаем аудио без изменений
    if (buffer.getNumChannels() > 1)
    {
        buffer.copyFrom (1, 0, buffer, 0, 0, numSamples);
    }
}

void GuitarTunerAudioProcessor::setManualMode (bool manual, int stringIndex)
{
    manualMode = manual;
    if (manual && stringIndex >= 0 && stringIndex < 6)
        selectedString = stringIndex;
    else if (!manual)
        selectedString = -1;
    
    std::cout << "Manual mode: " << (manual ? "ON" : "OFF") << ", selected string: " << selectedString << std::endl;
}

int GuitarTunerAudioProcessor::findClosestString (float frequency) const
{
    if (frequency <= 0.0f)
        return -1;
    
    int closestIndex = 0;
    float minDiff = std::abs (frequency - STRINGS[0].frequency);
    
    for (int i = 1; i < 6; ++i)
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

float GuitarTunerAudioProcessor::calculateCents (float detectedFreq, float targetFreq) const
{
    if (detectedFreq <= 0.0f || targetFreq <= 0.0f)
        return 0.0f;
    
    float ratio = detectedFreq / targetFreq;
    float cents = 1200.0f * std::log2 (ratio);
    
    return juce::jlimit (-50.0f, 50.0f, cents);
}

juce::String GuitarTunerAudioProcessor::frequencyToNoteName (float frequency) const
{
    if (frequency <= 0.0f)
        return "--";
    
    const float A4_FREQ = 440.0f;
    const int A4_MIDI = 69;
    const float SEMITONES_PER_OCTAVE = 12.0f;
    
    float midiFloat = A4_MIDI + SEMITONES_PER_OCTAVE * std::log2 (frequency / A4_FREQ);
    int midiNote = static_cast<int> (std::round (midiFloat));
    midiNote = juce::jlimit (40, 88, midiNote);
    
    const char* noteNames[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    int octave = (midiNote / 12) - 1;
    int noteIndex = midiNote % 12;
    
    return juce::String (noteNames[noteIndex]) + juce::String (octave);
}

bool GuitarTunerAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* GuitarTunerAudioProcessor::createEditor()
{
    return new GuitarTunerAudioEditor (*this);
}

void GuitarTunerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = juce::ValueTree ("GuitarTuner");
    state.setProperty ("manualMode", manualMode, nullptr);
    state.setProperty ("selectedString", selectedString, nullptr);
    
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void GuitarTunerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    if (xml != nullptr)
    {
        auto state = juce::ValueTree::fromXml (*xml);
        if (state.isValid())
        {
            manualMode = (bool) state.getProperty ("manualMode", false);
            selectedString = (int) state.getProperty ("selectedString", -1);
        }
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GuitarTunerAudioProcessor();
}