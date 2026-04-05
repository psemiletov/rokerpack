#include "BronzaEditor.h"

BronzaAudioEditor::BronzaAudioEditor (BronzaAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setLookAndFeel (&bronzaLookAndFeel);
    
    // === Настройка слайдера Level ===
    levelSlider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    levelSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 24);
    levelSlider.setRange (0.0f, 1.0f, 0.001f);
    levelSlider.setValue (audioProcessor.getLevelParam()->get(), juce::dontSendNotification);
    levelSlider.addListener (this);
    levelSlider.setTextValueSuffix (" dB");
    levelSlider.setRotaryParameters (0.0f, juce::MathConstants<float>::twoPi, true);
    addAndMakeVisible (levelSlider);
    
    // === Настройка слайдера Intensity ===
    intensitySlider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    intensitySlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 24);
    intensitySlider.setRange (0.0f, 1.0f, 0.001f);
    intensitySlider.setValue (audioProcessor.getIntensityParam()->get(), juce::dontSendNotification);
    intensitySlider.addListener (this);
    intensitySlider.setRotaryParameters (0.0f, juce::MathConstants<float>::twoPi, true);
    addAndMakeVisible (intensitySlider);
    
    // === Настройка меток ===
    levelLabel.setText ("LEVEL", juce::dontSendNotification);
    levelLabel.setJustificationType (juce::Justification::centred);
    levelLabel.setFont (juce::Font (16.0f, juce::Font::bold));
    levelLabel.attachToComponent (&levelSlider, false);
    addAndMakeVisible (levelLabel);
    
    intensityLabel.setText ("INTENSITY", juce::dontSendNotification);
    intensityLabel.setJustificationType (juce::Justification::centred);
    intensityLabel.setFont (juce::Font (16.0f, juce::Font::bold));
    intensityLabel.attachToComponent (&intensitySlider, false);
    addAndMakeVisible (intensityLabel);
    
    setSize (400, 370);
}

BronzaAudioEditor::~BronzaAudioEditor()
{
    setLookAndFeel (nullptr);
}

void BronzaAudioEditor::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // Затемнённый бронзовый градиент для фона
    juce::ColourGradient mainGradient (
        juce::Colour (0xFF6B3A1A),  // Светлая часть (теперь темнее)
        (float)bounds.getX(), (float)bounds.getY(),
        juce::Colour (0xFF3A1A08),  // Тёмная часть (значительно темнее)
        (float)bounds.getX(), (float)bounds.getBottom(),
        false
    );
    g.setGradientFill (mainGradient);
    g.fillAll();
    
    // Металлический блик (оставляем для объёма)
    juce::ColourGradient shineGradient (
        juce::Colour (0x33D4A84C),
        (float)bounds.getX(), (float)bounds.getY() + bounds.getHeight() * 0.15f,
        juce::Colour (0x00D4A84C),
        (float)bounds.getX(), (float)bounds.getY() + bounds.getHeight() * 0.4f,
        false
    );
    g.setGradientFill (shineGradient);
    g.fillAll();
    
    // Рамка
    g.setColour (juce::Colour (0xFFC9A03D));
    g.drawRect (bounds, 2.0f);
    g.setColour (juce::Colour (0xFFE8C66A).withAlpha (0.3f));
    g.drawRect (bounds.reduced (2), 1.0f);
    
    // Название плагина
    juce::Rectangle<int> titleArea (bounds.getX(), bounds.getY(), bounds.getWidth(), 70);
    
    // Тень
    g.setColour (juce::Colours::black.withAlpha (0.6f));
    g.setFont (juce::Font (34.0f, juce::Font::bold));
    g.drawFittedText ("BRONZA", 
                     titleArea.getX() + 2, 
                     titleArea.getY() + 2,
                     titleArea.getWidth(),
                     55,
                     juce::Justification::centredTop, 
                     1);
    
    // Основной текст
    g.setColour (juce::Colour (0xFFF5D97A));
    g.drawFittedText ("BRONZA", 
                     titleArea.getX(), 
                     titleArea.getY(),
                     titleArea.getWidth(),
                     55,
                     juce::Justification::centredTop, 
                     1);
    
    // Декоративная линия
    g.setColour (juce::Colour (0xFFC9A03D).withAlpha (0.6f));
    g.drawLine (50.0f, 62.0f, (float)bounds.getWidth() - 50, 62.0f, 1.5f);
}

void BronzaAudioEditor::resized()
{
    auto area = getLocalBounds();
    auto titleArea = area.removeFromTop (75);
    auto sliderArea = area.reduced (30, 25);
    
    auto leftArea = sliderArea.removeFromLeft (sliderArea.getWidth() / 2);
    auto rightArea = sliderArea;
    
    levelSlider.setBounds (leftArea.reduced (15, 10));
    intensitySlider.setBounds (rightArea.reduced (15, 10));
}

void BronzaAudioEditor::sliderValueChanged (juce::Slider* slider)
{
    if (slider == &levelSlider)
    {
        audioProcessor.getLevelParam()->setValueNotifyingHost ((float) levelSlider.getValue());
        levelSlider.repaint();
    }
    else if (slider == &intensitySlider)
    {
        audioProcessor.getIntensityParam()->setValueNotifyingHost ((float) intensitySlider.getValue());
        intensitySlider.repaint();
    }
}