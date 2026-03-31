#pragma once

#include "public.sdk/source/vst/vstaudioeffect.h"
#include "pluginterfaces/vst/ivsteditcontroller.h"
#include "dsp.h"
#include "fx-resofilter.h"

class BronzaProcessor : public Steinberg::Vst::AudioEffect
{
public:
    BronzaProcessor();
    ~BronzaProcessor() = default;
    
    static Steinberg::FUnknown* createInstance (void* context) {
        return static_cast<Steinberg::Vst::IAudioProcessor*>(new BronzaProcessor);
    }

    Steinberg::tresult PLUGIN_API initialize (Steinberg::FUnknown* context) override;
    Steinberg::tresult PLUGIN_API terminate () override;
    Steinberg::tresult PLUGIN_API setupProcessing (Steinberg::Vst::ProcessSetup& setup) override;
    Steinberg::tresult PLUGIN_API process (Steinberg::Vst::ProcessData& data) override;
    Steinberg::tresult PLUGIN_API setActive (Steinberg::TBool state) override;
    
    Steinberg::tresult PLUGIN_API setState (Steinberg::IBStream* state) override {
        return Steinberg::kResultOk;
    }
    
    Steinberg::tresult PLUGIN_API getState (Steinberg::IBStream* state) override {
        return Steinberg::kResultOk;
    }

    OBJ_METHODS(BronzaProcessor, Steinberg::Vst::AudioEffect)
    DEFINE_INTERFACES
        DEF_INTERFACE(Steinberg::Vst::IAudioProcessor)
        DEF_INTERFACE(Steinberg::Vst::IComponent)
    END_DEFINE_INTERFACES(Steinberg::Vst::AudioEffect)
    DELEGATE_REFCOUNT(Steinberg::Vst::AudioEffect)

protected:
    // Фильтры для каждого канала (левый и правый)
    CResoFilter lp[2];
    CResoFilter hp[2];
    
    // Параметры
    float fLevel = 0.0f;      // Нормализованное значение 0..1
    float fIntensity = 0.0f;  // Нормализованное значение 0..1
    float sampleRate = 44100.0f;
};