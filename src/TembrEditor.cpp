#include "TembrEditor.h"

TembrAudioEditor::TembrAudioEditor (TembrAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setLookAndFeel (&tembrLookAndFeel);
    
    // Lows Slider
    lowsSlider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    lowsSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 24);
    lowsSlider.setRange (-12.0f, 12.0f);  // без шага
    lowsSlider.setTextValueSuffix (" dB");
    addAndMakeVisible (lowsSlider);
    
    // Treble Slider
    trebleSlider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    trebleSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 24);
    trebleSlider.setRange (-12.0f, 12.0f);  // без шага
    trebleSlider.setTextValueSuffix (" dB");
    addAndMakeVisible (trebleSlider);
    
    // Attachments
    lowsAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "lows", lowsSlider);
    trebleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "treble", trebleSlider);
    
    // Labels
    lowsLabel.setText ("LOWS", juce::dontSendNotification);
    lowsLabel.setJustificationType (juce::Justification::centred);
    lowsLabel.setFont (juce::Font (14.0f, juce::Font::bold));
    lowsLabel.attachToComponent (&lowsSlider, false);
    addAndMakeVisible (lowsLabel);
    
    trebleLabel.setText ("TREBLE", juce::dontSendNotification);
    trebleLabel.setJustificationType (juce::Justification::centred);
    trebleLabel.setFont (juce::Font (14.0f, juce::Font::bold));
    trebleLabel.attachToComponent (&trebleSlider, false);
    addAndMakeVisible (trebleLabel);
    
    setSize (500, 370);
}

TembrAudioEditor::~TembrAudioEditor()
{
    setLookAndFeel (nullptr);
}

void TembrAudioEditor::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
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
    
    g.setColour (juce::Colour (0xFFC9A03D));
    g.drawRect (bounds, 2.0f);
    g.setColour (juce::Colour (0xFFE8C66A).withAlpha (0.3f));
    g.drawRect (bounds.reduced (2), 1.0f);
    
    // Название плагина
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
    
    // === ПОДПИСЬ ===
    g.setColour (juce::Colour (0xFFC9A03D).withAlpha (0.8f));
    g.setFont (juce::Font (14.0f, juce::Font::italic));
    g.drawFittedText ("equalizer by Peter Semiletov", 
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

void TembrAudioEditor::resized()
{
    auto area = getLocalBounds();
    auto titleArea = area.removeFromTop (80);
    auto sliderArea = area.reduced (30, 25);
    
    // Два слайдера в ряд
    int sliderWidth = sliderArea.getWidth() / 2;
    
    lowsSlider.setBounds (sliderArea.removeFromLeft (sliderWidth).reduced (15, 10));
    trebleSlider.setBounds (sliderArea.reduced (15, 10));
}