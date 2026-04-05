cmake_minimum_required(VERSION 3.22)
project(Bronza VERSION 1.0.0)

cmake_policy(SET CMP0135 NEW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Подтягиваем JUCE через FetchContent
set(JUCETAG "7.0.12")
include(FetchContent)

FetchContent_Declare(
  juce
  URL https://github.com/juce-framework/JUCE/archive/refs/tags/${JUCETAG}.tar.gz
  EXCLUDE_FROM_ALL
)

FetchContent_MakeAvailable(juce)

# Создаём плагин
juce_add_plugin(Bronza
  VERSION 1.0.0
  COMPANY_NAME "Semiletov"
  PRODUCT_NAME "Bronza"
  PLUGIN_MANUFACTURER_CODE Semi
  PLUGIN_CODE Brnz
  FORMATS VST3
  VST3_CATEGORIES Fx
  IS_SYNTH FALSE
  NEEDS_MIDI_INPUT FALSE
  NEEDS_MIDI_OUTPUT FALSE
  COPY_PLUGIN_AFTER_BUILD FALSE
  JUCE_DISPLAY_SPLASH_SCREEN FALSE
)

# Генерируем JuceHeader.h
juce_generate_juce_header(Bronza)

# Добавляем исходники
target_sources(Bronza PRIVATE
  src/BronzaLookAndFeel.cpp
  src/BronzaProcessor.cpp
  src/BronzaEditor.cpp
  src/dsp.cpp
  src/fx-resofilter.cpp
)

# Настройки компиляции
target_compile_definitions(Bronza PRIVATE
  JUCE_JACK=0
  JUCE_WEB_BROWSER=0
  JUCE_USE_CURL=0
  JUCE_VST3_CAN_REPLACE_VST2=0
)

# Подключаем библиотеки
target_link_libraries(Bronza PRIVATE
  juce::juce_audio_utils
  juce::juce_recommended_config_flags
  juce::juce_recommended_lto_flags
  juce::juce_recommended_warning_flags
)