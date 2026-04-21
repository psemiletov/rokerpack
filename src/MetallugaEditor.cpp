#include "MetallugaEditor.h"

MetallugaAudioEditor::MetallugaAudioEditor (MetallugaAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setLookAndFeel (&metallugaLookAndFeel);
    
  // Gate
gateSlider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
gateSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 24);
gateSlider.setRange (0.0f, 0.05f, 0.0001f);  // ← шаг уменьшен с 0.001 до 0.0001
gateSlider.setValue (0.005f, juce::dontSendNotification);
gateSlider.setRotaryParameters (0.0f, juce::MathConstants<float>::twoPi, true);
addAndMakeVisible (gateSlider);

    // Drive
    driveSlider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    driveSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 24);
    driveSlider.setRange (0.0f, 1.0f, 0.001f);
    driveSlider.setRotaryParameters (0.0f, juce::MathConstants<float>::twoPi, true);
    addAndMakeVisible (driveSlider);
    
    // Level
    levelSlider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    levelSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 24);
    levelSlider.setRange (0.0f, 32.0f, 0.1f);
    levelSlider.setTextValueSuffix (" dB");
    levelSlider.setRotaryParameters (0.0f, juce::MathConstants<float>::twoPi, true);
    addAndMakeVisible (levelSlider);
    
    // Attachments
    gateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "gate", gateSlider);
    driveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "drive", driveSlider);
    levelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "level", levelSlider);
    
    // Labels
    gateLabel.setText ("GATE", juce::dontSendNotification);
    gateLabel.setJustificationType (juce::Justification::centred);
    gateLabel.setFont (juce::Font (14.0f, juce::Font::bold));
    gateLabel.attachToComponent (&gateSlider, false);
    addAndMakeVisible (gateLabel);
    
    driveLabel.setText ("DRIVE", juce::dontSendNotification);
    driveLabel.setJustificationType (juce::Justification::centred);
    driveLabel.setFont (juce::Font (14.0f, juce::Font::bold));
    driveLabel.attachToComponent (&driveSlider, false);
    addAndMakeVisible (driveLabel);
    
    levelLabel.setText ("LEVEL", juce::dontSendNotification);
    levelLabel.setJustificationType (juce::Justification::centred);
    levelLabel.setFont (juce::Font (14.0f, juce::Font::bold));
    levelLabel.attachToComponent (&levelSlider, false);
    addAndMakeVisible (levelLabel);
    
    setSize (DEFAULT_WIDTH, DEFAULT_HEIGHT);
}

MetallugaAudioEditor::~MetallugaAudioEditor()
{
    setLookAndFeel (nullptr);
}

void MetallugaAudioEditor::paint (juce::Graphics& g)
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
    
    g.setColour (juce::Colour (0xFFC9A03D));
    g.drawRect (bounds, 2.0f);
    g.setColour (juce::Colour (0xFFE8C66A).withAlpha (0.3f));
    g.drawRect (bounds.reduced (2), 1.0f);
    
    // Название
    juce::Rectangle<int> titleArea (bounds.getX(), bounds.getY(), bounds.getWidth(), 70);
    
    // Тень
    g.setColour (juce::Colours::black.withAlpha (0.6f));
    g.setFont (juce::Font (34.0f, juce::Font::bold));
    g.drawFittedText ("METALLUGA", 
                     titleArea.getX() + 2, 
                     titleArea.getY() + 2,
                     titleArea.getWidth(),
                     50,
                     juce::Justification::centredTop, 
                     1);
    
    // Основной текст
    g.setColour (juce::Colour (0xFFF5D97A));
    g.drawFittedText ("METALLUGA", 
                     titleArea.getX(), 
                     titleArea.getY(),
                     titleArea.getWidth(),
                     50,
                     juce::Justification::centredTop, 
                     1);
    
    // Подпись
    g.setColour (juce::Colour (0xFFC9A03D).withAlpha (0.8f));
    g.setFont (juce::Font (14.0f, juce::Font::italic));
    g.drawFittedText ("distortion by Peter Semiletov", 
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

void MetallugaAudioEditor::resized()
{
    auto area = getLocalBounds();
    area.removeFromTop (80);
    auto sliderArea = area.reduced (30, 25);
    
    int sliderWidth = sliderArea.getWidth() / 3;
    
    auto gateArea = sliderArea.removeFromLeft (sliderWidth);
    auto driveArea = sliderArea.removeFromLeft (sliderWidth);
    auto levelArea = sliderArea;
    
    gateSlider.setBounds (gateArea.reduced (15, 10));
    driveSlider.setBounds (driveArea.reduced (15, 10));
    levelSlider.setBounds (levelArea.reduced (15, 10));
}