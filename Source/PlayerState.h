#pragma once

#include "MainContentComponent.h"

class MainContentComponent;

class PlayerState
{
public:
    virtual void initialize(MainContentComponent* context) = 0;
    virtual void changeTransportSource(juce::AudioTransportSource* source) = 0;
    virtual void playButtonClick() = 0;
    virtual void stopButtonClick() = 0;

protected:
    MainContentComponent* context;
};
