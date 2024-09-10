#include "CVLookAndFeel.h"

juce::Rectangle<int> CVLookAndFeel::getPropertyComponentContentPosition(PropertyComponent& component)
{
	return juce::Rectangle<int>(component.getWidth() / 2, 0, component.getWidth() / 2, component.getHeight());
}
