#pragma once
#include <JuceHeader.h>
#include "SettingsComponent.h"

class SettingsWindow : public juce::DocumentWindow
{
public:
	SettingsWindow(juce::ValueTree& properties, 
				   std::vector<ParameterInfo>& parameterInfos);
private:
	SettingsComponent component;
	void closeButtonPressed() override;
};
