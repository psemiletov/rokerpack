#include "TembrProcessor.h"
#include "TembrEditor.h"

TembrAudioProcessor::TembrAudioProcessor()
     : AudioProcessor (BusesProperties().withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
       apvts (*this, nullptr, "Parameters", {
           std::make_unique<juce::AudioParameterFloat> ("lows", "Lows", -12.0f, 12.0f, 0.0f),
           std::make_unique<juce::AudioParameterFloat> ("treble", "Treble", -12.0f, 12.0f, 0.0f)
       })
{
}

TembrAudioProcessor::~TembrAudioProcessor()
{
}

void TembrAudioProcessor::prepareToPlay (double sRate, int samplesPerBlock)
{
    sampleRate = (float) sRate;
    juce::ignoreUnused(samplesPerBlock);
    
    eqL.setSampleRate(sampleRate);
    eqR.setSampleRate(sampleRate);
    eqL.reset();
    eqR.reset();
}

void TembrAudioProcessor::releaseResources()
{
}

void TembrAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    juce::ignoreUnused(midiMessages);
    
    float lowsValue = apvts.getRawParameterValue("lows")->load();
    float trebleValue = apvts.getRawParameterValue("treble")->load();
    
    eqL.setLowsGain(lowsValue);
    eqL.setTrebleGain(trebleValue);
    
    eqR.setLowsGain(lowsValue);
    eqR.setTrebleGain(trebleValue);
    
    int numChannels = juce::jmin(2, buffer.getNumChannels());
    int numSamples = buffer.getNumSamples();
    
    for (int channel = 0; channel < numChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        VintageEQ& eq = (channel == 0) ? eqL : eqR;
        
        for (int i = 0; i < numSamples; ++i)
        {
            float f = channelData[i];
            f = eq.process(f);
            
            // Безопасный клиппинг (на всякий случай)
            if (f > 1.0f) f = 1.0f;
            else if (f < -1.0f) f = -1.0f;
            
            channelData[i] = f;
        }
    }
}

void TembrAudioProcessor::processBlock (juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(buffer, midiMessages);
}

juce::AudioProcessorEditor* TembrAudioProcessor::createEditor()
{
    return new TembrAudioEditor (*this);
}

bool TembrAudioProcessor::hasEditor() const
{
    return true;
}

const juce::String TembrAudioProcessor::getName() const
{
    return "Tembr";
}

bool TembrAudioProcessor::acceptsMidi() const
{
    return false;
}

bool TembrAudioProcessor::producesMidi() const
{
    return false;
}

double TembrAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TembrAudioProcessor::getNumPrograms()
{
    return 1;
}

int TembrAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TembrAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused(index);
}

const juce::String TembrAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused(index);
    return {};
}

void TembrAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void TembrAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void TembrAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    if (xml != nullptr)
    {
        apvts.replaceState (juce::ValueTree::fromXml (*xml));
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TembrAudioProcessor();
}