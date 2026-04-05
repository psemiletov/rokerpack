#include "BronzaProcessor.h"
#include "BronzaEditor.h"

//==============================================================================
BronzaAudioProcessor::BronzaAudioProcessor()
     : AudioProcessor (BusesProperties().withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
{
    // Создаём параметры в диапазоне 0..1, как в VST версии
    levelParam = new juce::AudioParameterFloat ("level", "Level", 
                                                 juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 
                                                 fLevel);
    
    intensityParam = new juce::AudioParameterFloat ("intensity", "Intensity",
                                                     juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f),
                                                     fIntensity);
    
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
    
    // Инициализируем таблицу dB (один раз)
    if (!dBTableInitialized)
    {
        init_db();
        dBTableInitialized = true;
    }
    
    // Инициализируем фильтры как в оригинальном setupProcessing
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
    // Сброс фильтров
    for (int i = 0; i < 2; i++)
    {
        lp[i].reset();
        hp[i].reset();
    }
}

void BronzaAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    // Получаем параметры
    fLevel = levelParam->get();
    fIntensity = intensityParam->get();
    
    // Предварительно вычисляем линейный уровень (0..1 -> 0..48 dB)
    float levelLin = db2lin(fLevel * 48.0f);
    float intensity = fIntensity;
    
    // Получаем каналы
    int numChannels = juce::jmin(2, buffer.getNumChannels());
    int numSamples = buffer.getNumSamples();
    
    // Обработка для каждого канала
    for (int channel = 0; channel < numChannels; channel++)
    {
        float* channelData = buffer.getWritePointer(channel);
        
        // Получаем фильтры для этого канала (теперь это обычные объекты)
        CResoFilter& lpFilter = lp[channel];
        CResoFilter& hpFilter = hp[channel];
        
        // Основной цикл обработки
        for (int i = 0; i < numSamples; i++)
        {
            float f = channelData[i];
            
            // Jimi fuzz
            f = jimi_fuzz(f, levelLin, intensity);
            
            // Фильтрация
            f = lpFilter.process(f);
            f = hpFilter.process(f);
            
            // Hard Clipping
            if (f > 1.0f) 
                f = 1.0f;
            else if (f < -1.0f) 
                f = -1.0f;
            
            channelData[i] = f;
        }
    }
}

// Перегрузка для double (чтобы убрать предупреждение)
void BronzaAudioProcessor::processBlock (juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    // Просто игнорируем double-версию или можно сконвертировать во float
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
    xml->setAttribute ("level", (double) fLevel);
    xml->setAttribute ("intensity", (double) fIntensity);
    copyXmlToBinary (*xml, destData);
}

void BronzaAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName ("BronzaSettings"))
    {
        fLevel = (float) xml->getDoubleAttribute ("level", 29.0f / 48.0f);
        fIntensity = (float) xml->getDoubleAttribute ("intensity", 0.87f);
        
        if (levelParam != nullptr)
            levelParam->setValueNotifyingHost (fLevel);
        if (intensityParam != nullptr)
            intensityParam->setValueNotifyingHost (fIntensity);
    }
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BronzaAudioProcessor();
}