#include "BassTunerProcessor.h"
#include "BassTunerEditor.h"
#include <cmath>
#include <iostream>

// Константы
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
    , detectedNote ("--")
    , targetNote ("--")
{
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
    
    pitchDetector = std::make_unique<SmartPitchDetector>();
    pitchDetector->prepare (sampleRate, samplesPerBlock);
    
    targetFrequency.store (0.0f);
    
    {
        juce::ScopedLock lock (stringDataLock);
        detectedNote = "--";
    }
}

void BassTunerAudioProcessor::releaseResources()
{
    pitchDetector.reset();
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

void BassTunerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    
    int numSamples = buffer.getNumSamples();
    
    if (numSamples > 0 && buffer.getNumChannels() > 0)
    {
        const float* channelData = buffer.getReadPointer (0);
        
        pitchDetector->processSamples (channelData, numSamples);
        
        if (pitchDetector->isNoteDetected())
        {
            float frequency = pitchDetector->getCurrentFrequency();
            float confidence = pitchDetector->getConfidence();
            
            if (frequency > 0.0f && confidence > 0.5f)
            {
                int closestString = findClosestString (frequency);
                if (closestString >= 0)
                {
                    float targetFreq = STRINGS[closestString].frequency;
                    float cents = 1200.0f * std::log2 (frequency / targetFreq);
                    
                    targetFrequency.store (targetFreq);
                    centsDeviation.store (cents);
                    
                    {
                        juce::ScopedLock lock (stringDataLock);
                        targetNote = STRINGS[closestString].noteName;
                    }
                    
                    stringNumber.store (NUM_BASS_STRINGS - closestString);
                    
                    {
                        juce::ScopedLock lock (stringDataLock);
                        detectedNote = frequencyToNoteName (frequency);
                    }
                }
            }
        }
        else
        {
            // Не обнуляем targetNote и stringNumber при тишине
            {
                juce::ScopedLock lock (stringDataLock);
                detectedNote = "--";
                // targetNote не обнуляем
                // stringNumber не обнуляем
            }
        }
    }
    
    if (buffer.getNumChannels() > 1)
    {
        buffer.copyFrom (1, 0, buffer, 0, 0, numSamples);
    }
}

void BassTunerAudioProcessor::processBlock (juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(buffer, midiMessages);
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
    juce::ignoreUnused(data, sizeInBytes);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BassTunerAudioProcessor();
}