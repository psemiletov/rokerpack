#include "GuitarTunerProcessor.h"
#include "GuitarTunerEditor.h"
#include <cmath>
#include <iostream>

// Константы
constexpr float MAX_CENTS_DEVIATION = 50.0f;
constexpr int MIN_GUITAR_MIDI = 40;
constexpr int MAX_GUITAR_MIDI = 88;

GuitarTunerAudioProcessor::GuitarTunerAudioProcessor()
    : AudioProcessor (BusesProperties().withInput  ("Input",  juce::AudioChannelSet::mono(), true)
                                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
    , currentSampleRate (44100.0)
    , targetFrequency (0.0f)
    , centsDeviation (0.0f)
    , stringNumber (-1)
    , signalActive (false)
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
    
    pitchDetector = std::make_unique<SmartPitchDetector>();
    pitchDetector->prepare (sampleRate, samplesPerBlock);
    
    targetFrequency.store (0.0f);
    centsDeviation.store (0.0f);
    stringNumber.store (-1);
    signalActive.store (false);
    targetNote = "--";
}

void GuitarTunerAudioProcessor::releaseResources()
{
    pitchDetector.reset();
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

void GuitarTunerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    
    int numSamples = buffer.getNumSamples();
    
    if (numSamples > 0 && buffer.getNumChannels() > 0)
    {
        const float* channelData = buffer.getReadPointer (0);
        
        // Проверяем наличие сигнала (gate)
        gateDetector.processBlock (channelData, numSamples);
        bool hasSignal = gateDetector.isSignalPresent();
        signalActive.store (hasSignal);
        
        if (hasSignal)
        {
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
                        targetNote = STRINGS[closestString].noteName;
                        stringNumber.store (NUM_GUITAR_STRINGS - closestString);
                    }
                }
            }
        }
        else
        {
            // Нет сигнала — сбрасываем только detected, но не target
            centsDeviation.store (0.0f);
            signalActive.store (false);
        }
    }
    
    if (buffer.getNumChannels() > 1)
    {
        buffer.copyFrom (1, 0, buffer, 0, 0, numSamples);
    }
}

void GuitarTunerAudioProcessor::processBlock (juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(buffer, midiMessages);
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