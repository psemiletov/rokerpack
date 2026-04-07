#include "MistralEditor.h"

MistralAudioEditor::MistralAudioEditor (MistralAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setLookAndFeel (&mistralLookAndFeel);
    
    // Rate Slider
    rateSlider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    rateSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 24);
    rateSlider.setRange (0.0f, 1.0f, 0.001f);
    addAndMakeVisible (rateSlider);
    
    // Depth Slider
    depthSlider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    depthSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 24);
    depthSlider.setRange (0.0f, 1.0f, 0.001f);
    addAndMakeVisible (depthSlider);
    
    // Feedback Slider
    feedbackSlider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    feedbackSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 24);
    //feedbackSlider.setRange (0.0f, 1.0f, 0.001f);
    //feedbackSlider.setRange (0.0f, 0.8f, 0.001f); 
    feedbackSlider.setRange (0.0f, 0.95f, 0.001f);  // Расширен до 0.95
    addAndMakeVisible (feedbackSlider);
    
    // Attachments
    rateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "rate", rateSlider);
    depthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "depth", depthSlider);
    feedbackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "feedback", feedbackSlider);
    
    // Labels
    rateLabel.setText ("RATE", juce::dontSendNotification);
    rateLabel.setJustificationType (juce::Justification::centred);
    rateLabel.setFont (juce::Font (14.0f, juce::Font::bold));
    rateLabel.attachToComponent (&rateSlider, false);
    addAndMakeVisible (rateLabel);
    
    depthLabel.setText ("DEPTH", juce::dontSendNotification);
    depthLabel.setJustificationType (juce::Justification::centred);
    depthLabel.setFont (juce::Font (14.0f, juce::Font::bold));
    depthLabel.attachToComponent (&depthSlider, false);
    addAndMakeVisible (depthLabel);
    
    feedbackLabel.setText ("FEEDBACK", juce::dontSendNotification);
    feedbackLabel.setJustificationType (juce::Justification::centred);
    feedbackLabel.setFont (juce::Font (14.0f, juce::Font::bold));
    feedbackLabel.attachToComponent (&feedbackSlider, false);
    addAndMakeVisible (feedbackLabel);
    
    setSize (600, 370);
}

MistralAudioEditor::~MistralAudioEditor()
{
    setLookAndFeel (nullptr);
}

void MistralAudioEditor::paint (juce::Graphics& g)
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
    
    juce::Rectangle<int> titleArea (bounds.getX(), bounds.getY(), bounds.getWidth(), 70);
    
    g.setColour (juce::Colours::black.withAlpha (0.6f));
    g.setFont (juce::Font (34.0f, juce::Font::bold));
    g.drawFittedText ("MISTRAL", 
                     titleArea.getX() + 2, 
                     titleArea.getY() + 2,
                     titleArea.getWidth(),
                     55,
                     juce::Justification::centredTop, 
                     1);
    
    g.setColour (juce::Colour (0xFFF5D97A));
    g.drawFittedText ("MISTRAL", 
                     titleArea.getX(), 
                     titleArea.getY(),
                     titleArea.getWidth(),
                     55,
                     juce::Justification::centredTop, 
                     1);
    
    g.setColour (juce::Colour (0xFFC9A03D).withAlpha (0.6f));
    g.drawLine (50.0f, 62.0f, (float)bounds.getWidth() - 50, 62.0f, 1.5f);
}

void MistralAudioEditor::resized()
{
    auto area = getLocalBounds();
    auto titleArea = area.removeFromTop (75);
    auto sliderArea = area.reduced (30, 25);
    
    int sliderWidth = sliderArea.getWidth() / 3;
    
    rateSlider.setBounds (sliderArea.removeFromLeft (sliderWidth).reduced (15, 10));
    depthSlider.setBounds (sliderArea.removeFromLeft (sliderWidth).reduced (15, 10));
    feedbackSlider.setBounds (sliderArea.reduced (15, 10));
}