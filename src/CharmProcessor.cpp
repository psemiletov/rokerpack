#include "CharmProcessor.h"
#include "CharmEditor.h"

CharmAudioProcessor::CharmAudioProcessor()
     : AudioProcessor (BusesProperties().withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
       apvts (*this, nullptr, "Parameters", {
           std::make_unique<juce::AudioParameterFloat> ("charm", "Charm", 0.0f, 1.0f, 0.50f)
       })
{
//    apvts.state.setProperty ("charm", 0.50f, nullptr);
}

CharmAudioProcessor::~CharmAudioProcessor()
{
}

void CharmAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    this->sampleRate = (float) sampleRate;
    
  /*  if (!dBTableInitialized)
    {
        init_db();
        dBTableInitialized = true;
    }*/
}

void CharmAudioProcessor::releaseResources()
{
}

void CharmAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    float charmValue = apvts.getRawParameterValue("charm")->load();
    
    int numChannels = juce::jmin(2, buffer.getNumChannels());
    int numSamples = buffer.getNumSamples();
    
    for (int channel = 0; channel < numChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        
        for (int i = 0; i < numSamples; ++i)
        {
            float f = channelData[i];
            f = warmify(f, charmValue);
            
            // Soft clipping (как в оригинале warmify уже делает clipping)
           // if (f > 1.0f) f = 1.0f;
            //else if (f < -1.0f) f = -1.0f;
            
            channelData[i] = f;
        }
    }
}

void CharmAudioProcessor::processBlock (juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(buffer, midiMessages);
}

juce::AudioProcessorEditor* CharmAudioProcessor::createEditor()
{
    return new CharmAudioEditor (*this);
}

bool CharmAudioProcessor::hasEditor() const
{
    return true;
}

const juce::String CharmAudioProcessor::getName() const
{
    return "Charm";
}

bool CharmAudioProcessor::acceptsMidi() const
{
    return false;
}

bool CharmAudioProcessor::producesMidi() const
{
    return false;
}

double CharmAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CharmAudioProcessor::getNumPrograms()
{
    return 1;
}

int CharmAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CharmAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused(index);
}

const juce::String CharmAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused(index);
    return {};
}

void CharmAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void CharmAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void CharmAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    if (xml != nullptr)
    {
        apvts.replaceState (juce::ValueTree::fromXml (*xml));
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CharmAudioProcessor();
}