#include "BronzaPlugView.h"
#include <iostream>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <cstring>

BronzaPlugView::BronzaPlugView(Steinberg::Vst::EditController* ctrl)
    : controller(ctrl) {
    
    renderer = std::make_shared<ImGuiRenderer>(800, 300);
    
    if (controller) {
        // Callback для установки параметров (GUI -> параметры плагина)
        renderer->setParameterCallback([this](int paramId, float value) {
            if (controller) {
                // Нормализуем значение и устанавливаем в контроллер
                Steinberg::Vst::ParamValue normalized = value;
                controller->setParamNormalized(paramId, normalized);
                
                std::cout << "[Bronza] Parameter " << paramId << " changed to " << value << std::endl;
            }
        });
        
        // Callback для получения текущих значений параметров (параметры -> GUI)
        renderer->setGetParameterCallback([this](int paramId) -> float {
            if (controller) {
                Steinberg::Vst::ParamValue normalized = controller->getParamNormalized(paramId);
                return (float)normalized;
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
    if (pixelData) {
        free(pixelData);
        pixelData = nullptr;
    }
    if (xPixmap) {
        XFreePixmap(xDisplay, xPixmap);
        xPixmap = 0;
    }
    if (xGC) {
        XFreeGC(xDisplay, xGC);
        xGC = nullptr;
    }
    if (xDisplay) {
        XCloseDisplay(xDisplay);
        xDisplay = nullptr;
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
            std::cerr << "[Bronza] Failed to initialize ImGui renderer" << std::endl;
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
        xPixmap = XCreatePixmap(xDisplay, xWindow, width, height, depth);
        if (!xPixmap) {
            std::cerr << "[Bronza] Failed to create X11 pixmap" << std::endl;
            return Steinberg::kResultFalse;
        }
        
        xGC = XCreateGC(xDisplay, xPixmap, 0, nullptr);
        if (!xGC) {
            std::cerr << "[Bronza] Failed to create X11 graphics context" << std::endl;
            return Steinberg::kResultFalse;
        }
        
        // Выделяем память для пикселей (RGBA = 4 байта на пиксель)
        pixelData = (char*)malloc(width * height * 4);
        if (!pixelData) {
            std::cerr << "[Bronza] Failed to allocate pixel buffer" << std::endl;
            return Steinberg::kResultFalse;
        }
        
        // Создаём XImage для работы с пикселями
        xImage = XCreateImage(xDisplay, visual, depth, ZPixmap, 0,
                              pixelData, width, height, 32, width * 4);
        
        if (!xImage) {
            std::cerr << "[Bronza] Failed to create XImage" << std::endl;
            free(pixelData);
            pixelData = nullptr;
            return Steinberg::kResultFalse;
        }
        
        std::cout << "[Bronza] PlugView attached successfully (" << width << "x" << height << ")" << std::endl;
        
        // Первая отрисовка
        paint();
        
        return Steinberg::kResultOk;
        
    } catch (const std::exception& e) {
        std::cerr << "[Bronza] Exception in attached(): " << e.what() << std::endl;
        return Steinberg::kResultFalse;
    } catch (...) {
        std::cerr << "[Bronza] Unknown exception in attached()" << std::endl;
        return Steinberg::kResultFalse;
    }
}

void BronzaPlugView::paint() {
    if (!renderer || !xDisplay || !xWindow || !xImage || !xPixmap || !pixelData) {
        return;
    }
    
    try {
        // Обновляем ImGui кадр
        renderer->newFrame();
        renderer->endFrame();
        
        // Получаем пиксели из renderer
        const unsigned char* srcPixels = renderer->getPixels();
        if (!srcPixels) {
            std::cerr << "[Bronza] Failed to get pixel buffer from renderer" << std::endl;
            return;
        }
        
        // Копируем пиксели в XImage буфер
        int pixelCount = width * height;
        std::memcpy(pixelData, srcPixels, pixelCount * 4);
        
        // Отправляем на экран
        XPutImage(xDisplay, xWindow, xGC, xImage, 
                  0, 0, 0, 0, width, height);
        XFlush(xDisplay);
        
    } catch (const std::exception& e) {
        std::cerr << "[Bronza] Exception in paint(): " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "[Bronza] Unknown exception in paint()" << std::endl;
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
    needsRepaint = true;
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API BronzaPlugView::onKeyDown(Steinberg::char16 key, Steinberg::int16 keyCode, Steinberg::int16 modifiers) {
    needsRepaint = true;
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API BronzaPlugView::onKeyUp(Steinberg::char16 key, Steinberg::int16 keyCode, Steinberg::int16 modifiers) {
    needsRepaint = true;
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API BronzaPlugView::onMouseDown(Steinberg::int32 x, Steinberg::int32 y) {
    lastMouseX = x;
    lastMouseY = y;
    renderer->onMouseDown(x, y);
    needsRepaint = true;
    paint();
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API BronzaPlugView::onMouseUp(Steinberg::int32 x, Steinberg::int32 y) {
    lastMouseX = x;
    lastMouseY = y;
    renderer->onMouseUp(x, y);
    needsRepaint = true;
    paint();
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API BronzaPlugView::onMouseMove(Steinberg::int32 x, Steinberg::int32 y) {
    if (x != lastMouseX || y != lastMouseY) {
        lastMouseX = x;
        lastMouseY = y;
        renderer->onMouseMove(x, y);
        needsRepaint = true;
        paint();
    }
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
        needsRepaint = true;
    }
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API BronzaPlugView::canResize() {
    return Steinberg::kResultTrue;
}

Steinberg::tresult PLUGIN_API BronzaPlugView::checkSizeConstraint(Steinberg::ViewRect* rect) {
    if (rect) {
        int minWidth = 400;
        int minHeight = 200;
        int newWidth = rect->right - rect->left;
        int newHeight = rect->bottom - rect->top;
        
        if (newWidth < minWidth) rect->right = rect->left + minWidth;
        if (newHeight < minHeight) rect->bottom = rect->top + minHeight;
        
        return Steinberg::kResultOk;
    }
    return Steinberg::kResultFalse;
}

Steinberg::tresult PLUGIN_API BronzaPlugView::onFocus(Steinberg::TBool state) {
    std::cout << "[Bronza] GUI focus: " << (state ? "true" : "false") << std::endl;
    if (state) {
        needsRepaint = true;
        paint();
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