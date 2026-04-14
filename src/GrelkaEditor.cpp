#include "GrelkaEditor.h"

GrelkaAudioEditor::GrelkaAudioEditor (GrelkaAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setLookAndFeel (&grelkaLookAndFeel);
    
// Drive
driveSlider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
driveSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 24);
driveSlider.setRange (0.01f, 1.0f, 0.001f);
driveSlider.setRotaryParameters (0.0f, juce::MathConstants<float>::twoPi, true);  // ДОБАВИТЬ
addAndMakeVisible (driveSlider);

// Level — только положительный (0 до 48 dB, где 0 dB — это ноль громкости)
levelSlider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
levelSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 24);
levelSlider.setRange (0.0f, 48.0f, 0.1f);  // 0..48 dB
levelSlider.setTextValueSuffix (" dB");
levelSlider.setRotaryParameters (0.0f, juce::MathConstants<float>::twoPi, true);
addAndMakeVisible (levelSlider);


// Lows
lowsSlider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
lowsSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 24);
lowsSlider.setRange (40.0f, 1000.0f, 1.0f);
lowsSlider.setTextValueSuffix (" Hz");
lowsSlider.setRotaryParameters (0.0f, juce::MathConstants<float>::twoPi, true);  // ДОБАВИТЬ
addAndMakeVisible (lowsSlider);

// Treble
trebleSlider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
trebleSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 24);
trebleSlider.setRange (500.0f, 16500.0f, 10.0f);
trebleSlider.setTextValueSuffix (" Hz");
trebleSlider.setRotaryParameters (0.0f, juce::MathConstants<float>::twoPi, true);  // ДОБАВИТЬ
addAndMakeVisible (trebleSlider);

// Attachments
    driveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "drive", driveSlider);
    levelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "level", levelSlider);
    lowsAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "lows", lowsSlider);
    trebleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "treble", trebleSlider);
    
    // Метки
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
    
    lowsLabel.setText ("LOW CUT", juce::dontSendNotification);
    lowsLabel.setJustificationType (juce::Justification::centred);
    lowsLabel.setFont (juce::Font (14.0f, juce::Font::bold));
    lowsLabel.attachToComponent (&lowsSlider, false);
    addAndMakeVisible (lowsLabel);
    
    trebleLabel.setText ("HIGH CUT", juce::dontSendNotification);
    trebleLabel.setJustificationType (juce::Justification::centred);
    trebleLabel.setFont (juce::Font (14.0f, juce::Font::bold));
    trebleLabel.attachToComponent (&trebleSlider, false);
    addAndMakeVisible (trebleLabel);
    
    setSize (600, 370);
}

GrelkaAudioEditor::~GrelkaAudioEditor()
{
    setLookAndFeel (nullptr);
}

void GrelkaAudioEditor::paint (juce::Graphics& g)
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
    g.drawFittedText ("GRELKA", 
                     titleArea.getX() + 2, 
                     titleArea.getY() + 2,
                     titleArea.getWidth(),
                     50,
                     juce::Justification::centredTop, 
                     1);
    
    // Основной текст
    g.setColour (juce::Colour (0xFFF5D97A));
    g.drawFittedText ("GRELKA", 
                     titleArea.getX(), 
                     titleArea.getY(),
                     titleArea.getWidth(),
                     50,
                     juce::Justification::centredTop, 
                     1);
    
    // === ПОДПИСЬ ===
    g.setColour (juce::Colour (0xFFC9A03D).withAlpha (0.8f));
    g.setFont (juce::Font (14.0f, juce::Font::italic));
    g.drawFittedText ("Overdrive by Peter Semiletov", 
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

void GrelkaAudioEditor::resized()
{
    auto area = getLocalBounds();
    auto titleArea = area.removeFromTop (80);
    auto sliderArea = area.reduced (20, 25);
    
    int sliderWidth = sliderArea.getWidth() / 4;
    
    driveSlider.setBounds (sliderArea.removeFromLeft (sliderWidth).reduced (10, 10));
    levelSlider.setBounds (sliderArea.removeFromLeft (sliderWidth).reduced (10, 10));
    lowsSlider.setBounds (sliderArea.removeFromLeft (sliderWidth).reduced (10, 10));
    trebleSlider.setBounds (sliderArea.reduced (10, 10));
}