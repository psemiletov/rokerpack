#include "BronzaController.h"
#include "pluginterfaces/base/ustring.h"

using namespace Steinberg;
using namespace Steinberg::Vst;

tresult PLUGIN_API BronzaController::initialize (FUnknown* context) {
    tresult result = EditController::initialize (context);
    if (result != kResultOk) return result;

    parameters.addParameter (STR16 ("Level"), STR16 ("dB"), 0, 
                             29.0 / 48.0, 
                             ParameterInfo::kCanAutomate, 0);

    parameters.addParameter (STR16 ("Fuzz"), STR16 ("%"), 0, 
                             0.87, 
                             ParameterInfo::kCanAutomate, 1);

    return kResultOk;
}

tresult PLUGIN_API BronzaController::terminate () {
    return EditController::terminate ();
}

tresult PLUGIN_API BronzaController::getParamStringByValue (ParamID id, ParamValue valueNormalized, String128 string) {
    if (id == 0) {
        float db = (float)valueNormalized * 48.0f;
        UString128 (string).printFloat (db, 1);
        return kResultTrue;
    }
    
    if (id == 1) {
        UString128 (string).printFloat ((float)valueNormalized * 100.0f, 1);
        return kResultTrue;
    }

    return EditController::getParamStringByValue (id, valueNormalized, string);
}