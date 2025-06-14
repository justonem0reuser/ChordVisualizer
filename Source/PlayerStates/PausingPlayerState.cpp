#include "PausingPlayerState.h"

#include "PausedPlayerState.h"
#include "PlayingPlayerState.h"
#include "StoppingPlayerState.h"

void PausingPlayerState::initialize(MainContentComponent* context)
{
    this->context = context;
    context->getTransportSource()->stop();
}

void PausingPlayerState::changeTransportSource(juce::AudioTransportSource* source)
{
    if (source->isPlaying())
        context->setState(new PlayingPlayerState);
    else
        context->setState(new PausedPlayerState);
}

void PausingPlayerState::playButtonClick()
{
}

void PausingPlayerState::stopButtonClick()
{
    context->setState(new StoppingPlayerState);
}
