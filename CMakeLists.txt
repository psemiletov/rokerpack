cmake_minimum_required(VERSION 3.21)
project(Bronza VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

include(FetchContent)

# ============================================================================
# ОФИЦИАЛЬНЫЕ ОПЦИИ SDK
# ============================================================================
set(SMTG_ENABLE_VST3_PLUGIN_EXAMPLES OFF CACHE BOOL "Disable VST3 plugin examples" FORCE)
set(SMTG_ENABLE_VST3_HOSTING_EXAMPLES OFF CACHE BOOL "Disable VST3 hosting examples" FORCE)
set(SMTG_ENABLE_VSTGUI_SUPPORT OFF CACHE BOOL "Disable VSTGUI support" FORCE)

set(SMTG_BUILD_VST3_TESTS OFF CACHE BOOL "" FORCE)
set(SMTG_BUILD_VSTGUI OFF CACHE BOOL "" FORCE)
set(SMTG_BUILD_AAX_WRAPPER OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    vst3sdk
    GIT_REPOSITORY https://github.com/steinbergmedia/vst3sdk.git
    GIT_TAG v3.7.11_build_10
    GIT_SHALLOW TRUE
)

set(ENV{SMTG_ENABLE_VST3_PLUGIN_EXAMPLES} "OFF")
set(ENV{SMTG_ENABLE_VST3_HOSTING_EXAMPLES} "OFF")
set(ENV{SMTG_ENABLE_VSTGUI_SUPPORT} "OFF")

FetchContent_MakeAvailable(vst3sdk)

if(TARGET vstgui AND NOT TARGET vstgui::vstgui)
    add_library(vstgui::vstgui ALIAS vstgui)
endif()

add_compile_definitions(
    VSTGUI_NOT_SUPPORTED
    SMTG_DISABLE_VSTGUI_SUPPORT
)

# Поиск X11 для Linux
find_package(X11 REQUIRED)

# ============================================================================
# Плагин
# ============================================================================
set(BRONZA_SOURCES
    src/factory.cpp
    src/BronzaProcessor.cpp
    src/BronzaController.cpp
    src/BronzaPlugView.cpp
    src/ImGuiRenderer.cpp
    src/UIWidgets.cpp
    src/dsp.cpp
    src/fx-resofilter.cpp
    src/vst3_entry.cpp
)

set(BRONZA_HEADERS
    src/BronzaProcessor.h
    src/BronzaController.h
    src/BronzaPlugView.h
    src/ImGuiRenderer.h
    src/UIWidgets.h
    src/dsp.h
    src/fx-resofilter.h
)

add_library(Bronza SHARED ${BRONZA_SOURCES} ${BRONZA_HEADERS})

set_target_properties(Bronza PROPERTIES
    PREFIX ""
    SUFFIX ".vst3"
    CXX_VISIBILITY_PRESET default
    VISIBILITY_INLINES_HIDDEN OFF
)

target_include_directories(Bronza PRIVATE 
    src
    ${vst3sdk_SOURCE_DIR}
    ${vst3sdk_SOURCE_DIR}/public.sdk/source/main
    ${vst3sdk_SOURCE_DIR}/public.sdk/source/vst
    ${vst3sdk_SOURCE_DIR}/public.sdk/source/common
    ${X11_INCLUDE_DIR}
)

target_link_libraries(Bronza PRIVATE 
    pluginterfaces
    base
    sdk
    ${X11_LIBRARIES}
)

if(LINUX)
    target_link_libraries(Bronza PRIVATE pthread dl)
    target_compile_options(Bronza PRIVATE -fPIC -fvisibility=default)
endif()

target_compile_definitions(Bronza PRIVATE
    BUILDING_VST3_PLUGIN
    SMTG_OS_LINUX
)

install(TARGETS Bronza
    LIBRARY DESTINATION ~/.vst3
    RUNTIME DESTINATION ~/.vst3
)

message(STATUS "========================================")
message(STATUS "Bronza VST3 Plugin Configuration:")
message(STATUS "  - Custom UI: ENABLED")
message(STATUS "  - Backend: X11 (Linux)")
message(STATUS "  - Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  - Parameters: Level, Fuzz")
message(STATUS "========================================")