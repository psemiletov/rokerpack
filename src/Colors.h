#pragma once
#include <JuceHeader.h>

namespace Colors
{
    // Точные цвета из BronzaLookAndFeel
    inline const juce::Colour bgDark          (0xFF4A2A12);
    inline const juce::Colour bgLight         (0xFF7A4F2A);
    inline const juce::Colour textLight       (0xFFF0D8B0);
    inline const juce::Colour brassDark       (0xFF8B6914);
    inline const juce::Colour brassMid        (0xFFC9A03D);
    inline const juce::Colour brassLight      (0xFFE8C66A);
    inline const juce::Colour brassHighlight  (0xFFF5D97A);
    
    // Специфичные для тюнера (добавляем к палитре)
    inline const juce::Colour ledRed          (0xFFFF3333);
    inline const juce::Colour ledOff          (0xFF442222);
    inline const juce::Colour meterBackground (0xFF2A1A08);
    inline const juce::Colour tooQuietYellow  (0xFFFFAA33);
    inline const juce::Colour clipRed         (0xFFFF6666);
    inline const juce::Colour wrongStringOrange(0xFFDD8844);
    
    inline const juce::Colour buttonBackground (0xFF8B6914);
    inline const juce::Colour buttonText (0xFFF5D97A);
}