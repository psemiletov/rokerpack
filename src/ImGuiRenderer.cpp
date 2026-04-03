#include "ImGuiRenderer.h"
#include "UIWidgets.h"
#include <iostream>
#include <cstring>
#include <algorithm>

ImGuiRenderer::ImGuiRenderer(int width, int height) 
    : width(width), height(height), pixels(width * height * 4, 0),
      ui(std::make_unique<UIWidgets>(pixels, width, height)) {
}

ImGuiRenderer::~ImGuiRenderer() {
    shutdown();
}

bool ImGuiRenderer::initialize() {
    try {
        std::cout << "[Bronza] ImGuiRenderer initialized (" << width << "x" << height << ")" << std::endl;
        initialized = true;
        return true;
    } catch (...) {
        std::cerr << "[Bronza] Exception during initialization" << std::endl;
        return false;
    }
}

void ImGuiRenderer::shutdown() {
    initialized = false;
}

void ImGuiRenderer::newFrame() {
    if (!initialized) return;
}

void ImGuiRenderer::endFrame() {
    if (!initialized) return;
    renderDrawData();
}

void ImGuiRenderer::render() {
}

void ImGuiRenderer::renderDrawData() {
    // Очистка
    std::fill(pixels.begin(), pixels.end(), 0x1A);
    
    try {
        // Получаем значения
        if (getParamCallback) {
            levelSlider = getParamCallback(0);
            fuzzSlider = getParamCallback(1);
        }
        
        // Главная панель
        UIWidgets::Rect mainPanel = {10, 10, width - 20, height - 20};
        ui->drawPanel(mainPanel, UIWidgets::Color(0x28, 0x28, 0x28),
                     UIWidgets::Color(0xFF, 0xFF, 0xFF), "Bronza VST3");
        
        // Слайдер Level слева
        bool levelChanged = ui->drawSlider(0, 50, 60, 150, 24,
                                          levelSlider, UIWidgets::Color(0x00, 0xFF, 0x00),
                                          "Level", 0.0f, 1.0f);
        if (levelChanged && paramCallback) paramCallback(0, levelSlider);
        
        // Knob Fuzz в центре
        bool fuzzChanged = ui->drawKnob(0, width/2, 120, 40,
                                       fuzzSlider, UIWidgets::Color(0xFF, 0x00, 0x00),
                                       "Fuzz", 0.0f, 1.0f);
        if (fuzzChanged && paramCallback) paramCallback(1, fuzzSlider);
        
        // Информация
        int infoY = 220;
        std::string levelInfo = "Level (dB): " + UIWidgets::formatFloat(levelSlider * 48.0f, 1);
        std::string fuzzInfo = "Fuzz (%): " + UIWidgets::formatFloat(fuzzSlider * 100.0f, 1);
        
        ui->drawLabel(30, infoY, levelInfo, UIWidgets::Color(0xAA, 0xAA, 0xAA));
        ui->drawLabel(30, infoY + 20, fuzzInfo, UIWidgets::Color(0xAA, 0xAA, 0xAA));
        
    } catch (const std::exception& e) {
        std::cerr << "[Bronza] Exception in renderDrawData: " << e.what() << std::endl;
    }
}

void ImGuiRenderer::onMouseMove(int x, int y) {
    if (ui) ui->onMouseMove(x, y);
}

void ImGuiRenderer::onMouseDown(int x, int y) {
    if (ui) ui->onMouseDown(x, y);
}

void ImGuiRenderer::onMouseUp(int x, int y) {
    if (ui) ui->onMouseUp(x, y);
}