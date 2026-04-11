#include "BassTunerProcessor.h"
#include "BassTunerEditor.h"
#include <cmath>

BassTunerAudioProcessor::BassTunerAudioProcessor()
    : AudioProcessor (BusesProperties().withInput  ("Input",  juce::AudioChannelSet::mono(), true)
                                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
    , currentSampleRate (44100.0)
    , currentBlockSize (512)
    , detectedFrequency (0.0f)
    , targetFrequency (0.0f)
    , centsDeviation (0.0f)
    , detectedNote ("--")
    , targetNote ("--")
    , stringNumber (-1)
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

/*
void BassTunerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
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
}
*/

void BassTunerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlock;
    
    // Увеличиваем буфер для E1 (4096 сэмплов)
    int analysisBlockSize = 4096;
    pitchDetector = std::make_unique<PitchDetector> (sampleRate, analysisBlockSize);
    
    detectedFrequency = 0.0f;
    
    {
        juce::ScopedLock lock (stringDataLock);
        detectedNote = "--";
        targetNote = "--";
        stringNumber = -1;
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
        
        float frequency = pitchDetector->getFrequency (channelData, numSamples);
        
        if (frequency > 0.0f)
        {
            detectedFrequency = frequency;
            
            int closestString = findClosestString (frequency);
            if (closestString >= 0)
            {
                targetFrequency = STRINGS[closestString].frequency;
                targetNote = STRINGS[closestString].noteName;
                stringNumber = 4 - closestString;
            }
            
            centsDeviation = calculateCents (detectedFrequency, targetFrequency);
            
            {
                juce::ScopedLock lock (stringDataLock);
                detectedNote = frequencyToNoteName (frequency);
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
    
    if (buffer.getNumChannels() > 1)
    {
        buffer.copyFrom (1, 0, buffer, 0, 0, numSamples);
    }
}

int BassTunerAudioProcessor::findClosestString (float frequency) const
{
    if (frequency <= 0.0f)
        return -1;
    
    int closestIndex = 0;
    float minDiff = std::abs (frequency - STRINGS[0].frequency);
    
    for (int i = 1; i < 4; ++i)
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
    
    return juce::jlimit (-50.0f, 50.0f, cents);
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
    midiNote = juce::jlimit (28, 55, midiNote);
    
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