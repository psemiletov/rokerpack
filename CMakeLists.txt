cmake_minimum_required(VERSION 3.22)
project(RokerPack VERSION 1.0.0)

cmake_policy(SET CMP0135 NEW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Подтягиваем JUCE
set(JUCETAG "7.0.12")
include(FetchContent)

FetchContent_Declare(
  juce
  URL https://github.com/juce-framework/JUCE/archive/refs/tags/${JUCETAG}.tar.gz
  EXCLUDE_FROM_ALL
)

FetchContent_MakeAvailable(juce)

# ==================== BRONZA PLUGIN ====================
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
)

juce_generate_juce_header(Bronza)

target_sources(Bronza PRIVATE
  src/BronzaProcessor.cpp
  src/BronzaEditor.cpp
  src/BronzaLookAndFeel.cpp
  src/dsp.cpp
  src/fx-resofilter.cpp
)

target_compile_definitions(Bronza PRIVATE
  JUCE_JACK=0
  JUCE_WEB_BROWSER=0
  JUCE_USE_CURL=0
  JUCE_VST3_CAN_REPLACE_VST2=0
  JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(Bronza PRIVATE
  juce::juce_audio_utils
  juce::juce_recommended_config_flags
  juce::juce_recommended_lto_flags
  juce::juce_recommended_warning_flags
)

# ==================== GRELKA PLUGIN ====================
juce_add_plugin(Grelka
  VERSION 1.0.0
  COMPANY_NAME "Semiletov"
  PRODUCT_NAME "Grelka"
  PLUGIN_MANUFACTURER_CODE Semi
  PLUGIN_CODE Grlk
  FORMATS VST3
  VST3_CATEGORIES Fx
  IS_SYNTH FALSE
  NEEDS_MIDI_INPUT FALSE
  NEEDS_MIDI_OUTPUT FALSE
  COPY_PLUGIN_AFTER_BUILD FALSE
)

juce_generate_juce_header(Grelka)

target_sources(Grelka PRIVATE
  src/GrelkaProcessor.cpp
  src/GrelkaEditor.cpp
  src/BronzaLookAndFeel.cpp  # Переиспользуем LookAndFeel
  src/dsp.cpp
  src/fx-resofilter.cpp
)

target_compile_definitions(Grelka PRIVATE
  JUCE_JACK=0
  JUCE_WEB_BROWSER=0
  JUCE_USE_CURL=0
  JUCE_VST3_CAN_REPLACE_VST2=0
  JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(Grelka PRIVATE
  juce::juce_audio_utils
  juce::juce_recommended_config_flags
  juce::juce_recommended_lto_flags
  juce::juce_recommended_warning_flags
)

# ==================== METALLUGA PLUGIN ====================
juce_add_plugin(Metalluga
  VERSION 1.0.0
  COMPANY_NAME "Semiletov"
  PRODUCT_NAME "Metalluga"
  PLUGIN_MANUFACTURER_CODE Semi
  PLUGIN_CODE Mtlg
  FORMATS VST3
  VST3_CATEGORIES Fx
  IS_SYNTH FALSE
  NEEDS_MIDI_INPUT FALSE
  NEEDS_MIDI_OUTPUT FALSE
  COPY_PLUGIN_AFTER_BUILD FALSE
)

juce_generate_juce_header(Metalluga)

target_sources(Metalluga PRIVATE
  src/MetallugaProcessor.cpp
  src/MetallugaEditor.cpp
  src/BronzaLookAndFeel.cpp
  src/dsp.cpp
  src/fx-resofilter.cpp
)

target_compile_definitions(Metalluga PRIVATE
  JUCE_JACK=0
  JUCE_WEB_BROWSER=0
  JUCE_USE_CURL=0
  JUCE_VST3_CAN_REPLACE_VST2=0
  JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(Metalluga PRIVATE
  juce::juce_audio_utils
  juce::juce_recommended_config_flags
  juce::juce_recommended_lto_flags
  juce::juce_recommended_warning_flags
)

# ==================== CHARM PLUGIN ====================
juce_add_plugin(Charm
  VERSION 1.0.0
  COMPANY_NAME "Semiletov"
  PRODUCT_NAME "Charm"
  PLUGIN_MANUFACTURER_CODE Semi
  PLUGIN_CODE Chrm
  FORMATS VST3
  VST3_CATEGORIES Fx
  IS_SYNTH FALSE
  NEEDS_MIDI_INPUT FALSE
  NEEDS_MIDI_OUTPUT FALSE
  COPY_PLUGIN_AFTER_BUILD FALSE
)

juce_generate_juce_header(Charm)

target_sources(Charm PRIVATE
  src/CharmProcessor.cpp
  src/CharmEditor.cpp
  src/BronzaLookAndFeel.cpp
  src/dsp.cpp
)

target_compile_definitions(Charm PRIVATE
  JUCE_JACK=0
  JUCE_WEB_BROWSER=0
  JUCE_USE_CURL=0
  JUCE_VST3_CAN_REPLACE_VST2=0
  JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(Charm PRIVATE
  juce::juce_audio_utils
  juce::juce_recommended_config_flags
  juce::juce_recommended_lto_flags
  juce::juce_recommended_warning_flags
)

# ==================== TEMBR PLUGIN ====================
juce_add_plugin(Tembr
  VERSION 1.0.0
  COMPANY_NAME "Semiletov"
  PRODUCT_NAME "Tembr"
  PLUGIN_MANUFACTURER_CODE Semi
  PLUGIN_CODE Tmbr
  FORMATS VST3
  VST3_CATEGORIES Fx
  IS_SYNTH FALSE
  NEEDS_MIDI_INPUT FALSE
  NEEDS_MIDI_OUTPUT FALSE
  COPY_PLUGIN_AFTER_BUILD FALSE
)

juce_generate_juce_header(Tembr)

target_sources(Tembr PRIVATE
  src/TembrProcessor.cpp
  src/TembrEditor.cpp
  src/BronzaLookAndFeel.cpp
  src/dsp.cpp
)

target_compile_definitions(Tembr PRIVATE
  JUCE_JACK=0
  JUCE_WEB_BROWSER=0
  JUCE_USE_CURL=0
  JUCE_VST3_CAN_REPLACE_VST2=0
  JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(Tembr PRIVATE
  juce::juce_audio_utils
  juce::juce_recommended_config_flags
  juce::juce_recommended_lto_flags
  juce::juce_recommended_warning_flags
)



# ==================== MISTRAL PLUGIN ====================
juce_add_plugin(Mistral
  VERSION 1.0.0
  COMPANY_NAME "Semiletov"
  PRODUCT_NAME "Mistral"
  PLUGIN_MANUFACTURER_CODE Semi
  PLUGIN_CODE Mstr
  FORMATS VST3
  VST3_CATEGORIES Fx
  IS_SYNTH FALSE
  NEEDS_MIDI_INPUT FALSE
  NEEDS_MIDI_OUTPUT FALSE
  COPY_PLUGIN_AFTER_BUILD FALSE
)

juce_generate_juce_header(Mistral)

target_sources(Mistral PRIVATE
  src/MistralProcessor.cpp
  src/MistralEditor.cpp
  src/BronzaLookAndFeel.cpp
)

target_compile_definitions(Mistral PRIVATE
  JUCE_JACK=0
  JUCE_WEB_BROWSER=0
  JUCE_USE_CURL=0
  JUCE_VST3_CAN_REPLACE_VST2=0
  JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(Mistral PRIVATE
  juce::juce_audio_utils
  juce::juce_recommended_config_flags
  juce::juce_recommended_lto_flags
  juce::juce_recommended_warning_flags
)