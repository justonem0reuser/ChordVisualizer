#include "StoppedPlayerState.h"

#include "PlayingPlayerState.h"
#include "StartingPlayerState.h"

void StoppedPlayerState::initialize(MainContentComponent* context)
{
    this->context = context;
    context->setButtonImage(context->getPlayButton(), *context->getPlayImage());
    context->getPlayButton()->setEnabled(true);
    context->getStopButton()->setEnabled(false);
    context->getOpenButton()->setEnabled(true);
    context->getSettingsButton()->setEnabled(true);
    context->getSlider()->setVisible(false);
    context->getPositionLabel()->setText("", juce::NotificationType::dontSendNotification);
    context->getTransportSource()->setPosition(0.0);
    context->getVisualizer()->setChord("", std::set<int>());
}

void StoppedPlayerState::changeTransportSource(juce::AudioTransportSource* source)
{
    if (source->isPlaying())
        context->setState(new PlayingPlayerState);
}

void StoppedPlayerState::playButtonClick()
{
    context->setState(new StartingPlayerState);
}

void StoppedPlayerState::stopButtonClick()
{
}
