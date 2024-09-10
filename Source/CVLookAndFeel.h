#pragma once
#include <JuceHeader.h>

class CVLookAndFeel : public juce::LookAndFeel_V4
{
public:
    juce::Rectangle<int> getPropertyComponentContentPosition(PropertyComponent&) override;
};