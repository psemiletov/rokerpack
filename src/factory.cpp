#include "public.sdk/source/main/pluginfactory.h"
#include "BronzaProcessor.h"
#include "BronzaController.h"
#include "pluginterfaces/vst/ivstcomponent.h"
#include "pluginterfaces/vst/ivsteditcontroller.h"

using namespace Steinberg;
using namespace Steinberg::Vst;

SMTG_EXPORT_SYMBOL IPluginFactory* PLUGIN_API GetPluginFactory () {
    if (gPluginFactory) {
        gPluginFactory->addRef ();
        return gPluginFactory;
    }

    PFactoryInfo factoryInfo (
        "Bedroom Studio",
        "https://github.com/psemiletov/bronza",
        "peter.semiletov@gmail.com",
        PFactoryInfo::kNoFlags
    );
    
    auto* factory = new CPluginFactory (factoryInfo);

    PClassInfo2 processorInfo (
        BronzaProcessorUID.toTUID(),
        PClassInfo::kManyInstances,
        kVstAudioEffectClass,
        "Bronza",
        Vst::kDistributable,
        "Fx|Distortion",
        "1.0.0",
        kVstVersionString,
        "Bedroom Studio"
    );
    factory->registerClass (&processorInfo, BronzaProcessor::createInstance);

    PClassInfo2 controllerInfo (
        BronzaControllerUID.toTUID(),
        PClassInfo::kManyInstances,
        kVstComponentControllerClass,
        "BronzaController",
        0,
        "",
        "1.0.0",
        kVstVersionString,
        "Bedroom Studio"
    );
    factory->registerClass (&controllerInfo, BronzaController::createInstance);

    gPluginFactory = factory;
    return gPluginFactory;
}

#if SMTG_OS_LINUX
    SMTG_EXPORT_SYMBOL bool PLUGIN_API InitModule () {
        return true;
    }
    
    SMTG_EXPORT_SYMBOL bool PLUGIN_API DeinitModule () {
        return true;
    }
#endif