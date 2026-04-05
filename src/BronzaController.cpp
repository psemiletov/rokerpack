#include "BronzaController.h"
#include "BronzaPlugView.h"
#include "pluginterfaces/base/ustring.h"
#include <iostream>

using namespace Steinberg;
using namespace Steinberg::Vst;

tresult PLUGIN_API BronzaController::initialize (FUnknown* context) {
    try {
        tresult result = EditController::initialize (context);
        if (result != kResultOk) return result;

        parameters.addParameter (STR16 ("Level"), STR16 ("dB"), 0, 
                                 29.0 / 48.0, 
                                 ParameterInfo::kCanAutomate, 0);

        parameters.addParameter (STR16 ("Fuzz"), STR16 ("%"), 0, 
                                 0.87, 
                                 ParameterInfo::kCanAutomate, 1);

        std::cout << "[Bronza] Controller initialized successfully" << std::endl;
        return kResultOk;
    } catch (...) {
        std::cerr << "[Bronza] Exception in controller initialize()" << std::endl;
        return kResultFalse;
    }
}

tresult PLUGIN_API BronzaController::terminate () {
    try {
        return EditController::terminate ();
    } catch (...) {
        std::cerr << "[Bronza] Exception in controller terminate()" << std::endl;
        return kResultFalse;
    }
}

Steinberg::IPlugView* PLUGIN_API BronzaController::createView (Steinberg::FIDString name) {
    try {
        // Проверка имени редактора
        if (strcmp(name, "editor") != 0) {
            return nullptr;
        }
        
        std::cout << "[Bronza] Creating editor GUI" << std::endl;
        
        // Возвращаем новый IPlugView
        BronzaPlugView* plugView = new BronzaPlugView(this);
        plugView->addRef();
        return plugView;
        
    } catch (const std::exception& e) {
        std::cerr << "[Bronza] Exception in createView: " << e.what() << std::endl;
        return nullptr;
    } catch (...) {
        std::cerr << "[Bronza] Unknown exception in createView" << std::endl;
        return nullptr;
    }
}

tresult PLUGIN_API BronzaController::getParamStringByValue (ParamID id, ParamValue valueNormalized, String128 string) {
    try {
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
    } catch (...) {
        std::cerr << "[Bronza] Exception in getParamStringByValue" << std::endl;
        return kResultFalse;
    }
}