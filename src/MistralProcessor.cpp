#include "MistralProcessor.h"
#include "MistralEditor.h"

MistralAudioProcessor::MistralAudioProcessor()
     : AudioProcessor (BusesProperties().withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
       apvts (*this, nullptr, "Parameters", {
           std::make_unique<juce::AudioParameterFloat> ("rate", "Rate", 0.0f, 1.0f, 0.5f),
           std::make_unique<juce::AudioParameterFloat> ("depth", "Depth", 0.0f, 1.0f, 0.5f),
           std::make_unique<juce::AudioParameterFloat> ("feedback", "Feedback", 0.0f, 0.9f, 0.4f)
       })
{
    apvts.state.setProperty ("rate", 0.5f, nullptr);
    apvts.state.setProperty ("depth", 0.5f, nullptr);
    apvts.state.setProperty ("feedback", 0.4f, nullptr);
}

MistralAudioProcessor::~MistralAudioProcessor()
{
}

void MistralAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    this->sampleRate = sampleRate;
    
    maxDelaySamples = (int)(0.05 * sampleRate);
    delayBuffer.setSize (2, maxDelaySamples + 1);
    delayBuffer.clear();
    writePosition = 0;
    phase = 0.0f;
}

void MistralAudioProcessor::releaseResources()
{
    delayBuffer.clear();
}
/*
void MistralAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    float rate = apvts.getRawParameterValue("rate")->load();
    float depth = apvts.getRawParameterValue("depth")->load();
    float feedback = apvts.getRawParameterValue("feedback")->load();
    
    // Плавное ограничение feedback
    feedback = juce::jmin(0.9f, feedback);
    
    float lfoFreq = 0.1f + rate * 4.9f;
    
    float minDelayMs = 0.5f;
    float maxDelayMs = 8.0f;
    float delayRangeMs = maxDelayMs - minDelayMs;
    
    int numSamples = buffer.getNumSamples();
    int numChannels = buffer.getNumChannels();
    
    for (int sample = 0; sample < numSamples; ++sample)
    {
        // Простая синусоидальная LFO (без лишних усложнений)
        float lfo = std::sin(phase);
        lfo = (lfo + 1.0f) / 2.0f;
        
        phase += 2.0f * juce::MathConstants<float>::pi * lfoFreq / sampleRate;
        if (phase >= juce::MathConstants<float>::twoPi)
            phase -= juce::MathConstants<float>::twoPi;
        
        for (int channel = 0; channel < numChannels; ++channel)
        {
            float delayMs = minDelayMs + lfo * depth * delayRangeMs;
            float delaySamples = delayMs * sampleRate / 1000.0f;
            
            float* channelData = buffer.getWritePointer(channel);
            float input = channelData[sample];
            
            float readPos = (float)writePosition - delaySamples;
            if (readPos < 0) readPos += maxDelaySamples;
            
            int readPosInt = (int)readPos;
            float frac = readPos - readPosInt;
            int readPosNext = readPosInt + 1;
            if (readPosNext >= maxDelaySamples) readPosNext = 0;
            
            float delayed = delayBuffer.getSample(channel, readPosInt) * (1.0f - frac) +
                            delayBuffer.getSample(channel, readPosNext) * frac;
            
            // Feedback с мягким ограничением (без лишних фильтров)
            float writeValue = input + delayed * feedback;
            writeValue = juce::jlimit(-1.0f, 1.0f, writeValue);
            delayBuffer.setSample(channel, writePosition, writeValue);
            
            // Чистый WET/DRY микс
            float output = input * (1.0f - depth) + delayed * depth;
            
            channelData[sample] = output;
        }
        
        writePosition++;
        if (writePosition >= maxDelaySamples)
            writePosition = 0;
    }
}
*/

/*
void MistralAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    float targetRate = apvts.getRawParameterValue("rate")->load();
    float targetDepth = apvts.getRawParameterValue("depth")->load();
    float targetFeedback = apvts.getRawParameterValue("feedback")->load();
    
    targetFeedback = juce::jmin(0.9f, targetFeedback);
    
    // Инициализация сглаженных параметров при первом вызове
    static float smoothRate = targetRate;
    static float smoothDepth = targetDepth;
    static float smoothFeedback = targetFeedback;
    
    // Коэффициент сглаживания (чем меньше, тем плавнее, но медленнее реакция)
    const float smoothing = 0.995f;  // Быстрая, но плавная реакция (~1 мс)
    
    // Плавное обновление параметров
    smoothRate = smoothRate * smoothing + targetRate * (1.0f - smoothing);
    smoothDepth = smoothDepth * smoothing + targetDepth * (1.0f - smoothing);
    smoothFeedback = smoothFeedback * smoothing + targetFeedback * (1.0f - smoothing);
    
    float rate = smoothRate;
    float depth = smoothDepth;
    float feedback = smoothFeedback;
    
    float lfoFreq = 0.1f + rate * 4.9f;
    
    float minDelayMs = 0.5f;
    float maxDelayMs = 8.0f;
    float delayRangeMs = maxDelayMs - minDelayMs;
    
    int numSamples = buffer.getNumSamples();
    int numChannels = buffer.getNumChannels();
    
    for (int sample = 0; sample < numSamples; ++sample)
    {
        float lfo = std::sin(phase);
        lfo = (lfo + 1.0f) / 2.0f;
        
        phase += 2.0f * juce::MathConstants<float>::pi * lfoFreq / sampleRate;
        if (phase >= juce::MathConstants<float>::twoPi)
            phase -= juce::MathConstants<float>::twoPi;
        
        for (int channel = 0; channel < numChannels; ++channel)
        {
            float delayMs = minDelayMs + lfo * depth * delayRangeMs;
            float delaySamples = delayMs * sampleRate / 1000.0f;
            
            float* channelData = buffer.getWritePointer(channel);
            float input = channelData[sample];
            
            float readPos = (float)writePosition - delaySamples;
            if (readPos < 0) readPos += maxDelaySamples;
            
            int readPosInt = (int)readPos;
            float frac = readPos - readPosInt;
            int readPosNext = readPosInt + 1;
            if (readPosNext >= maxDelaySamples) readPosNext = 0;
            
            float delayed = delayBuffer.getSample(channel, readPosInt) * (1.0f - frac) +
                            delayBuffer.getSample(channel, readPosNext) * frac;
            
            float writeValue = input + delayed * feedback;
            writeValue = juce::jlimit(-1.0f, 1.0f, writeValue);
            delayBuffer.setSample(channel, writePosition, writeValue);
            
            float output = input * (1.0f - depth) + delayed * depth;
            
            channelData[sample] = output;
        }
        
        writePosition++;
        if (writePosition >= maxDelaySamples)
            writePosition = 0;
    }
}
*/

void MistralAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    float targetRate = apvts.getRawParameterValue("rate")->load();
    float targetDepth = apvts.getRawParameterValue("depth")->load();
    float targetFeedback = apvts.getRawParameterValue("feedback")->load();
    targetFeedback = juce::jmin(0.9f, targetFeedback);
    
    static float smoothRate = targetRate;
    static float smoothDepth = targetDepth;
    static float smoothFeedback = targetFeedback;
    
    // Плавное сглаживание (без адаптации для простоты)
    const float smoothing = 0.995f;
    smoothRate = smoothRate * smoothing + targetRate * (1.0f - smoothing);
    smoothDepth = smoothDepth * smoothing + targetDepth * (1.0f - smoothing);
    smoothFeedback = smoothFeedback * smoothing + targetFeedback * (1.0f - smoothing);
    
    float rate = smoothRate;
    float depth = smoothDepth;
    float feedback = smoothFeedback;
    
    float lfoFreq = 0.1f + rate * 4.9f;
    
    float minDelayMs = 0.5f;
    float maxDelayMs = 8.0f;
    float delayRangeMs = maxDelayMs - minDelayMs;
    
    int numSamples = buffer.getNumSamples();
    int numChannels = buffer.getNumChannels();
    
    // === КЛЮЧЕВОЕ ИЗМЕНЕНИЕ: интерполяция позиции чтения ===
    static float currentDelaySamples = 0.0f;
    
    for (int sample = 0; sample < numSamples; ++sample)
    {
        float lfo = std::sin(phase);
        lfo = (lfo + 1.0f) / 2.0f;
        
        phase += 2.0f * juce::MathConstants<float>::pi * lfoFreq / sampleRate;
        if (phase >= juce::MathConstants<float>::twoPi)
            phase -= juce::MathConstants<float>::twoPi;
        
        // Целевая задержка на основе текущего depth
        float targetDelaySamples = (minDelayMs + lfo * depth * delayRangeMs) * sampleRate / 1000.0f;
        
        // Плавная интерполяция позиции чтения (устраняет треск!)
        currentDelaySamples = currentDelaySamples * 0.99f + targetDelaySamples * 0.01f;
        
        for (int channel = 0; channel < numChannels; ++channel)
        {
            float* channelData = buffer.getWritePointer(channel);
            float input = channelData[sample];
            
            float readPos = (float)writePosition - currentDelaySamples;
            if (readPos < 0) readPos += maxDelaySamples;
            
            int readPosInt = (int)readPos;
            float frac = readPos - readPosInt;
            int readPosNext = readPosInt + 1;
            if (readPosNext >= maxDelaySamples) readPosNext = 0;
            
            float delayed = delayBuffer.getSample(channel, readPosInt) * (1.0f - frac) +
                            delayBuffer.getSample(channel, readPosNext) * frac;
            
            float writeValue = input + delayed * feedback;
            writeValue = juce::jlimit(-1.0f, 1.0f, writeValue);
            delayBuffer.setSample(channel, writePosition, writeValue);
            
            float output = input * (1.0f - depth) + delayed * depth;
            
            channelData[sample] = output;
        }
        
        writePosition++;
        if (writePosition >= maxDelaySamples)
            writePosition = 0;
    }
}


void MistralAudioProcessor::processBlock (juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(buffer, midiMessages);
}

juce::AudioProcessorEditor* MistralAudioProcessor::createEditor()
{
    return new MistralAudioEditor (*this);
}

bool MistralAudioProcessor::hasEditor() const
{
    return true;
}

const juce::String MistralAudioProcessor::getName() const
{
    return "Mistral";
}

bool MistralAudioProcessor::acceptsMidi() const
{
    return false;
}

bool MistralAudioProcessor::producesMidi() const
{
    return false;
}

double MistralAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MistralAudioProcessor::getNumPrograms()
{
    return 1;
}

int MistralAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MistralAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused(index);
}

const juce::String MistralAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused(index);
    return {};
}

void MistralAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void MistralAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void MistralAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    if (xml != nullptr)
    {
        apvts.replaceState (juce::ValueTree::fromXml (*xml));
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MistralAudioProcessor();
}