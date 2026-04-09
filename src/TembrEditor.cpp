#include "TembrEditor.h"

TembrAudioEditor::TembrAudioEditor (TembrAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setLookAndFeel (&tembrLookAndFeel);
    
    // === Lows Slider (размер и стиль как в Bronza) ===
    lowsSlider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    lowsSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 24);
    lowsSlider.setRange (-12.0f, 12.0f);
    lowsSlider.setTextValueSuffix (" dB");
    lowsSlider.setRotaryParameters (0.0f, juce::MathConstants<float>::twoPi, true);  // Добавлено: полный круг как в Bronza
    addAndMakeVisible (lowsSlider);
    
    // === Treble Slider ===
    trebleSlider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    trebleSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 24);
    trebleSlider.setRange (-12.0f, 12.0f);
    trebleSlider.setTextValueSuffix (" dB");
    trebleSlider.setRotaryParameters (0.0f, juce::MathConstants<float>::twoPi, true);  // Добавлено
    addAndMakeVisible (trebleSlider);
    
    // Attachments
    lowsAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "lows", lowsSlider);
    trebleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "treble", trebleSlider);
    
    // === Labels (шрифт как в Bronza: 16pt bold вместо 14pt) ===
    lowsLabel.setText ("LOWS", juce::dontSendNotification);
    lowsLabel.setJustificationType (juce::Justification::centred);
    lowsLabel.setFont (juce::Font (16.0f, juce::Font::bold));  // Изменено: 14 → 16
    lowsLabel.attachToComponent (&lowsSlider, false);
    addAndMakeVisible (lowsLabel);
    
    trebleLabel.setText ("TREBLE", juce::dontSendNotification);
    trebleLabel.setJustificationType (juce::Justification::centred);
    trebleLabel.setFont (juce::Font (16.0f, juce::Font::bold));  // Изменено: 14 → 16
    trebleLabel.attachToComponent (&trebleSlider, false);
    addAndMakeVisible (trebleLabel);
    
    // === ИЗМЕНЕНО: размер окна как в Bronza (400 x 370) ===
    setSize (400, 370);
}

TembrAudioEditor::~TembrAudioEditor()
{
    setLookAndFeel (nullptr);
}

void TembrAudioEditor::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // Бронзовый градиент (полностью скопирован из Bronza)
    juce::ColourGradient mainGradient (
        juce::Colour (0xFF6B3A1A),
        (float)bounds.getX(), (float)bounds.getY(),
        juce::Colour (0xFF3A1A08),
        (float)bounds.getX(), (float)bounds.getBottom(),
        false
    );
    g.setGradientFill (mainGradient);
    g.fillAll();
    
    // Металлический блик
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
    
    // === Название плагина (позиция как в Bronza: высота 70, отступ сверху) ===
    juce::Rectangle<int> titleArea (bounds.getX(), bounds.getY(), bounds.getWidth(), 70);
    
    // Тень
    g.setColour (juce::Colours::black.withAlpha (0.6f));
    g.setFont (juce::Font (34.0f, juce::Font::bold));
    g.drawFittedText ("TEMBR", 
                     titleArea.getX() + 2, 
                     titleArea.getY() + 2,
                     titleArea.getWidth(),
                     50,
                     juce::Justification::centredTop, 
                     1);
    
    // Основной текст
    g.setColour (juce::Colour (0xFFF5D97A));
    g.drawFittedText ("TEMBR", 
                     titleArea.getX(), 
                     titleArea.getY(),
                     titleArea.getWidth(),
                     50,
                     juce::Justification::centredTop, 
                     1);
    
    // === ПОДПИСЬ (как в Bronza: y = titleArea.getY() + 45) ===
    g.setColour (juce::Colour (0xFFC9A03D).withAlpha (0.8f));
    g.setFont (juce::Font (14.0f, juce::Font::italic));
    g.drawFittedText ("equalizer by Peter Semiletov", 
                     bounds.getX(), 
                     titleArea.getY() + 45,
                     bounds.getWidth(),
                     20,
                     juce::Justification::centredTop, 
                     1);
    
    // Декоративная линия
    g.setColour (juce::Colour (0xFFC9A03D).withAlpha (0.6f));
    g.drawLine (50.0f, 70.0f, (float)bounds.getWidth() - 50, 70.0f, 1.5f);
}

void TembrAudioEditor::resized()
{
    auto area = getLocalBounds();
    auto titleArea = area.removeFromTop (80);      // Как в Bronza: 80 px под заголовок
    auto sliderArea = area.reduced (30, 25);       // Отступы как в Bronza: 30 по бокам, 25 сверху/снизу
    
    // Два слайдера в ряд, каждый как в Bronza: reduced(15, 10)
    auto leftArea = sliderArea.removeFromLeft (sliderArea.getWidth() / 2);
    auto rightArea = sliderArea;
    
    lowsSlider.setBounds (leftArea.reduced (15, 10));   // Точные отступы как в Bronza
    trebleSlider.setBounds (rightArea.reduced (15, 10));
}