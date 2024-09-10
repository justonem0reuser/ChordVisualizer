#include "SettingsComponent.h"

SettingsComponent::SettingsComponent(Component* parent,
                                     juce::ValueTree& initProperties,
                                     std::vector<ParameterInfo>& parameterInfos) :
    parent(parent),
    initProperties(initProperties),
    editedProperties("editedProperties")
{
    editedProperties.copyPropertiesFrom(initProperties, nullptr);

    auto propComps = createPropertyComponents(initProperties, parameterInfos);
    panel.addProperties(propComps, margin);

    auto sSliderComponent = findPropComponent(propComps, "s", parameterInfos);
    auto useNNLSComponent = findPropComponent(propComps, "useNNLS", parameterInfos);
    auto useNNLSToggleButton = static_cast<juce::ToggleButton*>(useNNLSComponent->getChildComponent(0));
    useNNLSToggleButton->onClick = [this, sSliderComponent, useNNLSToggleButton]
    {
        sSliderComponent->setEnabled(useNNLSToggleButton->getToggleState());
    };

    int currentY = margin;
    addAndMakeVisible(&panel);
    panel.setBounds(margin, currentY, width - 2 * margin, panel.getTotalContentHeight());
    currentY += panel.getBottom() + margin;

    addAndMakeVisible(&okButton);
    okButton.setButtonText("OK");
    okButton.setBounds(margin, currentY, buttonWidth, componentHeight);
    okButton.onClick = [this] { okButtonClicked(); };

    addAndMakeVisible(&cancelButton);
    cancelButton.setButtonText("Cancel");
    cancelButton.setBounds(width - margin - buttonWidth, currentY, buttonWidth, componentHeight);
    cancelButton.onClick = [this] { cancel(); };

    currentY += componentHeight + margin;
    setSize(width, currentY);
}

SettingsComponent::~SettingsComponent()
{
}

void SettingsComponent::okButtonClicked()
{
    initProperties.copyPropertiesFrom(editedProperties, nullptr);
    parent->setVisible(false);
}

juce::Array<juce::PropertyComponent*> SettingsComponent::createPropertyComponents(
    juce::ValueTree& initProperties, std::vector<ParameterInfo>& parameterInfos)
{
    juce::Array<juce::PropertyComponent*> components;
    for (int i = 0; i < parameterInfos.size(); i++)
    {
        auto info = parameterInfos[i];
        auto prop = editedProperties.getPropertyAsValue(info.name, nullptr);
        if (info.isBoolean)
            components.add(new juce::BooleanPropertyComponent(
                prop, info.text, ""));
        else
            components.add(new juce::SliderPropertyComponent(
                prop, info.text, info.minValue, info.maxValue, info.step));
    }
    return components;
}

void SettingsComponent::cancel()
{
    editedProperties.copyPropertiesFrom(initProperties, nullptr);
    this->parent->setVisible(false);
}

juce::PropertyComponent* SettingsComponent::findPropComponent(
    juce::Array<juce::PropertyComponent*>& propComps,
    juce::String propName,
    std::vector<ParameterInfo>& parameterInfos)
{
    juce::String text;
    int i = 0;
    while (i < parameterInfos.size() && text.isEmpty())
    {
        if (propName == parameterInfos[i].name)
            text = parameterInfos[i].text;
        i++;
    }

    juce::PropertyComponent* component = nullptr;
    i = 0;
    while (i < propComps.size() && component == nullptr)
    {
        if (text == propComps[i]->getName())
            component = propComps[i];
        i++;
    }
    return component;
}