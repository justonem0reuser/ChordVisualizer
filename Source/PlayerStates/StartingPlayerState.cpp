#include "StartingPlayerState.h"

#include "PlayingPlayerState.h"
#include "StoppingPlayerState.h"

void StartingPlayerState::initialize(MainContentComponent* context)
{
    this->context = context;
    context->getOpenButton()->setEnabled(false);
    context->getSettingsButton()->setEnabled(false);
    context->resetCurrentIndex();
    context->getTransportSource()->start();
}

void StartingPlayerState::changeTransportSource(juce::AudioTransportSource* source)
{
    if (source->isPlaying())
        context->setState(new PlayingPlayerState);
}

void StartingPlayerState::playButtonClick()
{
}

void StartingPlayerState::stopButtonClick()
{
    context->setState(new StoppingPlayerState);
}
