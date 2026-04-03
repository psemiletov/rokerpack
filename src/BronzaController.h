#pragma once

#include "public.sdk/source/vst/vsteditcontroller.h"
#include "pluginterfaces/vst/ivsteditcontroller.h"
#include <memory>

// UID Контроллера
static const Steinberg::FUID BronzaProcessorUID (0x5053454D, 0x42524F4E, 0x5A413130, 0x31303130);
static const Steinberg::FUID BronzaControllerUID (0x5053454D, 0x42524F4E, 0x5A413130, 0x31303131);

class BronzaController : public Steinberg::Vst::EditController {
public:
    BronzaController() {}
    ~BronzaController() {}
    
    static Steinberg::FUnknown* createInstance (void*) {
        return static_cast<Steinberg::Vst::IEditController*>(new BronzaController);
    }

    Steinberg::tresult PLUGIN_API initialize (Steinberg::FUnknown* context) override;
    Steinberg::tresult PLUGIN_API terminate () override;
    
    // Создание ImGui GUI
    Steinberg::IPlugView* PLUGIN_API createView (Steinberg::FIDString name) override;
    
    Steinberg::tresult PLUGIN_API getParamStringByValue (
        Steinberg::Vst::ParamID id, 
        Steinberg::Vst::ParamValue valueNormalized, 
        Steinberg::Vst::String128 string) override;

    // Макросы для VST3
    OBJ_METHODS(BronzaController, Steinberg::Vst::EditController)
    DEFINE_INTERFACES
        DEF_INTERFACE(Steinberg::Vst::IEditController)
    END_DEFINE_INTERFACES(Steinberg::Vst::EditController)
    DELEGATE_REFCOUNT(Steinberg::Vst::EditController)
};