#pragma once

#include "public.sdk/source/vst/vsteditcontroller.h"
#include "pluginterfaces/base/funknown.h"



// UID Контроллера
static const Steinberg::FUID BronzaProcessorUID (0x5053454D, 0x42524F4E, 0x5A413130, 0x31303130);
static const Steinberg::FUID BronzaControllerUID (0x5053454D, 0x42524F4E, 0x5A413130, 0x31303131);


class BronzaPlugView;

class BronzaController : public Steinberg::Vst::EditController {
public:
    static Steinberg::FUnknown* createInstance(void* context) {
        return static_cast<Steinberg::Vst::IEditController*>(new BronzaController());
    }
    
    Steinberg::tresult PLUGIN_API initialize(Steinberg::FUnknown* context) override;
    Steinberg::tresult PLUGIN_API terminate() override;
    
    Steinberg::IPlugView* PLUGIN_API createView(Steinberg::FIDString name) override;
    
    Steinberg::tresult PLUGIN_API getParamStringByValue(Steinberg::Vst::ParamID id, 
                                                       Steinberg::Vst::ParamValue valueNormalized, 
                                                       Steinberg::Vst::String128 string) override;
    
    Steinberg::tresult PLUGIN_API getParamValueByString(Steinberg::Vst::ParamID id, 
                                                       const Steinberg::Vst::TChar* string, 
                                                       Steinberg::Vst::ParamValue& valueNormalized) override;
    
    Steinberg::tresult PLUGIN_API setComponentState(Steinberg::IBStream* state) override;
    
    OBJ_METHODS(BronzaController, Steinberg::Vst::EditController)
    DEFINE_INTERFACES
        DEF_INTERFACE(Steinberg::Vst::IEditController)
    END_DEFINE_INTERFACES(Steinberg::Vst::EditController)
    DELEGATE_REFCOUNT(Steinberg::Vst::EditController)

private:
    BronzaPlugView* currentView = nullptr;
};

// UIDs для параметров
//static const Steinberg::FUID BronzaControllerUID(0x12345678, 0x9ABCDEF0, 0x11111111, 0x22222222);