#include "GuitarTunerProcessor.h"
#include "GuitarTunerEditor.h"
#include <cmath>

// Константы
constexpr int ANALYSIS_BLOCK_SIZE = 2048;
constexpr float MAX_CENTS_DEVIATION = 50.0f;
constexpr int MIN_GUITAR_MIDI = 40;
constexpr int MAX_GUITAR_MIDI = 88;

GuitarTunerAudioProcessor::GuitarTunerAudioProcessor()
    : AudioProcessor (BusesProperties().withInput  ("Input",  juce::AudioChannelSet::mono(), true)
                                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
    , currentSampleRate (44100.0)
    , detectedFrequency (0.0f)
    , targetFrequency (0.0f)
    , centsDeviation (0.0f)
    , stringNumber (-1)
    , signalActive (false)
    , smoothedFrequency (0.0f)
    , detectedNote ("--")
    , targetNote ("--")
{
}

GuitarTunerAudioProcessor::~GuitarTunerAudioProcessor()
{
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
    
    pitchDetector = std::make_unique<PitchDetector> (sampleRate, ANALYSIS_BLOCK_SIZE);
    
    detectedFrequency.store (0.0f);
    targetFrequency.store (0.0f);
    centsDeviation.store (0.0f);
    stringNumber.store (-1);
    signalActive.store (false);
    smoothedFrequency.store (0.0f);
    silenceCounter = 0;
    
    {
        juce::ScopedLock lock (stringDataLock);
        detectedNote = "--";
        targetNote = "--";
    }
}

void GuitarTunerAudioProcessor::releaseResources()
{
    pitchDetector.reset();
}

void GuitarTunerAudioProcessor::processBlock (juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(buffer, midiMessages);
}

void GuitarTunerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    
    int numSamples = buffer.getNumSamples();
    
    if (numSamples > 0 && buffer.getNumChannels() > 0)
    {
        const float* channelData = buffer.getReadPointer (0);
        
        // Проверяем наличие сигнала
        gateDetector.processBlock (channelData, numSamples);
        bool hasSignal = gateDetector.isSignalPresent();
        signalActive.store (hasSignal);
        
        if (hasSignal)
        {
            float frequency = pitchDetector->getFrequency (channelData, numSamples);
            
            if (frequency > 0.0f)
            {
                // Сглаживание частоты для UI
                silenceCounter = 0;
                float currentSmoothed = smoothedFrequency.load();
                
                if (currentSmoothed <= 0.0f)
                {
                    smoothedFrequency.store (frequency);
                }
                else
                {
                    float newSmoothed = currentSmoothed * (1.0f - smoothingFactor) + frequency * smoothingFactor;
                    smoothedFrequency.store (newSmoothed);
                }
                
                float displayFreq = smoothedFrequency.load();
                detectedFrequency.store (displayFreq);
                
                int closestString = findClosestString (displayFreq);
                if (closestString >= 0)
                {
                    targetFrequency.store (STRINGS[closestString].frequency);
                    
                    {
                        juce::ScopedLock lock (stringDataLock);
                        targetNote = STRINGS[closestString].noteName;
                    }
                    
                    stringNumber.store (NUM_GUITAR_STRINGS - closestString);
                }
                
                float targetFreq = targetFrequency.load();
                if (targetFreq > 0.0f)
                {
                    centsDeviation.store (calculateCents (displayFreq, targetFreq));
                }
                else
                {
                    centsDeviation.store (0.0f);
                }
                
                {
                    juce::ScopedLock lock (stringDataLock);
                    detectedNote = frequencyToNoteName (displayFreq);
                }
            }
            else
            {
                detectedFrequency.store (0.0f);
                centsDeviation.store (0.0f);
                {
                    juce::ScopedLock lock (stringDataLock);
                    detectedNote = "--";
                }
            }
        }
        else
        {
            // Плавное затухание частоты при отсутствии сигнала
            silenceCounter++;
            if (silenceCounter > silenceTimeout)
            {
                float currentSmoothed = smoothedFrequency.load();
                if (currentSmoothed > 0.0f)
                {
                    float newSmoothed = currentSmoothed * 0.92f;  // постепенное затухание
                    if (newSmoothed < 0.5f)
                        newSmoothed = 0.0f;
                    smoothedFrequency.store (newSmoothed);
                    detectedFrequency.store (newSmoothed);
                }
            }
            
            centsDeviation.store (0.0f);
            {
                juce::ScopedLock lock (stringDataLock);
                detectedNote = "--";
                targetNote = "--";
                stringNumber.store (-1);
            }
        }
    }
    
    if (buffer.getNumChannels() > 1)
    {
        buffer.copyFrom (1, 0, buffer, 0, 0, numSamples);
    }
}

int GuitarTunerAudioProcessor::findClosestString (float frequency) const
{
    if (frequency <= 0.0f)
        return -1;
    
    int closestIndex = 0;
    float minDiff = std::abs (frequency - STRINGS[0].frequency);
    
    for (int i = 1; i < NUM_GUITAR_STRINGS; ++i)
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
    
    return juce::jlimit (-MAX_CENTS_DEVIATION, MAX_CENTS_DEVIATION, cents);
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
    midiNote = juce::jlimit (MIN_GUITAR_MIDI, MAX_GUITAR_MIDI, midiNote);
    
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
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void GuitarTunerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // Восстанавливать нечего
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GuitarTunerAudioProcessor();
}