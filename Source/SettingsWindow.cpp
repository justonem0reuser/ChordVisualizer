#include "SettingsWindow.h"

SettingsWindow::SettingsWindow(juce::ValueTree& properties,
                               std::vector<ParameterInfo>& parameterInfos):
    DocumentWindow(
        "Settings",
        juce::Colours::grey,
        juce::DocumentWindow::TitleBarButtons::closeButton,
        true),
    component(this, properties, parameterInfos)
{
    auto w = component.getWidth(),
         h = component.getHeight();
    setContentComponent(&component);
    setUsingNativeTitleBar(true);
    setSize(w, h);
    centreWithSize(w, h);
    setResizable(false, false);
}

void SettingsWindow::closeButtonPressed()
{
    component.cancel();
}
