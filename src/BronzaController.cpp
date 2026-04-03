#include "BronzaController.h"
#include "BronzaPlugView.h"
#include "pluginterfaces/base/ustring.h"
#include <iostream>
#include <cmath>

using namespace Steinberg;
using namespace Steinberg::Vst;

tresult PLUGIN_API BronzaController::initialize(FUnknown* context) {
    try {
        tresult result = EditController::initialize(context);
        if (result != kResultOk) return result;

        // Параметр Level (0-1, нормализованный)
        parameters.addParameter(
            STR16("Level"),
            STR16("dB"),
            0,
            0.6f,  // default value
            ParameterInfo::kCanAutomate,
            0,     // Parameter ID
            STR16("Distortion"));

        // Параметр Fuzz (0-1, нормализованный)
        parameters.addParameter(
            STR16("Fuzz"),
            STR16("%"),
            0,
            0.87f, // default value
            ParameterInfo::kCanAutomate,
            1,     // Parameter ID
            STR16("Distortion"));

        std::cout << "[Bronza] Controller initialized" << std::endl;
        return kResultOk;
    } catch (...) {
        std::cerr << "[Bronza] Exception in controller initialize()" << std::endl;
        return kResultFalse;
    }
}

tresult PLUGIN_API BronzaController::terminate() {
    try {
        return EditController::terminate();
    } catch (...) {
        std::cerr << "[Bronza] Exception in controller terminate()" << std::endl;
        return kResultFalse;
    }
}

IPlugView* PLUGIN_API BronzaController::createView(FIDString name) {
    try {
        if (strcmp(name, "editor") != 0) {
            return nullptr;
        }

        std::cout << "[Bronza] Creating editor GUI" << std::endl;
        
        BronzaPlugView* plugView = new BronzaPlugView(this);
        plugView->addRef();
        currentView = plugView;
        return plugView;
        
    } catch (const std::exception& e) {
        std::cerr << "[Bronza] Exception in createView: " << e.what() << std::endl;
        return nullptr;
    } catch (...) {
        std::cerr << "[Bronza] Unknown exception in createView" << std::endl;
        return nullptr;
    }
}

tresult PLUGIN_API BronzaController::getParamStringByValue(ParamID id, ParamValue valueNormalized, String128 string) {
    try {
        switch (id) {
            case 0: {  // Level
                float db = valueNormalized * 48.0f;
                UString128(string).printFloat(db, 1);
                return kResultTrue;
            }
            case 1: {  // Fuzz
                float percent = valueNormalized * 100.0f;
                UString128(string).printFloat(percent, 1);
                return kResultTrue;
            }
            default:
                return EditController::getParamStringByValue(id, valueNormalized, string);
        }
    } catch (...) {
        std::cerr << "[Bronza] Exception in getParamStringByValue" << std::endl;
        return kResultFalse;
    }
}

tresult PLUGIN_API BronzaController::getParamValueByString(ParamID id, const TChar* string, ParamValue& valueNormalized) {
    try {
        UString128 us(string, strlen16(string));
        switch (id) {
            case 0: {  // Level
                double db = us.toDouble();
                valueNormalized = db / 48.0f;
                return kResultTrue;
            }
            case 1: {  // Fuzz
                double percent = us.toDouble();
                valueNormalized = percent / 100.0f;
                return kResultTrue;
            }
            default:
                return EditController::getParamValueByString(id, string, valueNormalized);
        }
    } catch (...) {
        std::cerr << "[Bronza] Exception in getParamValueByString" << std::endl;
        return kResultFalse;
    }
}

tresult PLUGIN_API BronzaController::setComponentState(IBStream* state) {
    try {
        return EditController::setComponentState(state);
    } catch (...) {
        std::cerr << "[Bronza] Exception in setComponentState" << std::endl;
        return kResultFalse;
    }
}