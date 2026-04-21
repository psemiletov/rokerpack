#include "MetallugaProcessor.h"
#include "MetallugaEditor.h"

MetallugaAudioProcessor::MetallugaAudioProcessor()
     : AudioProcessor (BusesProperties().withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
       apvts (*this, nullptr, "Parameters", {
           std::make_unique<juce::AudioParameterFloat> ("gate", "Gate", 
               juce::NormalisableRange<float> (0.0f, 0.05f, 0.0001f), 0.005f),
           std::make_unique<juce::AudioParameterFloat> ("drive", "Drive", 0.0f, 1.0f, 0.50f),
           std::make_unique<juce::AudioParameterFloat> ("level", "Level", 0.0f, 32.0f, 0.0f)
       })
{
}

MetallugaAudioProcessor::~MetallugaAudioProcessor()
{
}

void MetallugaAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    this->sampleRate = (float) sampleRate;
    
    if (!dBTableInitialized)
    {
        init_db();
        dBTableInitialized = true;
    }
}

void MetallugaAudioProcessor::releaseResources()
{
}

void MetallugaAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    float gateThreshold = apvts.getRawParameterValue("gate")->load();
    float driveValue = apvts.getRawParameterValue("drive")->load();
    float levelValue = apvts.getRawParameterValue("level")->load();
    
    float levelLin = db2lin(levelValue);
    
    float preGain = 1.0f + driveValue * 12.0f;
    float postGain = 0.3f + driveValue * 0.7f;
    
    int numChannels = juce::jmin(2, buffer.getNumChannels());
    int numSamples = buffer.getNumSamples();
    
    for (int channel = 0; channel < numChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        
        for (int i = 0; i < numSamples; ++i)
        {
            float f = channelData[i];
            
            // === GATE ===
            if (std::abs(f) < gateThreshold)
            {
                channelData[i] = 0.0f;
                continue;
            }
            
            // Усиление
            f = f * preGain;
            
            // Асимметричный клип
            if (f > 0.0f)
                f = 1.0f - expf(-f * 1.5f);
            else
                f = -1.0f + expf(f * 1.5f);
            
            f = f * postGain;
            f = tanh(f);
            f = f * levelLin;
            f = std::min(1.0f, std::max(-1.0f, f));
            
            channelData[i] = f;
        }
    }
}

void MetallugaAudioProcessor::processBlock (juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(buffer, midiMessages);
}

juce::AudioProcessorEditor* MetallugaAudioProcessor::createEditor()
{
    return new MetallugaAudioEditor (*this);
}

bool MetallugaAudioProcessor::hasEditor() const
{
    return true;
}

const juce::String MetallugaAudioProcessor::getName() const
{
    return "Metalluga";
}

bool MetallugaAudioProcessor::acceptsMidi() const
{
    return false;
}

bool MetallugaAudioProcessor::producesMidi() const
{
    return false;
}

double MetallugaAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MetallugaAudioProcessor::getNumPrograms()
{
    return 1;
}

int MetallugaAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MetallugaAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused(index);
}

const juce::String MetallugaAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused(index);
    return {};
}

void MetallugaAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void MetallugaAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void MetallugaAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    if (xml != nullptr)
    {
        apvts.replaceState (juce::ValueTree::fromXml (*xml));
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MetallugaAudioProcessor();
}