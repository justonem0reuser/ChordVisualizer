#include "NoContentPlayerState.h"

void NoContentPlayerState::initialize(MainContentComponent* context)
{
    this->context = context;
    context->getStopButton()->setEnabled(false);
    context->getOpenButton()->setEnabled(true);
    context->getPlayButton()->setEnabled(false);
    context->getSettingsButton()->setEnabled(true);
    context->getSlider()->setVisible(false);
}

void NoContentPlayerState::changeTransportSource(juce::AudioTransportSource* source)
{
}

void NoContentPlayerState::playButtonClick()
{
}

void NoContentPlayerState::stopButtonClick()
{
}

