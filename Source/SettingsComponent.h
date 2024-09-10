#pragma once
#include <JuceHeader.h>
#include "ParameterInfo.h"

class SettingsComponent : public juce::Component
{
public:
	SettingsComponent(Component* parent, 
		              juce::ValueTree& initProperties,
					  std::vector<ParameterInfo>& parameterInfos);
	~SettingsComponent();
	void cancel();

private:
	const int width = 600,
			  margin = 20,
			  buttonWidth = width / 2 - margin * 2,
			  componentHeight = 30;

	Component* parent;
	
	juce::PropertyPanel panel;
	juce::TextButton okButton, cancelButton;
	
	juce::ValueTree initProperties;
	juce::ValueTree editedProperties;

	void okButtonClicked();
	juce::Array<juce::PropertyComponent*> createPropertyComponents(
		juce::ValueTree& initProperties, std::vector<ParameterInfo>& parameterInfos);
	juce::PropertyComponent* findPropComponent(
		juce::Array<juce::PropertyComponent*>& propComps,
		juce::String propName,
		std::vector<ParameterInfo>& parameterInfos);
};