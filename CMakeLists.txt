cmake_minimum_required(VERSION 3.21)
project(Bronza VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

include(FetchContent)

# ============================================================================
# ОФИЦИАЛЬНЫЕ ОПЦИИ SDK ДЛЯ ОТКЛЮЧЕНИЯ ПРИМЕРОВ И GUI
# Должны быть установлены ДО вызова FetchContent_Declare
# ============================================================================
set(SMTG_ENABLE_VST3_PLUGIN_EXAMPLES OFF CACHE BOOL "Disable VST3 plugin examples" FORCE)
set(SMTG_ENABLE_VST3_HOSTING_EXAMPLES OFF CACHE BOOL "Disable VST3 hosting examples" FORCE)
set(SMTG_ENABLE_VSTGUI_SUPPORT OFF CACHE BOOL "Disable VSTGUI support" FORCE)

# Дополнительные опции для полной чистоты
set(SMTG_BUILD_VST3_TESTS OFF CACHE BOOL "" FORCE)
set(SMTG_BUILD_VSTGUI OFF CACHE BOOL "" FORCE)
set(SMTG_BUILD_AAX_WRAPPER OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    vst3sdk
    GIT_REPOSITORY https://github.com/steinbergmedia/vst3sdk.git
    GIT_TAG v3.7.11_build_10
    GIT_SHALLOW TRUE
)

# Передаем опции в дочерний CMake через переменные окружения
set(ENV{SMTG_ENABLE_VST3_PLUGIN_EXAMPLES} "OFF")
set(ENV{SMTG_ENABLE_VST3_HOSTING_EXAMPLES} "OFF")
set(ENV{SMTG_ENABLE_VSTGUI_SUPPORT} "OFF")

FetchContent_MakeAvailable(vst3sdk)

# Если после всех манипуляций vstgui всё ещё пытается собраться,
# принудительно превращаем его в пустышку
if(TARGET vstgui AND NOT TARGET vstgui::vstgui)
    add_library(vstgui::vstgui ALIAS vstgui)
endif()

# Определяем макросы для отключения GUI в коде
add_compile_definitions(
    VSTGUI_NOT_SUPPORTED
    SMTG_DISABLE_VSTGUI_SUPPORT
)

# ============================================================================
# Ваш плагин
# ============================================================================
set(BRONZA_SOURCES
    src/factory.cpp
    src/BronzaProcessor.cpp
    src/BronzaController.cpp
    src/dsp.cpp
    src/fx-resofilter.cpp
    src/vst3_entry.cpp
)

set(BRONZA_HEADERS
    src/BronzaProcessor.h
    src/BronzaController.h
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
)

target_link_libraries(Bronza PRIVATE 
    pluginterfaces
    base
    sdk
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
message(STATUS "  - VSTGUI: DISABLED (official option)")
message(STATUS "  - Plugin Examples: DISABLED")
message(STATUS "  - Hosting Examples: DISABLED")
message(STATUS "  - Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "========================================")