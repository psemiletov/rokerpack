#include "public.sdk/source/main/pluginfactory.h"

#if defined(__linux__) || defined(__gnu_linux__)
#include <dlfcn.h>

// Стандартная точка входа для VST3 на Linux
extern "C" {
    SMTG_EXPORT_SYMBOL Steinberg::IPluginFactory* PLUGIN_API GetPluginFactory ();

    // Это та самая функция, которую ищет валидатор и DAW
    SMTG_EXPORT_SYMBOL bool ModuleEntry (void* sharedLibraryHandle) {
        return true; 
    }

    SMTG_EXPORT_SYMBOL bool ModuleExit (void* sharedLibraryHandle) {
        return true;
    }
}
#endif