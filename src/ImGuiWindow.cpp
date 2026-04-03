#include "ImGuiWindow.h"
#include <iostream>

ImGuiWindow::ImGuiWindow(int width, int height) 
    : width(width), height(height) {
}

bool ImGuiWindow::initialize() {
    try {
        // Инициализация GLFW
        if (!glfwInit()) {
            std::cerr << "[Bronza] Failed to initialize GLFW" << std::endl;
            return false;
        }
        
        // Контекст OpenGL
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // Скрытое окно по умолчанию
        
        // Создание окна
        window = glfwCreateWindow(width, height, "Bronza VST3 - ImGui Control Panel", nullptr, nullptr);
        if (!window) {
            std::cerr << "[Bronza] Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return false;
        }
        
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1); // VSync
        
        // Установка OpenGL параметров
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        
        // Инициализация ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        
        // Стиль
        ImGui::StyleColorsDark();
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 5.0f;
        style.FrameRounding = 3.0f;
        
        // Бэкенды
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
        
        initialized = true;
        std::cout << "[Bronza] ImGui window initialized successfully" << std::endl;
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[Bronza] Exception during ImGui initialization: " << e.what() << std::endl;
        return false;
    } catch (...) {
        std::cerr << "[Bronza] Unknown exception during ImGui initialization" << std::endl;
        return false;
    }
}

void ImGuiWindow::shutdown() {
    if (initialized) {
        try {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
            initialized = false;
        } catch (...) {
            std::cerr << "[Bronza] Exception during ImGui shutdown" << std::endl;
        }
    }
    
    if (window) {
        glfwDestroyWindow(window);
        glfwTerminate();
        window = nullptr;
    }
}

bool ImGuiWindow::isOpen() const {
    return window && !glfwWindowShouldClose(window) && initialized;
}

void ImGuiWindow::newFrame() {
    if (!initialized || !window) return;
    
    try {
        glfwPollEvents();
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        drawUI();
        
        ImGui::Render();
    } catch (...) {
        std::cerr << "[Bronza] Exception in newFrame" << std::endl;
    }
}

void ImGuiWindow::render() {
    if (!initialized || !window) return;
    
    try {
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    } catch (...) {
        std::cerr << "[Bronza] Exception in render" << std::endl;
    }
}

void ImGuiWindow::swapBuffers() {
    if (!initialized || !window) return;
    
    try {
        glfwSwapBuffers(window);
    } catch (...) {
        std::cerr << "[Bronza] Exception in swapBuffers" << std::endl;
    }
}

void ImGuiWindow::drawUI() {
    if (!initialized) return;
    
    try {
        // Главное окно
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
        
        ImGui::Begin("Bronza VST3 Control Panel", nullptr, ImGuiWindowFlags_NoMove);
        
        ImGui::Text("VST3 Audio Plugin GUI");
        ImGui::Separator();
        
        // Получение текущих значений из плагина
        if (getParamCallback) {
            levelSlider = getParamCallback(0);
            fuzzSlider = getParamCallback(1);
        }
        
        // Слайдер для уровня
        ImGui::Text("Level");
        if (ImGui::SliderFloat("##Level", &levelSlider, 0.0f, 1.0f, "%.2f")) {
            if (paramCallback) {
                paramCallback(0, levelSlider);
            }
        }
        
        // Слайдер для Fuzz
        ImGui::Text("Fuzz");
        if (ImGui::SliderFloat("##Fuzz", &fuzzSlider, 0.0f, 1.0f, "%.2f")) {
            if (paramCallback) {
                paramCallback(1, fuzzSlider);
            }
        }
        
        // Информационная панель
        ImGui::Separator();
        ImGui::Text("Level (dB): %.1f", levelSlider * 48.0f);
        ImGui::Text("Fuzz (%%): %.1f", fuzzSlider * 100.0f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 
                    1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        
        ImGui::End();
    } catch (...) {
        std::cerr << "[Bronza] Exception in drawUI" << std::endl;
    }
}