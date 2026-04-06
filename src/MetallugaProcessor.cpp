#include "MetallugaProcessor.h"
#include "MetallugaEditor.h"

MetallugaAudioProcessor::MetallugaAudioProcessor()
     : AudioProcessor (BusesProperties().withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
       apvts (*this, nullptr, "Parameters", {
           std::make_unique<juce::AudioParameterFloat> ("drive", "Drive", 0.0f, 1.0f, 0.50f),
           std::make_unique<juce::AudioParameterFloat> ("level", "Level", -30.0f, 32.0f, 0.0f),
           std::make_unique<juce::AudioParameterFloat> ("weight", "Weight", 0.01f, 0.99f, 0.68f),
           std::make_unique<juce::AudioParameterFloat> ("reso", "Resonance", 0.01f, 0.99f, 0.50f),
           std::make_unique<juce::AudioParameterFloat> ("warmth", "Warmth", 0.01f, 0.99f, 0.50f)
       })
{
    // Убедимся, что apvts state инициализирован
    apvts.state.setProperty ("drive", 0.50f, nullptr);
    apvts.state.setProperty ("level", 0.0f, nullptr);
    apvts.state.setProperty ("weight", 0.68f, nullptr);
    apvts.state.setProperty ("reso", 0.50f, nullptr);
    apvts.state.setProperty ("warmth", 0.50f, nullptr);
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
    
    for (int i = 0; i < 2; ++i)
    {
        lp[i].mode = FILTER_MODE_LOWPASS;
        hp[i].mode = FILTER_MODE_HIGHPASS;
        lp[i].reset();
        hp[i].reset();
    }
    
    updateParameters();
}

void MetallugaAudioProcessor::releaseResources()
{
    for (int i = 0; i < 2; ++i)
    {
        lp[i].reset();
        hp[i].reset();
    }
}

void MetallugaAudioProcessor::updateParameters()
{
    // Получаем значения из apvts
    float weightValue = apvts.getRawParameterValue("weight")->load();
    float resoValue = apvts.getRawParameterValue("reso")->load();
    
    // Отладка
    std::cout << "updateParameters: weight=" << weightValue << ", reso=" << resoValue << std::endl;
    
    // Частота среза фильтров: 1 - weight
    float cutoff = 1.0f - weightValue;
    cutoff = juce::jlimit(0.01f, 0.99f, cutoff);
    
    for (int i = 0; i < 2; ++i)
    {
        lp[i].set_cutoff(cutoff);
        hp[i].set_cutoff(cutoff);
        lp[i].set_resonance(resoValue);
        hp[i].set_resonance(resoValue);
    }
}

void MetallugaAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    // Получаем параметры из apvts
    float driveValue = apvts.getRawParameterValue("drive")->load();
    float levelValue = apvts.getRawParameterValue("level")->load();
    float warmthValue = apvts.getRawParameterValue("warmth")->load();
    
    // Отладка (один раз)
    static bool logged = false;
    if (!logged)
    {
        std::cout << "processBlock: drive=" << driveValue << ", level=" << levelValue 
                  << ", warmth=" << warmthValue << std::endl;
        logged = true;
    }
    
    // Обновляем фильтры (параметры weight и reso)
    updateParameters();
    
    float levelLin = db2lin(levelValue);
    
    int numChannels = juce::jmin(2, buffer.getNumChannels());
    int numSamples = buffer.getNumSamples();
    
    for (int channel = 0; channel < numChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        CResoFilter& lpFilter = lp[channel];
        CResoFilter& hpFilter = hp[channel];
        
        for (int i = 0; i < numSamples; ++i)
        {
            float f = channelData[i];
            
            // Уровень применяется ДО искажения
            f *= levelLin;
            
            // Основное искажение
            f = gritty_guitar_distortion(f, driveValue);
            
            // Фильтрация
            f = lpFilter.process(f);
            f = hpFilter.process(f);
            
            // Резонанс (получаем свежее значение)
            float resoValue = apvts.getRawParameterValue("reso")->load();
            f = apply_resonance(f, resoValue);
            
            // Теплота
            f = warmify(f, warmthValue);
            
            // Hard clipping
            if (f > 1.0f) f = 1.0f;
            else if (f < -1.0f) f = -1.0f;
            
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
        updateParameters();
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MetallugaAudioProcessor();
}