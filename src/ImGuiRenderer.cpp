#include "ImGuiRenderer.h"
#include "imgui.h"
#include <iostream>
#include <cstring>
#include <algorithm>

ImGuiRenderer::ImGuiRenderer(int width, int height) 
    : width(width), height(height), pixels(width * height * 4, 0) {
}

ImGuiRenderer::~ImGuiRenderer() {
    shutdown();
}

bool ImGuiRenderer::initialize() {
    try {
        // Инициализация ImGui без окна
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.DisplaySize = ImVec2((float)width, (float)height);
        io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
        
        // Стиль
        ImGui::StyleColorsDark();
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 5.0f;
        style.FrameRounding = 3.0f;
        style.WindowPadding = ImVec2(10, 10);
        
        initialized = true;
        std::cout << "[Bronza] ImGui Renderer initialized (" << width << "x" << height << ")" << std::endl;
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[Bronza] Exception during ImGui initialization: " << e.what() << std::endl;
        return false;
    } catch (...) {
        std::cerr << "[Bronza] Unknown exception during ImGui initialization" << std::endl;
        return false;
    }
}

void ImGuiRenderer::shutdown() {
    if (initialized) {
        try {
            ImGui::DestroyContext();
            initialized = false;
        } catch (...) {
            std::cerr << "[Bronza] Exception during ImGui shutdown" << std::endl;
        }
    }
}

void ImGuiRenderer::newFrame() {
    if (!initialized) return;
    
    try {
        ImGui::NewFrame();
        drawUI();
    } catch (...) {
        std::cerr << "[Bronza] Exception in newFrame" << std::endl;
    }
}

void ImGuiRenderer::endFrame() {
    if (!initialized) return;
    
    try {
        ImGui::Render();
        renderDrawData();
    } catch (...) {
        std::cerr << "[Bronza] Exception in endFrame" << std::endl;
    }
}

void ImGuiRenderer::render() {
    // Рисование уже произошло в endFrame()
}

void ImGuiRenderer::renderDrawData() {
    // Заполняем пиксели серым цветом
    uint32_t bgColor = 0xFF282828; // ABGR формат (тёмный серый)
    for (int i = 0; i < width * height; ++i) {
        pixels[i * 4 + 0] = 0x28;     // B
        pixels[i * 4 + 1] = 0x28;     // G
        pixels[i * 4 + 2] = 0x28;     // R
        pixels[i * 4 + 3] = 0xFF;     // A
    }
    
    // Простая отрисовка белого прямоугольника для тестирования
    int margin = 20;
    int boxW = width - margin * 2;
    int boxH = 100;
    int boxY = 30;
    
    // Рисуем белую границу
    for (int x = margin; x < margin + boxW; ++x) {
        for (int y = boxY; y < boxY + boxH; ++y) {
            if (y >= 0 && y < height && x >= 0 && x < width) {
                int idx = (y * width + x) * 4;
                pixels[idx + 0] = 0xFF;     // B
                pixels[idx + 1] = 0xFF;     // G
                pixels[idx + 2] = 0xFF;     // R
                pixels[idx + 3] = 0xFF;     // A
            }
        }
    }
    
    // Простой текст "Level" (рисуем прямоугольники вместо текста)
    int labelY = boxY + 15;
    for (int y = labelY; y < labelY + 8; ++y) {
        for (int x = margin + 10; x < margin + 50; ++x) {
            if (y >= 0 && y < height && x >= 0 && x < width) {
                int idx = (y * width + x) * 4;
                pixels[idx + 0] = 0xFF;
                pixels[idx + 1] = 0xFF;
                pixels[idx + 2] = 0xFF;
                pixels[idx + 3] = 0xFF;
            }
        }
    }
    
    // Рисуем слайдеры как прямоугольники
    int sliderX = margin + 10;
    int sliderY = labelY + 20;
    int sliderWidth = boxW - 30;
    int sliderHeight = 20;
    
    // Фон слайдера
    for (int y = sliderY; y < sliderY + sliderHeight; ++y) {
        for (int x = sliderX; x < sliderX + sliderWidth; ++x) {
            if (y >= 0 && y < height && x >= 0 && x < width) {
                int idx = (y * width + x) * 4;
                pixels[idx + 0] = 0x44;
                pixels[idx + 1] = 0x44;
                pixels[idx + 2] = 0x44;
                pixels[idx + 3] = 0xFF;
            }
        }
    }
    
    // Ползунок Level
    int levelPos = sliderX + (int)((sliderWidth - 10) * levelSlider);
    for (int y = sliderY + 2; y < sliderY + sliderHeight - 2; ++y) {
        for (int x = levelPos; x < levelPos + 10; ++x) {
            if (y >= 0 && y < height && x >= 0 && x < width) {
                int idx = (y * width + x) * 4;
                pixels[idx + 0] = 0x00;
                pixels[idx + 1] = 0xFF;
                pixels[idx + 2] = 0x00;
                pixels[idx + 3] = 0xFF;
            }
        }
    }
    
    // Второй слайдер для Fuzz
    int fuzzY = sliderY + 30;
    for (int y = fuzzY; y < fuzzY + sliderHeight; ++y) {
        for (int x = sliderX; x < sliderX + sliderWidth; ++x) {
            if (y >= 0 && y < height && x >= 0 && x < width) {
                int idx = (y * width + x) * 4;
                pixels[idx + 0] = 0x44;
                pixels[idx + 1] = 0x44;
                pixels[idx + 2] = 0x44;
                pixels[idx + 3] = 0xFF;
            }
        }
    }
    
    int fuzzPos = sliderX + (int)((sliderWidth - 10) * fuzzSlider);
    for (int y = fuzzY + 2; y < fuzzY + sliderHeight - 2; ++y) {
        for (int x = fuzzPos; x < fuzzPos + 10; ++x) {
            if (y >= 0 && y < height && x >= 0 && x < width) {
                int idx = (y * width + x) * 4;
                pixels[idx + 0] = 0xFF;
                pixels[idx + 1] = 0x00;
                pixels[idx + 2] = 0x00;
                pixels[idx + 3] = 0xFF;
            }
        }
    }
}

void ImGuiRenderer::drawUI() {
    if (!initialized) return;
    
    try {
        // Получение текущих значений из плагина
        if (getParamCallback) {
            levelSlider = getParamCallback(0);
            fuzzSlider = getParamCallback(1);
        }
        
        // Главное окно
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(380, 280), ImGuiCond_FirstUseEver);
        
        ImGui::Begin("Bronza VST3", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        
        ImGui::Text("VST3 Audio Plugin - Bronza");
        ImGui::Separator();
        
        // Слайдер для уровня
        if (ImGui::SliderFloat("Level##level", &levelSlider, 0.0f, 1.0f, "%.2f")) {
            if (paramCallback) {
                paramCallback(0, levelSlider);
            }
        }
        
        // Слайдер для Fuzz
        if (ImGui::SliderFloat("Fuzz##fuzz", &fuzzSlider, 0.0f, 1.0f, "%.2f")) {
            if (paramCallback) {
                paramCallback(1, fuzzSlider);
            }
        }
        
        // Информационная панель
        ImGui::Separator();
        ImGui::Text("Level (dB): %.1f", levelSlider * 48.0f);
        ImGui::Text("Fuzz (%%): %.1f", fuzzSlider * 100.0f);
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        
        ImGui::End();
        
    } catch (...) {
        std::cerr << "[Bronza] Exception in drawUI" << std::endl;
    }
}