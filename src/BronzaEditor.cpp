#include "BronzaEditor.h"

BronzaAudioEditor::BronzaAudioEditor (BronzaAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Настройка слайдера Level
    levelSlider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    levelSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    levelSlider.setRange (0.0f, 1.0f, 0.001f);
    levelSlider.setValue (audioProcessor.getLevelParam()->get(), juce::dontSendNotification);
    levelSlider.addListener (this);
    addAndMakeVisible (levelSlider);
    
    // Настройка слайдера Intensity
    intensitySlider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    intensitySlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    intensitySlider.setRange (0.0f, 1.0f, 0.001f);
    intensitySlider.setValue (audioProcessor.getIntensityParam()->get(), juce::dontSendNotification);
    intensitySlider.addListener (this);
    addAndMakeVisible (intensitySlider);
    
    // Лейблы
    levelLabel.setText ("Level", juce::dontSendNotification);
    levelLabel.attachToComponent (&levelSlider, false);
    addAndMakeVisible (levelLabel);
    
    intensityLabel.setText ("Intensity", juce::dontSendNotification);
    intensityLabel.attachToComponent (&intensitySlider, false);
    addAndMakeVisible (intensityLabel);
    
    setSize (400, 300);
}

BronzaAudioEditor::~BronzaAudioEditor()
{
}

void BronzaAudioEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::darkgrey);
}

void BronzaAudioEditor::resized()
{
    auto area = getLocalBounds().reduced (20);
    auto sliderArea = area;
    levelSlider.setBounds (sliderArea.removeFromLeft (area.getWidth() / 2).reduced (10));
    intensitySlider.setBounds (sliderArea.reduced (10));
}

void BronzaAudioEditor::sliderValueChanged (juce::Slider* slider)
{
    if (slider == &levelSlider)
    {
        audioProcessor.getLevelParam()->setValueNotifyingHost ((float) levelSlider.getValue());
    }
    else if (slider == &intensitySlider)
    {
        audioProcessor.getIntensityParam()->setValueNotifyingHost ((float) intensitySlider.getValue());
    }
}