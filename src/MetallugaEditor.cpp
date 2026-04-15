#include "MetallugaEditor.h"

MetallugaAudioEditor::MetallugaAudioEditor (MetallugaAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setLookAndFeel (&metallugaLookAndFeel);
    
    // Drive
    driveSlider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    driveSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 24);
    driveSlider.setRange (0.0f, 1.0f, 0.001f);
    driveSlider.setRotaryParameters (0.0f, juce::MathConstants<float>::twoPi, true);
    addAndMakeVisible (driveSlider);
    
    // Level (0..62 dB)
    levelSlider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    levelSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 24);
    levelSlider.setRange (0.0f, 62.0f, 0.1f);
    levelSlider.setTextValueSuffix (" dB");
    levelSlider.setRotaryParameters (0.0f, juce::MathConstants<float>::twoPi, true);
    addAndMakeVisible (levelSlider);
    
    // Weight
    weightSlider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    weightSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 24);
    weightSlider.setRange (0.01f, 0.99f, 0.001f);
    weightSlider.setRotaryParameters (0.0f, juce::MathConstants<float>::twoPi, true);
    addAndMakeVisible (weightSlider);
    
    // Aggro (было Resonance)
    aggroSlider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    aggroSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 24);
    aggroSlider.setRange (0.01f, 0.99f, 0.001f);
    aggroSlider.setRotaryParameters (0.0f, juce::MathConstants<float>::twoPi, true);
    addAndMakeVisible (aggroSlider);
    
    // Warmth
    warmthSlider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    warmthSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 24);
    warmthSlider.setRange (0.01f, 0.99f, 0.001f);
    warmthSlider.setRotaryParameters (0.0f, juce::MathConstants<float>::twoPi, true);
    addAndMakeVisible (warmthSlider);
    
    // Attachments
    driveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "drive", driveSlider);
    levelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "level", levelSlider);
    weightAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "weight", weightSlider);
    aggroAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "aggro", aggroSlider);  // ← параметр теперь "aggro"
    warmthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "warmth", warmthSlider);
    
    // Labels
    driveLabel.setText ("DRIVE", juce::dontSendNotification);
    driveLabel.setJustificationType (juce::Justification::centred);
    driveLabel.setFont (juce::Font (12.0f, juce::Font::bold));
    driveLabel.attachToComponent (&driveSlider, false);
    addAndMakeVisible (driveLabel);
    
    levelLabel.setText ("LEVEL", juce::dontSendNotification);
    levelLabel.setJustificationType (juce::Justification::centred);
    levelLabel.setFont (juce::Font (12.0f, juce::Font::bold));
    levelLabel.attachToComponent (&levelSlider, false);
    addAndMakeVisible (levelLabel);
    
    weightLabel.setText ("WEIGHT", juce::dontSendNotification);
    weightLabel.setJustificationType (juce::Justification::centred);
    weightLabel.setFont (juce::Font (12.0f, juce::Font::bold));
    weightLabel.attachToComponent (&weightSlider, false);
    addAndMakeVisible (weightLabel);
    
    aggroLabel.setText ("AGGRO", juce::dontSendNotification);
    aggroLabel.setJustificationType (juce::Justification::centred);
    aggroLabel.setFont (juce::Font (12.0f, juce::Font::bold));
    aggroLabel.attachToComponent (&aggroSlider, false);
    addAndMakeVisible (aggroLabel);
    
    warmthLabel.setText ("WARMTH", juce::dontSendNotification);
    warmthLabel.setJustificationType (juce::Justification::centred);
    warmthLabel.setFont (juce::Font (12.0f, juce::Font::bold));
    warmthLabel.attachToComponent (&warmthSlider, false);
    addAndMakeVisible (warmthLabel);
    
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
    auto titleArea = area.removeFromTop (80);
    auto sliderArea = area.reduced (20, 25);
    
    int sliderWidth = sliderArea.getWidth() / 5;
    
    driveSlider.setBounds (sliderArea.removeFromLeft (sliderWidth).reduced (10, 10));
    levelSlider.setBounds (sliderArea.removeFromLeft (sliderWidth).reduced (10, 10));
    weightSlider.setBounds (sliderArea.removeFromLeft (sliderWidth).reduced (10, 10));
    aggroSlider.setBounds (sliderArea.removeFromLeft (sliderWidth).reduced (10, 10));
    warmthSlider.setBounds (sliderArea.reduced (10, 10));
}