#include "BronzaPlugView.h"
#include <iostream>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

BronzaPlugView::BronzaPlugView(Steinberg::Vst::EditController* ctrl)
    : controller(ctrl) {
    
    renderer = std::make_shared<ImGuiRenderer>(800, 600);
    
    if (controller) {
        // Установка callbacks для обновления параметров
        renderer->setParameterCallback([this](int paramId, float value) {
            if (controller) {
                controller->setParamNormalized(paramId, value);
            }
        });
        
        renderer->setGetParameterCallback([this](int paramId) -> float {
            if (controller) {
                return controller->getParamNormalized(paramId);
            }
            return 0.0f;
        });
    }
}

BronzaPlugView::~BronzaPlugView() {
    if (renderer) {
        renderer->shutdown();
    }
    if (xImage) {
        XDestroyImage(xImage);
        xImage = nullptr;
    }
    if (xPixmap) {
        XFreePixmap(xDisplay, xPixmap);
        xPixmap = 0;
    }
    if (xGC) {
        XFreeGC(xDisplay, xGC);
        xGC = nullptr;
    }
}

Steinberg::tresult PLUGIN_API BronzaPlugView::isPlatformTypeSupported(Steinberg::FIDString type) {
    if (strcmp(type, "X11Window") == 0 || strcmp(type, "X11EmbeddedWindow") == 0) {
        return Steinberg::kResultOk;
    }
    return Steinberg::kResultFalse;
}

Steinberg::tresult PLUGIN_API BronzaPlugView::attached(void* parent, Steinberg::FIDString type) {
    try {
        if (!renderer || !renderer->initialize()) {
            std::cerr << "[Bronza] Failed to initialize renderer" << std::endl;
            return Steinberg::kResultFalse;
        }
        
        xWindow = (Window)(intptr_t)parent;
        xDisplay = XOpenDisplay(nullptr);
        
        if (!xDisplay) {
            std::cerr << "[Bronza] Failed to open X11 display" << std::endl;
            return Steinberg::kResultFalse;
        }
        
        int screen = DefaultScreen(xDisplay);
        Visual* visual = DefaultVisual(xDisplay, screen);
        int depth = DefaultDepth(xDisplay, screen);
        
        // Создаём pixmap для отрисовки
        xPixmap = XCreatePixmap(xDisplay, xWindow, 800, 600, depth);
        xGC = XCreateGC(xDisplay, xPixmap, 0, nullptr);
        
        // Создаём XImage для работы с пикселями
        xImage = XCreateImage(xDisplay, visual, depth, ZPixmap, 0,
                             (char*)malloc(800 * 600 * 4), 800, 600, 32, 0);
        
        std::cout << "[Bronza] PlugView attached, X11 window initialized" << std::endl;
        
        // Рисуем первый раз
        renderer->newFrame();
        renderer->endFrame();
        
        return Steinberg::kResultOk;
        
    } catch (const std::exception& e) {
        std::cerr << "[Bronza] Exception in attached(): " << e.what() << std::endl;
        return Steinberg::kResultFalse;
    } catch (...) {
        std::cerr << "[Bronza] Unknown exception in attached()" << std::endl;
        return Steinberg::kResultFalse;
    }
}

Steinberg::tresult PLUGIN_API BronzaPlugView::removed() {
    if (renderer) {
        renderer->shutdown();
    }
    std::cout << "[Bronza] PlugView removed" << std::endl;
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API BronzaPlugView::onWheel(float distance) {
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API BronzaPlugView::onKeyDown(Steinberg::char16 key, Steinberg::int16 keyCode, Steinberg::int16 modifiers) {
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API BronzaPlugView::onKeyUp(Steinberg::char16 key, Steinberg::int16 keyCode, Steinberg::int16 modifiers) {
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API BronzaPlugView::getSize(Steinberg::ViewRect* rect) {
    if (rect) {
        rect->left = 0;
        rect->top = 0;
        rect->right = width;
        rect->bottom = height;
        return Steinberg::kResultOk;
    }
    return Steinberg::kResultFalse;
}

Steinberg::tresult PLUGIN_API BronzaPlugView::onSize(Steinberg::ViewRect* newSize) {
    if (newSize) {
        width = newSize->right - newSize->left;
        height = newSize->bottom - newSize->top;
    }
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API BronzaPlugView::canResize() {
    return Steinberg::kResultFalse;
}

Steinberg::tresult PLUGIN_API BronzaPlugView::checkSizeConstraint(Steinberg::ViewRect* rect) {
    if (rect) {
        rect->right = rect->left + 800;
        rect->bottom = rect->top + 600;
        return Steinberg::kResultOk;
    }
    return Steinberg::kResultFalse;
}

Steinberg::tresult PLUGIN_API BronzaPlugView::onFocus(Steinberg::TBool state) {
    std::cout << "[Bronza] GUI focus: " << (state ? "true" : "false") << std::endl;
    if (state && renderer && xDisplay && xWindow) {
        // Перерисовка при получении фокуса
        renderer->newFrame();
        renderer->endFrame();
    }
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API BronzaPlugView::setFrame(Steinberg::IPlugFrame* frame) {
    plugFrame = frame;
    std::cout << "[Bronza] PlugFrame set" << std::endl;
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API BronzaPlugView::queryInterface(const Steinberg::TUID _iid, void** obj) {
    if (!obj) {
        return Steinberg::kInvalidArgument;
    }
    
    if (Steinberg::FUnknownPrivate::iidEqual(_iid, Steinberg::FUnknown::iid)) {
        *obj = static_cast<Steinberg::IPlugView*>(this);
        addRef();
        return Steinberg::kResultOk;
    }
    if (Steinberg::FUnknownPrivate::iidEqual(_iid, Steinberg::IPlugView::iid)) {
        *obj = static_cast<Steinberg::IPlugView*>(this);
        addRef();
        return Steinberg::kResultOk;
    }
    *obj = nullptr;
    return Steinberg::kNoInterface;
}

Steinberg::uint32 PLUGIN_API BronzaPlugView::addRef() {
    return ++refCount;
}

Steinberg::uint32 PLUGIN_API BronzaPlugView::release() {
    if (--refCount == 0) {
        delete this;
        return 0;
    }
    return refCount;
}