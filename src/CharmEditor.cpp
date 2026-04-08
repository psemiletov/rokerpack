#include "CharmEditor.h"

CharmAudioEditor::CharmAudioEditor (CharmAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setLookAndFeel (&charmLookAndFeel);
    
    // Charm Slider
    charmSlider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    charmSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 24);
    charmSlider.setRange (0.0f, 1.0f, 0.001f);
    addAndMakeVisible (charmSlider);
    
    // Attachment
    charmAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "charm", charmSlider);
    
    // Label
    charmLabel.setText ("CHARM", juce::dontSendNotification);
    charmLabel.setJustificationType (juce::Justification::centred);
    charmLabel.setFont (juce::Font (16.0f, juce::Font::bold));
    charmLabel.attachToComponent (&charmSlider, false);
    addAndMakeVisible (charmLabel);
    
    setSize (300, 370);
}

CharmAudioEditor::~CharmAudioEditor()
{
    setLookAndFeel (nullptr);
}

void CharmAudioEditor::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // Градиентный фон
    juce::ColourGradient mainGradient (
        juce::Colour (0xFF6B3A1A),
        (float)bounds.getX(), (float)bounds.getY(),
        juce::Colour (0xFF3A1A08),
        (float)bounds.getX(), (float)bounds.getBottom(),
        false
    );
    g.setGradientFill (mainGradient);
    g.fillAll();
    
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
    g.drawFittedText ("CHARM", 
                     titleArea.getX() + 2, 
                     titleArea.getY() + 2,
                     titleArea.getWidth(),
                     50,
                     juce::Justification::centredTop, 
                     1);
    
    // Основной текст
    g.setColour (juce::Colour (0xFFF5D97A));
    g.drawFittedText ("CHARM", 
                     titleArea.getX(), 
                     titleArea.getY(),
                     titleArea.getWidth(),
                     50,
                     juce::Justification::centredTop, 
                     1);
    
  // === ПОДПИСЬ ===
g.setColour (juce::Colour (0xFFC9A03D).withAlpha (0.8f));
g.setFont (juce::Font (14.0f, juce::Font::italic));
g.drawFittedText ("analog saturator by Peter Semiletov", 
                 bounds.getX(), 
                 titleArea.getY() + 45,
                 bounds.getWidth(),
                 20,
                 juce::Justification::centredTop, 
                 1);
    
    // Декоративная линия (опущена ниже)
    g.setColour (juce::Colour (0xFFC9A03D).withAlpha (0.6f));
    g.drawLine (50.0f, 70.0f, (float)bounds.getWidth() - 50, 70.0f, 1.5f);
}

void CharmAudioEditor::resized()
{
    auto area = getLocalBounds();
    auto titleArea = area.removeFromTop (80);
    auto sliderArea = area.reduced (30, 25);
    
    // Кноб занимает всю доступную область с отступами
    charmSlider.setBounds (sliderArea.reduced (15, 10));
}