#include "ContentChangingPlayerState.h"

#include "StoppedPlayerState.h"

void ContentChangingPlayerState::initialize(MainContentComponent* context)
{
    this->context = context;
    context->getStopButton()->setEnabled(false);
    context->getOpenButton()->setEnabled(false);
    context->getPlayButton()->setEnabled(false);
    context->getSettingsButton()->setEnabled(false);
    context->getSlider()->setVisible(false);
}

void ContentChangingPlayerState::changeTransportSource(juce::AudioTransportSource* source)
{
    context->setState(new StoppedPlayerState);
}

void ContentChangingPlayerState::playButtonClick()
{
}

void ContentChangingPlayerState::stopButtonClick()
{
}

