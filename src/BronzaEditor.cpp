#include "BronzaEditor.h"

BronzaAudioEditor::BronzaAudioEditor (BronzaAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setLookAndFeel (&bronzaLookAndFeel);
    
    // === Настройка слайдера Level ===
    levelSlider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    levelSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 24);  // Увеличен текстбокс
    levelSlider.setRange (0.0f, 1.0f, 0.001f);
    levelSlider.setValue (audioProcessor.getLevelParam()->get(), juce::dontSendNotification);
    levelSlider.addListener (this);
    levelSlider.setTextValueSuffix (" dB");
    levelSlider.setRotaryParameters (0.0f, juce::MathConstants<float>::twoPi, true);
    addAndMakeVisible (levelSlider);
    
    // === Настройка слайдера Intensity ===
    intensitySlider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    intensitySlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 24);  // Увеличен текстбокс
    intensitySlider.setRange (0.0f, 1.0f, 0.001f);
    intensitySlider.setValue (audioProcessor.getIntensityParam()->get(), juce::dontSendNotification);
    intensitySlider.addListener (this);
    intensitySlider.setRotaryParameters (0.0f, juce::MathConstants<float>::twoPi, true);
    addAndMakeVisible (intensitySlider);
    
    // === Настройка меток (увеличенный шрифт) ===
    levelLabel.setText ("LEVEL", juce::dontSendNotification);
    levelLabel.setJustificationType (juce::Justification::centred);
    levelLabel.setFont (juce::Font (16.0f, juce::Font::bold));  // Увеличен шрифт
    levelLabel.attachToComponent (&levelSlider, false);
    addAndMakeVisible (levelLabel);
    
    intensityLabel.setText ("INTENSITY", juce::dontSendNotification);
    intensityLabel.setJustificationType (juce::Justification::centred);
    intensityLabel.setFont (juce::Font (16.0f, juce::Font::bold));  // Увеличен шрифт
    intensityLabel.attachToComponent (&intensitySlider, false);
    addAndMakeVisible (intensityLabel);
    
    setSize (400, 370);  // Немного увеличили окно
}

BronzaAudioEditor::~BronzaAudioEditor()
{
    setLookAndFeel (nullptr);
}

void BronzaAudioEditor::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // Основной градиент с цветом #472B15
    juce::ColourGradient mainGradient (
        juce::Colour (0xFF472B15),  // Основной цвет
        (float)bounds.getX(), (float)bounds.getY(),
        juce::Colour (0xFF2A180A),  // Темнее
        (float)bounds.getX(), (float)bounds.getBottom(),
        false
    );
    g.setGradientFill (mainGradient);
    g.fillAll();
    
    // Металлический блик
    juce::ColourGradient shineGradient (
        juce::Colour (0x33CD7F32),
        (float)bounds.getX(), (float)bounds.getY() + bounds.getHeight() * 0.2f,
        juce::Colour (0x00CD7F32),
        (float)bounds.getX(), (float)bounds.getY() + bounds.getHeight() * 0.5f,
        false
    );
    g.setGradientFill (shineGradient);
    g.fillAll();
    
    // Рамка
    g.setColour (juce::Colour (0xFFCD7F32));
    g.drawRect (bounds, 2.0f);
    g.setColour (juce::Colour (0xFFE8A95B).withAlpha (0.3f));
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
    g.setColour (juce::Colour (0xFFF4C77A));
    g.drawFittedText ("BRONZA", 
                     titleArea.getX(), 
                     titleArea.getY(),
                     titleArea.getWidth(),
                     55,
                     juce::Justification::centredTop, 
                     1);
    
    // Декоративная линия
    g.setColour (juce::Colour (0xFFCD7F32).withAlpha (0.5f));
    g.drawLine (50.0f, 62.0f, (float)bounds.getWidth() - 50, 62.0f, 1.5f);
    
    // Бронзовый индикатор
    juce::ColourGradient ledGradient (
        juce::Colour (0xFFF4C77A),
        (float)bounds.getRight() - 20, (float)bounds.getBottom() - 20,
        juce::Colour (0xFF8B5A2B),
        (float)bounds.getRight() - 16, (float)bounds.getBottom() - 16,
        true
    );
    g.setGradientFill (ledGradient);
    g.fillEllipse ((float)bounds.getRight() - 20, (float)bounds.getBottom() - 20, 10.0f, 10.0f);
    g.setColour (juce::Colours::white.withAlpha (0.4f));
    g.fillEllipse ((float)bounds.getRight() - 18, (float)bounds.getBottom() - 18, 3.0f, 3.0f);
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