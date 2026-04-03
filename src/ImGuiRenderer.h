#pragma once

#include "imgui.h"
#include <memory>
#include <functional>
#include <atomic>
#include <vector>

class ImGuiRenderer {
public:
    ImGuiRenderer(int width = 800, int height = 600);
    ~ImGuiRenderer();
    
    bool initialize();
    void shutdown();
    
    void newFrame();
    void endFrame();
    void render();
    
    // Для получения пикселей для отрисовки
    const unsigned char* getPixels() const { return pixels.data(); }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getStride() const { return width * 4; } // RGBA
    
    // Callbacks для обновления параметров
    void setParameterCallback(std::function<void(int, float)> callback) {
        paramCallback = callback;
    }
    
    void setGetParameterCallback(std::function<float(int)> callback) {
        getParamCallback = callback;
    }
    
    bool isInitialized() const { return initialized; }
    
private:
    void drawUI();
    void renderDrawData();
    
    int width, height;
    std::atomic<bool> initialized = false;
    
    // Пиксели для отрисовки (RGBA)
    std::vector<unsigned char> pixels;
    
    // Параметры UI
    float levelSlider = 0.6f;
    float fuzzSlider = 0.87f;
    
    std::function<void(int, float)> paramCallback;
    std::function<float(int)> getParamCallback;
};