#include "MistralEditor.h"

MistralAudioEditor::MistralAudioEditor (MistralAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setLookAndFeel (&mistralLookAndFeel);
    
    // === Rate Slider (размер как в Bronza) ===
    rateSlider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    rateSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 24);
    rateSlider.setRange (0.0f, 1.0f, 0.001f);
    rateSlider.setRotaryParameters (0.0f, juce::MathConstants<float>::twoPi, true);
    addAndMakeVisible (rateSlider);
    
    // === Depth Slider ===
    depthSlider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    depthSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 24);
    depthSlider.setRange (0.0f, 1.0f, 0.001f);
    depthSlider.setRotaryParameters (0.0f, juce::MathConstants<float>::twoPi, true);
    addAndMakeVisible (depthSlider);
    
    // === Feedback Slider ===
    feedbackSlider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    feedbackSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 24);
    feedbackSlider.setRange (0.0f, 0.95f, 0.001f);
    feedbackSlider.setRotaryParameters (0.0f, juce::MathConstants<float>::twoPi, true);
    addAndMakeVisible (feedbackSlider);
    
    // Attachments
    rateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "rate", rateSlider);
    depthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "depth", depthSlider);
    feedbackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "feedback", feedbackSlider);
    
    // === Labels (шрифт как в Bronza: 16pt bold) ===
    rateLabel.setText ("RATE", juce::dontSendNotification);
    rateLabel.setJustificationType (juce::Justification::centred);
    rateLabel.setFont (juce::Font (16.0f, juce::Font::bold));
    rateLabel.attachToComponent (&rateSlider, false);
    addAndMakeVisible (rateLabel);
    
    depthLabel.setText ("DEPTH", juce::dontSendNotification);
    depthLabel.setJustificationType (juce::Justification::centred);
    depthLabel.setFont (juce::Font (16.0f, juce::Font::bold));
    depthLabel.attachToComponent (&depthSlider, false);
    addAndMakeVisible (depthLabel);
    
    feedbackLabel.setText ("FEEDBACK", juce::dontSendNotification);
    feedbackLabel.setJustificationType (juce::Justification::centred);
    feedbackLabel.setFont (juce::Font (16.0f, juce::Font::bold));
    feedbackLabel.attachToComponent (&feedbackSlider, false);
    addAndMakeVisible (feedbackLabel);
    
    // === ИЗМЕНЕНО: ширина окна под три кноба по 140 px ===
    setSize (510, 370);
}

MistralAudioEditor::~MistralAudioEditor()
{
    setLookAndFeel (nullptr);
}

void MistralAudioEditor::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // Бронзовый градиент (как в Bronza)
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
    
    // === Название плагина ===
    juce::Rectangle<int> titleArea (bounds.getX(), bounds.getY(), bounds.getWidth(), 70);
    
    // Тень
    g.setColour (juce::Colours::black.withAlpha (0.6f));
    g.setFont (juce::Font (34.0f, juce::Font::bold));
    g.drawFittedText ("MISTRAL", 
                     titleArea.getX() + 2, 
                     titleArea.getY() + 2,
                     titleArea.getWidth(),
                     50,
                     juce::Justification::centredTop, 
                     1);
    
    // Основной текст
    g.setColour (juce::Colour (0xFFF5D97A));
    g.drawFittedText ("MISTRAL", 
                     titleArea.getX(), 
                     titleArea.getY(),
                     titleArea.getWidth(),
                     50,
                     juce::Justification::centredTop, 
                     1);
    
    // === ПОДПИСЬ ===
    g.setColour (juce::Colour (0xFFC9A03D).withAlpha (0.8f));
    g.setFont (juce::Font (14.0f, juce::Font::italic));
    g.drawFittedText ("flanger by Peter Semiletov", 
                     bounds.getX(), 
                     titleArea.getY() + 45,
                     bounds.getWidth(),
                     20,
                     juce::Justification::centredTop, 
                     1);
    
    // Декоративная линия (адаптирована под ширину)
    g.setColour (juce::Colour (0xFFC9A03D).withAlpha (0.6f));
    g.drawLine (50.0f, 70.0f, (float)bounds.getWidth() - 50, 70.0f, 1.5f);
}

void MistralAudioEditor::resized()
{
    auto area = getLocalBounds();
    auto titleArea = area.removeFromTop (80);      // Как в Bronza
    auto sliderArea = area.reduced (30, 25);       // Отступы как в Bronza
    
    // Три слайдера одинаковой ширины (каждый будет ~140 px)
    int sliderWidth = sliderArea.getWidth() / 3;
    
    auto leftArea = sliderArea.removeFromLeft (sliderWidth);
    auto middleArea = sliderArea.removeFromLeft (sliderWidth);
    auto rightArea = sliderArea;
    
    // Каждый слайдер получает такие же внутренние отступы, как в Bronza (15, 10)
    rateSlider.setBounds (leftArea.reduced (15, 10));
    depthSlider.setBounds (middleArea.reduced (15, 10));
    feedbackSlider.setBounds (rightArea.reduced (15, 10));
}