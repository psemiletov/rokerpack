#pragma once

#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <memory>
#include <functional>
#include <atomic>

class ImGuiWindow {
public:
    ImGuiWindow(int width = 800, int height = 600);
    ~ImGuiWindow() {
        shutdown();
    }
    
    bool initialize();
    void shutdown();
    bool isOpen() const;
    void newFrame();
    void render();
    void swapBuffers();
    
    // Callbacks для обновления параметров
    void setParameterCallback(std::function<void(int, float)> callback) {
        paramCallback = callback;
    }
    
    void setGetParameterCallback(std::function<float(int)> callback) {
        getParamCallback = callback;
    }
    
    GLFWwindow* getWindow() const { return window; }
    bool isInitialized() const { return initialized; }
    
private:
    void drawUI();
    
    GLFWwindow* window = nullptr;
    int width, height;
    const char* glsl_version = "#version 330";
    std::atomic<bool> initialized = false;
    
    // Параметры UI
    float levelSlider = 0.6f;
    float fuzzSlider = 0.87f;
    
    std::function<void(int, float)> paramCallback;
    std::function<float(int)> getParamCallback;
};