#include "BronzaEditor.h"


BronzaAudioEditor::BronzaAudioEditor (BronzaAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Устанавливаем кастомный LookAndFeel для всего редактора
    setLookAndFeel (&bronzaLookAndFeel);
    
    // === Настройка слайдера Level ===
    levelSlider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    levelSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
    levelSlider.setRange (0.0f, 1.0f, 0.001f);
    levelSlider.setValue (audioProcessor.getLevelParam()->get(), juce::dontSendNotification);
    levelSlider.addListener (this);
    levelSlider.setTextValueSuffix (" dB");
    
    // КЛЮЧЕВОЕ ДОБАВЛЕНИЕ: настройка Rotary параметров
    levelSlider.setRotaryParameters (0.0f,                     // Начальный угол (радианы)
                                      juce::MathConstants<float>::twoPi,  // Конечный угол (полный круг)
                                      true);                   // Обратный отсчёт
    
    addAndMakeVisible (levelSlider);
    
    // === Настройка слайдера Intensity ===
    intensitySlider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    intensitySlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
    intensitySlider.setRange (0.0f, 1.0f, 0.001f);
    intensitySlider.setValue (audioProcessor.getIntensityParam()->get(), juce::dontSendNotification);
    intensitySlider.addListener (this);
    
    // КЛЮЧЕВОЕ ДОБАВЛЕНИЕ: настройка Rotary параметров
    intensitySlider.setRotaryParameters (0.0f, 
                                         juce::MathConstants<float>::twoPi, 
                                         true);
    
    addAndMakeVisible (intensitySlider);
    
    // === Настройка меток ===
    levelLabel.setText ("Level", juce::dontSendNotification);
    levelLabel.setJustificationType (juce::Justification::centred);
    levelLabel.attachToComponent (&levelSlider, false);
    addAndMakeVisible (levelLabel);
    
    intensityLabel.setText ("Intensity", juce::dontSendNotification);
    intensityLabel.setJustificationType (juce::Justification::centred);
    intensityLabel.attachToComponent (&intensitySlider, false);
    addAndMakeVisible (intensityLabel);
    
    // Размер окна
    setSize (350, 300);
}

BronzaAudioEditor::~BronzaAudioEditor()
{
    // Важно: сбрасываем LookAndFeel перед удалением
    setLookAndFeel (nullptr);
}

void BronzaAudioEditor::paint (juce::Graphics& g)
{
    // Рисуем фон с текстурой "под шагрень"
    g.fillAll (bronzaLookAndFeel.findColour (juce::ResizableWindow::backgroundColourId));
    
    // Добавляем декоративную рамку
    g.setColour (juce::Colour (0xFF1A1A1A));
    g.drawRect (getLocalBounds(), 2.0f);
    
    // Добавляем "ламповый" индикатор (красная точка в углу)
    g.setColour (juce::Colour (0xFFFF4500));
    g.fillEllipse (getWidth() - 20, 10, 8, 8);
    g.setColour (juce::Colours::black);
    g.drawEllipse (getWidth() - 20, 10, 8, 8, 1.0f);
}

void BronzaAudioEditor::resized()
{
    auto area = getLocalBounds().reduced (30, 40);
    
    // Делим область на две части
    auto leftArea = area.removeFromLeft (area.getWidth() / 2);
    auto rightArea = area;
    
    // Размещаем слайдеры
    levelSlider.setBounds (leftArea.reduced (20, 0));
    intensitySlider.setBounds (rightArea.reduced (20, 0));
}

void BronzaAudioEditor::sliderValueChanged (juce::Slider* slider)
{
    if (slider == &levelSlider)
    {
        audioProcessor.getLevelParam()->setValueNotifyingHost ((float) levelSlider.getValue());
        levelSlider.repaint();  // Принудительно перерисовываем слайдер
    }
    else if (slider == &intensitySlider)
    {
        audioProcessor.getIntensityParam()->setValueNotifyingHost ((float) intensitySlider.getValue());
        intensitySlider.repaint();  // Принудительно перерисовываем слайдер
    }
}