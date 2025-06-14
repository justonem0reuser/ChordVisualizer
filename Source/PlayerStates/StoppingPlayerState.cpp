#include "StoppingPlayerState.h"

#include "PlayingPlayerState.h"
#include "StoppedPlayerState.h"

void StoppingPlayerState::initialize(MainContentComponent* context)
{
    this->context = context;
    context->getSlider()->setVisible(false);
    context->getTransportSource()->stop();
}

void StoppingPlayerState::changeTransportSource(juce::AudioTransportSource* source)
{
    if (source->isPlaying())
        context->setState(new PlayingPlayerState);
    else
        context->setState(new StoppedPlayerState);
}

void StoppingPlayerState::playButtonClick()
{
}

void StoppingPlayerState::stopButtonClick()
{
}
