#include "BronzaProcessor.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "public.sdk/source/vst/vstaudioprocessoralgo.h"
#include "BronzaController.h"
#include "dsp.h"
#include "fx-resofilter.h"

using namespace Steinberg;
using namespace Steinberg::Vst;

BronzaProcessor::BronzaProcessor() 
    : fLevel(29.0f / 48.0f)    // 29 dB из 48 (0.604)
    , fIntensity(0.87f)         // 0.87
    , sampleRate(44100.0f)
{
    setControllerClass(BronzaControllerUID);
}

tresult PLUGIN_API BronzaProcessor::initialize(FUnknown* context) {
    tresult result = AudioEffect::initialize(context);
    if (result != kResultOk) return result;

    // Добавляем стерео шины
    addAudioInput(STR16("Stereo In"), SpeakerArr::kStereo);
    addAudioOutput(STR16("Stereo Out"), SpeakerArr::kStereo);

    return kResultOk;
}

tresult PLUGIN_API BronzaProcessor::terminate() {
    return AudioEffect::terminate();
}

tresult PLUGIN_API BronzaProcessor::setupProcessing(ProcessSetup& setup) {
    sampleRate = (float)setup.sampleRate;
    
    // Инициализация таблиц db (из dsp.cpp)
    init_db();

    // Инициализируем фильтры для левого и правого канала
    for (int i = 0; i < 2; i++) {
        lp[i].set_cutoff(12000.0f / sampleRate);
        lp[i].mode = FILTER_MODE_LOWPASS;
        lp[i].reset();

        hp[i].set_cutoff(500.0f / sampleRate);
        hp[i].mode = FILTER_MODE_HIGHPASS;
        hp[i].reset();
    }
    
    return kResultOk;
}

tresult PLUGIN_API BronzaProcessor::setActive(TBool state) {
    if (state) {
        // Сбрасываем фильтры при активации
        for (int i = 0; i < 2; i++) {
            lp[i].reset();
            hp[i].reset();
        }
    }
    return AudioEffect::setActive(state);
}

tresult PLUGIN_API BronzaProcessor::process(ProcessData& data) {
    // 1. Обработка автоматизации параметров
    if (data.inputParameterChanges) {
        int32 numActualChanges = data.inputParameterChanges->getParameterCount();
        for (int32 i = 0; i < numActualChanges; i++) {
            IParamValueQueue* queue = data.inputParameterChanges->getParameterData(i);
            if (queue) {
                ParamValue value;
                int32 sampleOffset;
                int32 numPoints = queue->getPointCount();
                
                // Берем последнее значение в блоке
                if (queue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
                    switch (queue->getParameterId()) {
                        case 0: // Level
                            fLevel = (float)value; 
                            break;
                        case 1: // Fuzz
                            fIntensity = (float)value;
                            break;
                    }
                }
            }
        }
    }

    // 2. Проверка буферов
    if (data.numInputs == 0 || data.numOutputs == 0 || data.inputs[0].numChannels == 0) {
        return kResultOk;
    }

    // 3. Получаем количество каналов
    int32 numChannels = data.inputs[0].numChannels;
    int32 sampleFrames = data.numSamples;
    
    // Ограничиваем до 2 каналов (стерео)
    if (numChannels > 2) numChannels = 2;

    // 4. Предварительно вычисляем линейный уровень
    float levelLin = db2lin(fLevel * 48.0f);
    float intensity = fIntensity;

    // 5. Обработка для каждого канала с отдельными фильтрами
    for (int32 channel = 0; channel < numChannels; channel++) {
        float* in  = data.inputs[0].channelBuffers32[channel];
        float* out = data.outputs[0].channelBuffers32[channel];
        
        // Получаем фильтры для этого канала
        CResoFilter& lpFilter = lp[channel];
        CResoFilter& hpFilter = hp[channel];
        
        // Основной цикл обработки
        for (int32 i = 0; i < sampleFrames; i++) {
            float f = in[i];

            // Вызов jimi_fuzz
            f = jimi_fuzz(f, levelLin, intensity);

            // Фильтрация
            f = lpFilter.process(f);
            f = hpFilter.process(f);

            // Hard Clipping
            if (f > 1.0f) 
                f = 1.0f;
            else if (f < -1.0f) 
                f = -1.0f;

            out[i] = f;
        }
    }

    return kResultOk;
}