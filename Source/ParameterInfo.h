#pragma once
#include <JuceHeader.h>
struct ParameterInfo
{
public:
	ParameterInfo() 
	{}

	ParameterInfo(juce::String name,
				  juce::String text,
				  juce::var defaultValue,
				  juce::var minValue,
				  juce::var maxValue,
				  juce::var step,
				  bool isBoolean,
				  bool isInteger) :
		name(name),
		text(text),
		defaultValue(defaultValue),
		minValue(minValue),
		maxValue(maxValue),
		step(step),
		isBoolean(isBoolean),
		isInteger(isInteger) 
	{}

	juce::String name;
	juce::String text;
	juce::var defaultValue;
	juce::var minValue;
	juce::var maxValue;
	juce::var step;
	bool isBoolean;
	bool isInteger;
};