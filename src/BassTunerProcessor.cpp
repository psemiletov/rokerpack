#include "BassTunerProcessor.h"
#include "BassTunerEditor.h"
#include <cmath>

// Константы
constexpr int ANALYSIS_BLOCK_SIZE = 4096;
constexpr float MAX_CENTS_DEVIATION = 50.0f;
constexpr int MIN_BASS_MIDI = 28;
constexpr int MAX_BASS_MIDI = 55;

BassTunerAudioProcessor::BassTunerAudioProcessor()
    : AudioProcessor (BusesProperties().withInput  ("Input",  juce::AudioChannelSet::mono(), true)
                                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
    , currentSampleRate (44100.0)
    , targetFrequency (0.0f)
    , centsDeviation (0.0f)
    , stringNumber (-1)
    , smoothedFrequency (0.0f)
    , detectedNote ("--")
    , targetNote ("--")
{
    gateParam = new juce::AudioParameterFloat ("gate", "Gate", 0.0f, 1.0f, 0.0f);
    addParameter (gateParam);
}

BassTunerAudioProcessor::~BassTunerAudioProcessor()
{
}

const juce::String BassTunerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BassTunerAudioProcessor::acceptsMidi() const
{
    return false;
}

bool BassTunerAudioProcessor::producesMidi() const
{
    return false;
}

double BassTunerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BassTunerAudioProcessor::getNumPrograms()
{
    return 1;
}

int BassTunerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BassTunerAudioProcessor::setCurrentProgram (int index)
{
    (void) index;
}

const juce::String BassTunerAudioProcessor::getProgramName (int index)
{
    (void) index;
    return {};
}

void BassTunerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    (void) index;
    (void) newName;
}

void BassTunerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    
    pitchDetector = std::make_unique<BassPitchDetector>();
    pitchDetector->prepare (sampleRate, ANALYSIS_BLOCK_SIZE);
    pitchDetector->setSilenceThreshold (0.005f);
    
    targetFrequency.store (0.0f);
    centsDeviation.store (0.0f);
    stringNumber.store (-1);
    smoothedFrequency.store (0.0f);
    silenceCounter = 0;
    
    {
        juce::ScopedLock lock (stringDataLock);
        detectedNote = "--";
        targetNote = "--";
    }
}

void BassTunerAudioProcessor::releaseResources()
{
    pitchDetector.reset();
}

void BassTunerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    
    int numSamples = buffer.getNumSamples();
    
    if (numSamples > 0 && buffer.getNumChannels() > 0)
    {
        const float* channelData = buffer.getReadPointer (0);
        
        float frequency = pitchDetector->processSamples (channelData, numSamples);
        
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
            
            int closestString = findClosestString (displayFreq);
            if (closestString >= 0)
            {
                targetFrequency.store (STRINGS[closestString].frequency);
                
                {
                    juce::ScopedLock lock (stringDataLock);
                    targetNote = STRINGS[closestString].noteName;
                }
                
                stringNumber.store (NUM_BASS_STRINGS - closestString);
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
    
    // Копируем входной сигнал в правый канал (если нужно стерео)
    if (buffer.getNumChannels() > 1)
    {
        buffer.copyFrom (1, 0, buffer, 0, 0, numSamples);
    }
}

void BassTunerAudioProcessor::processBlock (juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(buffer, midiMessages);
}

int BassTunerAudioProcessor::findClosestString (float frequency) const
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

float BassTunerAudioProcessor::calculateCents (float detectedFreq, float targetFreq) const
{
    if (detectedFreq <= 0.0f || targetFreq <= 0.0f)
        return 0.0f;
    
    float ratio = detectedFreq / targetFreq;
    float cents = 1200.0f * std::log2 (ratio);
    
    return juce::jlimit (-MAX_CENTS_DEVIATION, MAX_CENTS_DEVIATION, cents);
}

juce::String BassTunerAudioProcessor::frequencyToNoteName (float frequency) const
{
    if (frequency <= 0.0f)
        return "--";
    
    const float A4_FREQ = 440.0f;
    const int A4_MIDI = 69;
    const float SEMITONES_PER_OCTAVE = 12.0f;
    
    float midiFloat = A4_MIDI + SEMITONES_PER_OCTAVE * std::log2 (frequency / A4_FREQ);
    int midiNote = static_cast<int> (std::round (midiFloat));
    midiNote = juce::jlimit (MIN_BASS_MIDI, MAX_BASS_MIDI, midiNote);
    
    const char* noteNames[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    int octave = (midiNote / 12) - 1;
    int noteIndex = midiNote % 12;
    
    return juce::String (noteNames[noteIndex]) + juce::String (octave);
}

bool BassTunerAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* BassTunerAudioProcessor::createEditor()
{
    return new BassTunerAudioEditor (*this);
}

void BassTunerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = juce::ValueTree ("BassTuner");
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void BassTunerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // Восстанавливать нечего
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BassTunerAudioProcessor();
}