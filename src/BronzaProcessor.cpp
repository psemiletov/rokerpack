#include "BronzaProcessor.h"
#include "BronzaEditor.h"

//==============================================================================
BronzaAudioProcessor::BronzaAudioProcessor()
     : AudioProcessor (BusesProperties().withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
{
    // Создаём параметры
    levelParam = new juce::AudioParameterFloat ("level", "Level", 
                                                 juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 
                                                 0.5f);
    intensityParam = new juce::AudioParameterFloat ("intensity", "Intensity",
                                                     juce::NormalisableRange<float> (0.001f, 1.0f, 0.001f),
                                                     0.87f);
    
    addParameter (levelParam);
    addParameter (intensityParam);
}

BronzaAudioProcessor::~BronzaAudioProcessor()
{
}

//==============================================================================
void BronzaAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    this->sampleRate = (float) sampleRate;
    juce::ignoreUnused(samplesPerBlock);
    
    // Инициализируем таблицу dB (один раз)
    if (!dBTableInitialized)
    {
        init_db();
        dBTableInitialized = true;
    }
    
    // Инициализируем фильтры
    for (int i = 0; i < 2; i++)
    {
        lp[i].set_cutoff(12000.0f / (float) sampleRate);
        lp[i].mode = FILTER_MODE_LOWPASS;
        lp[i].reset();

        hp[i].set_cutoff(500.0f / (float) sampleRate);
        hp[i].mode = FILTER_MODE_HIGHPASS;
        hp[i].reset();
    }
}

void BronzaAudioProcessor::releaseResources()
{
    for (int i = 0; i < 2; i++)
    {
        lp[i].reset();
        hp[i].reset();
    }
}

void BronzaAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    juce::ignoreUnused(midiMessages);
    
    float fLevel = levelParam->get();
    float fIntensity = intensityParam->get();
    
    float levelLin = db2lin(fLevel * 48.0f);
    
    int numChannels = juce::jmin(2, buffer.getNumChannels());
    int numSamples = buffer.getNumSamples();
    
    for (int channel = 0; channel < numChannels; channel++)
    {
        float* channelData = buffer.getWritePointer(channel);
        CResoFilter& lpFilter = lp[channel];
        CResoFilter& hpFilter = hp[channel];
        
        for (int i = 0; i < numSamples; i++)
        {
            float f = channelData[i];
            
            f = jimi_fuzz(f, levelLin, fIntensity);
            f = lpFilter.process(f);
            f = hpFilter.process(f);
            
            channelData[i] = f;
        }
    }
}

void BronzaAudioProcessor::processBlock (juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(buffer, midiMessages);
}

//==============================================================================
juce::AudioProcessorEditor* BronzaAudioProcessor::createEditor()
{
    return new BronzaAudioEditor (*this);
}

bool BronzaAudioProcessor::hasEditor() const
{
    return true;
}

//==============================================================================
const juce::String BronzaAudioProcessor::getName() const
{
    return "Bronza";
}

bool BronzaAudioProcessor::acceptsMidi() const
{
    return false;
}

bool BronzaAudioProcessor::producesMidi() const
{
    return false;
}

double BronzaAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

//==============================================================================
int BronzaAudioProcessor::getNumPrograms()
{
    return 1;
}

int BronzaAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BronzaAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused(index);
}

const juce::String BronzaAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused(index);
    return {};
}

void BronzaAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void BronzaAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    std::unique_ptr<juce::XmlElement> xml (new juce::XmlElement ("BronzaSettings"));
    xml->setAttribute ("level", (double) levelParam->get());
    xml->setAttribute ("intensity", (double) intensityParam->get());
    copyXmlToBinary (*xml, destData);
}

void BronzaAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName ("BronzaSettings"))
    {
        float level = (float) xml->getDoubleAttribute ("level", 0.5f);
        float intensity = (float) xml->getDoubleAttribute ("intensity", 0.87f);
        
        levelParam->setValueNotifyingHost (level);
        intensityParam->setValueNotifyingHost (intensity);
    }
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BronzaAudioProcessor();
}