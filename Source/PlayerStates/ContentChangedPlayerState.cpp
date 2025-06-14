#include "ContentChangedPlayerState.h"

#include "NoContentPlayerState.h"
#include "StoppedPlayerState.h"

void ContentChangedPlayerState::initialize(MainContentComponent* context)
{
	if (context->getTransportSource()->getTotalLength() > 0)
		context->setState(new StoppedPlayerState);
	else
		context->setState(new NoContentPlayerState);
}

void ContentChangedPlayerState::changeTransportSource(juce::AudioTransportSource* source)
{
}

void ContentChangedPlayerState::playButtonClick()
{
}

void ContentChangedPlayerState::stopButtonClick()
{
}

