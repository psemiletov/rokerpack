#include "GrelkaProcessor.h"
#include "GrelkaEditor.h"

GrelkaAudioProcessor::GrelkaAudioProcessor()
     : AudioProcessor (BusesProperties().withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
       apvts (*this, nullptr, "Parameters", {
           std::make_unique<juce::AudioParameterFloat> ("drive", "Drive", 0.01f, 1.0f, 0.16f),
           std::make_unique<juce::AudioParameterFloat> ("level", "Level", -16.0f, 32.0f, 16.0f),
           std::make_unique<juce::AudioParameterFloat> ("lows", "Lows", 40.0f, 1000.0f, 200.0f),
  //         std::make_unique<juce::AudioParameterFloat> ("treble", "Treble", 7000.0f, 16500.0f, 13000.0f)
              std::make_unique<juce::AudioParameterFloat> ("treble", "Treble", 500.0f, 16500.0f, 13000.0f)
       })
{
}

GrelkaAudioProcessor::~GrelkaAudioProcessor()
{
}

void GrelkaAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    this->sampleRate = (float) sampleRate;
    
    if (!dBTableInitialized)
    {
        init_db();
        dBTableInitialized = true;
    }
    
    for (int i = 0; i < 2; ++i)
    {
        lp[i].setSampleRate(sampleRate);
        hp[i].setSampleRate(sampleRate);
        lp[i].reset();
        hp[i].reset();
    }
    
    updateParameters();
}

void GrelkaAudioProcessor::releaseResources()
{
    for (int i = 0; i < 2; ++i)
    {
        lp[i].reset();
        hp[i].reset();
    }
}

void GrelkaAudioProcessor::updateParameters()
{
    float lowsValue = apvts.getRawParameterValue("lows")->load();
    float trebleValue = apvts.getRawParameterValue("treble")->load();
    
    for (int i = 0; i < 2; ++i)
    {
        lp[i].setCutoff(trebleValue);
        hp[i].setCutoff(lowsValue);
    }
}

void GrelkaAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    float driveValue = apvts.getRawParameterValue("drive")->load();
    float levelValue = apvts.getRawParameterValue("level")->load();
    
    // Обновляем фильтры перед обработкой
    updateParameters();
    
    float levelLin = db2lin(levelValue);
    
    int numChannels = juce::jmin(2, buffer.getNumChannels());
    int numSamples = buffer.getNumSamples();
    
    for (int channel = 0; channel < numChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        SimpleLPF& lpFilter = lp[channel];
        SimpleHPF& hpFilter = hp[channel];
        
        for (int i = 0; i < numSamples; ++i)
        {
            float f = channelData[i];
            
            f = overdrive(f, driveValue, levelLin);
            f = lpFilter.process(f);
            f = hpFilter.process(f);
            
            if (f > 1.0f) f = 1.0f;
            else if (f < -1.0f) f = -1.0f;
            
            channelData[i] = f;
        }
    }
}

void GrelkaAudioProcessor::processBlock (juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(buffer, midiMessages);
}

juce::AudioProcessorEditor* GrelkaAudioProcessor::createEditor()
{
    return new GrelkaAudioEditor (*this);
}

bool GrelkaAudioProcessor::hasEditor() const
{
    return true;
}

const juce::String GrelkaAudioProcessor::getName() const
{
    return "Grelka";
}

bool GrelkaAudioProcessor::acceptsMidi() const
{
    return false;
}

bool GrelkaAudioProcessor::producesMidi() const
{
    return false;
}

double GrelkaAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GrelkaAudioProcessor::getNumPrograms()
{
    return 1;
}

int GrelkaAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GrelkaAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused(index);
}

const juce::String GrelkaAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused(index);
    return {};
}

void GrelkaAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void GrelkaAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void GrelkaAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    if (xml != nullptr)
    {
        apvts.replaceState (juce::ValueTree::fromXml (*xml));
        updateParameters();
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GrelkaAudioProcessor();
}