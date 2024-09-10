#pragma once

#include "PlayerState.h"

class StoppedPlayerState : public PlayerState
{
    virtual void initialize(MainContentComponent* context) override;
    virtual void changeTransportSource(juce::AudioTransportSource* source) override;
    virtual void playButtonClick() override;
    virtual void stopButtonClick() override;
};