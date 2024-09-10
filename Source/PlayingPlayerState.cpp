#include "PlayingPlayerState.h"

#include "PausingPlayerState.h"
#include "StoppedPlayerState.h"
#include "StoppingPlayerState.h"

void PlayingPlayerState::initialize(MainContentComponent* context)
{
    this->context = context;
    context->setButtonImage(context->getPlayButton(), *context->getPauseImage());
    context->getStopButton()->setEnabled(true);
    context->getOpenButton()->setEnabled(false);
    context->getSettingsButton()->setEnabled(false);
    context->getSlider()->setVisible(true);
}

void PlayingPlayerState::changeTransportSource(juce::AudioTransportSource* source)
{
    if (!source->isPlaying())
        context->setState(new StoppedPlayerState);
}

void PlayingPlayerState::playButtonClick()
{
    context->setState(new PausingPlayerState);
}

void PlayingPlayerState::stopButtonClick()
{
    context->setState(new StoppingPlayerState);
}
