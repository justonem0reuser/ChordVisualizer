#include "PausedPlayerState.h"

#include "PlayingPlayerState.h"
#include "StartingPlayerState.h"
#include "StoppedPlayerState.h"

void PausedPlayerState::initialize(MainContentComponent* context)
{
    this->context = context;
    context->setButtonImage(context->getPlayButton(), *context->getPlayPauseImage());
}

void PausedPlayerState::changeTransportSource(juce::AudioTransportSource* source)
{
    if (source->isPlaying())
        context->setState(new PlayingPlayerState);
}

void PausedPlayerState::playButtonClick()
{
    context->setState(new StartingPlayerState);
}

void PausedPlayerState::stopButtonClick()
{
    context->setState(new StoppedPlayerState);
}
