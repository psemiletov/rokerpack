#pragma once

#include "pluginterfaces/gui/iplugview.h"
#include "pluginterfaces/base/funknown.h"
#include "public.sdk/source/vst/vsteditcontroller.h"
#include "ImGuiRenderer.h"
#include <memory>
#include <X11/Xlib.h>
#include <chrono>

class BronzaPlugView : public Steinberg::IPlugView {
public:
    BronzaPlugView(Steinberg::Vst::EditController* controller);
    ~BronzaPlugView();
    
    // IPlugView interface
    Steinberg::tresult PLUGIN_API isPlatformTypeSupported(Steinberg::FIDString type) override;
    Steinberg::tresult PLUGIN_API attached(void* parent, Steinberg::FIDString type) override;
    Steinberg::tresult PLUGIN_API removed() override;
    Steinberg::tresult PLUGIN_API onWheel(float distance) override;
    Steinberg::tresult PLUGIN_API onKeyDown(Steinberg::char16 key, Steinberg::int16 keyCode, Steinberg::int16 modifiers) override;
    Steinberg::tresult PLUGIN_API onKeyUp(Steinberg::char16 key, Steinberg::int16 keyCode, Steinberg::int16 modifiers) override;
    Steinberg::tresult PLUGIN_API getSize(Steinberg::ViewRect* rect) override;
    Steinberg::tresult PLUGIN_API onSize(Steinberg::ViewRect* newSize) override;
    Steinberg::tresult PLUGIN_API canResize() override;
    Steinberg::tresult PLUGIN_API checkSizeConstraint(Steinberg::ViewRect* rect) override;
    Steinberg::tresult PLUGIN_API onFocus(Steinberg::TBool state) override;
    Steinberg::tresult PLUGIN_API setFrame(Steinberg::IPlugFrame* frame) override;
    
    // IUnknown interface
    Steinberg::tresult PLUGIN_API queryInterface(const Steinberg::TUID _iid, void** obj) override;
    Steinberg::uint32 PLUGIN_API addRef() override;
    Steinberg::uint32 PLUGIN_API release() override;
    
private:
    void paint();
    
    std::shared_ptr<ImGuiRenderer> renderer;
    Steinberg::Vst::EditController* controller = nullptr;
    Steinberg::IPlugFrame* plugFrame = nullptr;
    Steinberg::uint32 refCount = 1;
    int width = 800;
    int height = 600;
    
    // X11 ресурсы
    Display* xDisplay = nullptr;
    Window xWindow = 0;
    Pixmap xPixmap = 0;
    GC xGC = nullptr;
    XImage* xImage = nullptr;
    std::chrono::steady_clock::time_point lastFrameTime;
    bool needsRepaint = true;
};