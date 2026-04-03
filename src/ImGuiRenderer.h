#pragma once

#include <vector>
#include <functional>
#include <atomic>
#include <memory>

class UIWidgets;

class ImGuiRenderer {
public:
    ImGuiRenderer(int width = 800, int height = 300);
    ~ImGuiRenderer();
    
    const std::vector<uint8_t>& getPixelBuffer() const { return pixels; }
    
    bool initialize();
    void shutdown();
    
    void newFrame();
    void endFrame();
    void render();
    
    const unsigned char* getPixels() const { return pixels.data(); }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getStride() const { return width * 4; }
    
    void setParameterCallback(std::function<void(int, float)> callback) {
        paramCallback = callback;
    }
    
    void setGetParameterCallback(std::function<float(int)> callback) {
        getParamCallback = callback;
    }
    
    bool isInitialized() const { return initialized; }
    
    // Обработчики мыши
    void onMouseMove(int x, int y);
    void onMouseDown(int x, int y);
    void onMouseUp(int x, int y);
    
private:
    void renderDrawData();
    
    int width, height;
    std::atomic<bool> initialized = false;
    std::vector<unsigned char> pixels;
    std::unique_ptr<UIWidgets> ui;
    
    float levelSlider = 0.6f;
    float fuzzSlider = 0.87f;
    
    std::function<void(int, float)> paramCallback;
    std::function<float(int)> getParamCallback;
};