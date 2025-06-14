#include "MainContentComponent.h"
#include "PlayerStates/NoContentPlayerState.h"
#include "PlayerStates/ContentChangingPlayerState.h"
#include "PlayerStates/ContentChangedPlayerState.h"
#include "ExtractorExceptions.h"

MainContentComponent::MainContentComponent() :
    state(nullptr),
    settingsWindow(nullptr),
    curIndex(0),
    length(0),
    numChannels(0),
    sampleRate(0),
    properties("properties"),
    unchangedProperties(properties.getType()),
    chooser("Open file", juce::File{}, "*.aif;*.aiff;*.flac;*.ogg;*.wav;*.wma"),
    errorMessage(""),
    isDragging(false),
    audioLength(0.0)
{
    tryInitializeExtractor();
    juce::LookAndFeel::setDefaultLookAndFeel(&laf);
    fillDefaultProperties();

    openImage = ImageCache::getFromMemory(BinaryData::open_png, BinaryData::open_pngSize);
    playImage = ImageCache::getFromMemory(BinaryData::play_png, BinaryData::play_pngSize);
    pauseImage = ImageCache::getFromMemory(BinaryData::pause_png, BinaryData::pause_pngSize);
    playPauseImage = ImageCache::getFromMemory(BinaryData::playPause_png, BinaryData::playPause_pngSize);
    stopImage = ImageCache::getFromMemory(BinaryData::stop_png, BinaryData::stop_pngSize);
    settingsImage = ImageCache::getFromMemory(BinaryData::settings_png, BinaryData::settings_pngSize);

    addAndMakeVisible(&openButton);
    setButtonImage(&openButton, openImage);
    openButton.onClick = [this] { openButtonClicked(); };

    addAndMakeVisible(&settingsButton);
    setButtonImage(&settingsButton, settingsImage);
    settingsButton.onClick = [this] { settingsButtonClicked(); };

    addAndMakeVisible(&playButton);
    setButtonImage(&playButton, playImage);
    playButton.setEnabled(false);
    playButton.onClick = [this] { state->playButtonClick(); };

    addAndMakeVisible(&stopButton);
    setButtonImage(&stopButton, stopImage);
    stopButton.setEnabled(false);
    stopButton.onClick = [this] { state->stopButtonClick(); };

    addAndMakeVisible(&positionLabel);
    positionLabel.setText("", juce::dontSendNotification);
    positionLabel.setFont(juce::Font::fromString("Helvetica Neue").withHeight(24.f).withStyle(juce::Font::FontStyleFlags::bold));

    addAndMakeVisible(&slider);
    slider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    slider.setRange(0.0, 1.0, juce::NotificationType::dontSendNotification);
    slider.onDragStart = [this]() { isDragging = true; };
    slider.onDragEnd = [this]() { isDragging = false; };
    slider.onValueChange = [this]() 
        { 
            double pos = slider.getValue() * audioLength;
            transportSource.setPosition(pos);
            visualizePosition(pos);
        };

    visualizer = std::make_unique<ChordVisualizer>();
    addAndMakeVisible(visualizer.get());

    setSize(300, 300);

    formatManager.registerBasicFormats();
    transportSource.addChangeListener(this);
    addChangeListener(this);
    
    setAudioChannels(0, 16); // 0 input, max output channels 
    setState(new NoContentPlayerState);
    startTimer(20);
}

MainContentComponent::~MainContentComponent() 
{
    shutdownAudio();
    if (extractThread)
    {
        extractor->sendTerminateMessage();
        if (extractThread && extractThread->joinable())
            extractThread->join();
    }
    LookAndFeel::setDefaultLookAndFeel(nullptr);
}

void MainContentComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void MainContentComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainContentComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) 
{
    if (readerSource.get() == nullptr)
        bufferToFill.clearActiveBufferRegion();
    else
        transportSource.getNextAudioBlock(bufferToFill);
}

void MainContentComponent::releaseResources()
{
    transportSource.releaseResources();
}

void MainContentComponent::resized() 
{
    int w = getWidth(),
        h = getHeight();
    visualizer->setBounds(0, 110, getWidth(), getHeight() - 110);
    int x = visualizer->getKeyboardX();

    openButton.setBounds(x, 8, imageButtonSize, imageButtonSize);
    x += imageButtonSize + imageButtonDistance;
    settingsButton.setBounds(x, 8, imageButtonSize, imageButtonSize);
    x += imageButtonSize + imageButtonDistance;
    playButton.setBounds(x, 8, imageButtonSize, imageButtonSize);
    x += imageButtonSize + imageButtonDistance;
    stopButton.setBounds(x, 8, imageButtonSize, imageButtonSize);

    positionLabel.setBounds((w - positionLabelWidth) / 2, 50, positionLabelWidth, 30);
    slider.setBounds(visualizer->getKeyboardX(), 80, visualizer->getKeyboardWidth(), 30);
}

void MainContentComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &transportSource)
    {
        state->changeTransportSource(&transportSource);
        audioLength = transportSource.getLengthInSeconds();
    }
    else if (source == this)
    {
        if (errorMessage.empty())
        {
            if (chordInfos->size() == 0)
                catchException(std::exception(noChordsErrStr.c_str()));
            else
                setState(new ContentChangedPlayerState);
        }
        else
            catchException(std::exception(errorMessage.c_str()));
    }
}

void MainContentComponent::timerCallback() 
{
    if (transportSource.isPlaying())
    {
        double pos = transportSource.getCurrentPosition();
        if (!isDragging)
            slider.setValue(pos / audioLength, juce::dontSendNotification);
        visualizePosition(pos);
    }
}

void MainContentComponent::setState(PlayerState* state)
{
    this->state.reset(state);
    this->state->initialize(this);
}

void MainContentComponent::fillDefaultProperties()
{
    const int maxFftSize = 15;
    parameterInfos.clear();
    parameterInfos.push_back(ParameterInfo(
        "fftOrder", "FFT order", 14, 8, maxFftSize, 1, false, true));
    parameterInfos.push_back(ParameterInfo(
        "stepSize", "Step size", 2048, 1, 1 << maxFftSize, 1, false, true));
    parameterInfos.push_back(ParameterInfo(
        "boostn", "Boost likelihood of NO CHORD label", 0.35, 0.0, 1.0, 0.01, false, false));
    parameterInfos.push_back(ParameterInfo(
        "rollon", "Spectral roll-on (%)", 0.0, 0.0, 5.0, 0.5, false, false));
    parameterInfos.push_back(ParameterInfo(
        "whitening", "Spectral whitening", 1.0, 0.0, 1.0, 0.01, false, false));
    parameterInfos.push_back(ParameterInfo(
        "s", "Spectral shape", 0.7, 0.5, 0.9, 0.01, false, false));
    parameterInfos.push_back(ParameterInfo(
        "useNNLS", "Use approximate transcription (NNLS)", true, false, true, 1, true, false));
    parameterInfos.push_back(ParameterInfo(
        "tuningmode", "Use local tuning", false, false, true, 1, true, false));
    parameterInfos.push_back(ParameterInfo(
        "usehartesyntax", "Use Chris Harte's chord syntax", false, false, true, 1, true, false));
    // Other options are not supported in the current version of NNLS-Chroma Chordino.

    for (int i = 0; i < parameterInfos.size(); i++)
        properties.setProperty(
            parameterInfos[i].name, parameterInfos[i].defaultValue, nullptr);
}

void MainContentComponent::catchException(const std::exception& e)
{
    positionLabel.setText("", juce::NotificationType::dontSendNotification);
    juce::NativeMessageBox::showMessageBoxAsync(
        juce::MessageBoxIconType::WarningIcon,
        "Error", e.what(), this, NULL);
    setState(new ContentChangedPlayerState);
}

void MainContentComponent::openButtonClicked()
{
    chooser.launchAsync(chooserFlags, [this](const juce::FileChooser& fc)
        {
            try
            {
                auto file = chooser.getResult();
                if (file != juce::File{})
                {
                    setState(new ContentChangingPlayerState);
                    positionLabel.setText("Chords extracting...", juce::NotificationType::dontSendNotification);
                    if (extractThread && extractThread->joinable())
                        extractThread->join();
                    extractThread.reset(new std::thread(
                        &MainContentComponent::extract, this, true));
                }
            }
            catch (const PluginNotInstalledException& e)
            {
                auto callback = juce::ModalCallbackFunction::create([this](int res)
                    {
                        if (res == 1)
                        {
                            try
                            {
                                std::system(pluginAddressCmdStr.c_str());
                            }
                            catch (const std::exception& e)
                            {
                                catchException(e);
                            }
                        }
                        JUCEApplicationBase::quit();
                    });

                juce::NativeMessageBox::showOkCancelBox(
                    juce::MessageBoxIconType::WarningIcon,
                    "Error", 
                    pluginNotInstalledErrStr,
                    this, callback);
            }
            catch (const std::exception& e)
            {
                catchException(e);
            }
        });
}

void MainContentComponent::settingsButtonClicked()
{
    if (!settingsWindow)
    {
        settingsWindow.reset(new SettingsWindow(properties, parameterInfos));
        settingsWindow->addComponentListener(this);
    }
    unchangedProperties.copyPropertiesFrom(properties, nullptr);
    settingsWindow->toFront(true);
    settingsWindow->setVisible(true);
}

juce::String MainContentComponent::getTimeStr(double sec)
{
    juce::RelativeTime position(sec);
    auto minutes = ((int)position.inMinutes()) % 60;
    auto seconds = ((int)position.inSeconds()) % 60;
    auto tenth = (((int)position.inMilliseconds()) % 1000) / 100;
    return juce::String::formatted("%02d:%02d.%01d", minutes, seconds, tenth);
}

void MainContentComponent::visualizePosition(double pos)
{
    juce::String posStr = getTimeStr(pos) + " / " + getTimeStr(audioLength);
    positionLabel.setText(posStr, juce::dontSendNotification);
    
    int chordInfosMaxIndex = (int)chordInfos->size() - 1;
    if (chordInfosMaxIndex < 0 ||
        pos < (*chordInfos)[0].timestamp) 
    {
        visualizer->setChord("", std::set<int>());
    }
    else 
    {
        if (pos >= (*chordInfos)[curIndex].timestamp)
            while (curIndex < chordInfosMaxIndex &&
                (*chordInfos)[curIndex + 1].timestamp < pos)
                curIndex++;
        else
            while (curIndex > 0 &&
                (*chordInfos)[curIndex].timestamp > pos)
                curIndex--;
        visualizer->setChord((*chordInfos)[curIndex].name, (*chordInfos)[curIndex].notes);
    }
}

void MainContentComponent::tryInitializeExtractor()
{
    try
    {
        extractor = std::make_unique<ChordinoFeatureExtractor>();
    }
    catch (const PluginLoaderErrorException& e)
    {
        auto callback = juce::ModalCallbackFunction::create([this](int res)
            {
                JUCEApplicationBase::quit();
            });

        juce::NativeMessageBox::showMessageBoxAsync(
            juce::MessageBoxIconType::WarningIcon,
            "Error", e.what(), this, callback);
    }
    catch (const PluginNotInstalledException& e)
    {
        auto callback = juce::ModalCallbackFunction::create([this](int res)
            {
                if (res == 1)
                    std::system(pluginAddressCmdStr.c_str());
                JUCEApplicationBase::quit();
            });

        juce::NativeMessageBox::showOkCancelBox(
            juce::MessageBoxIconType::WarningIcon,
            "Error", pluginNotInstalledErrStr, this, callback);
    }
}

void MainContentComponent::setButtonImage(juce::ImageButton* button, 
                                               juce::Image& image)
{
    button->setImages(
        false, true, true,
        image, 1.0f, juce::Colours::transparentWhite,
        image, 0.8f, juce::Colours::transparentWhite,
        image, 0.5f, juce::Colours::transparentWhite);
}

void MainContentComponent::resetCurrentIndex()
{
    curIndex = 0;
}

void MainContentComponent::componentVisibilityChanged(Component& component)
{
    if (&component == settingsWindow.get())
    {
        if (component.isVisible())
            setState(new ContentChangingPlayerState);
        else
        {
            if (audioData.size() > 0 &&
                !unchangedProperties.isEquivalentTo(properties))
            {
                auto callback = juce::ModalCallbackFunction::create([this](int res)
                    {
                        if (res == 1)
                        {
                            positionLabel.setText("Chords extracting...", juce::NotificationType::sendNotificationSync);
                            if (extractThread && extractThread->joinable())
                                extractThread->join();
                            extractThread.reset(new std::thread(
                                &MainContentComponent::extract, this, false));
                        }
                        else
                            setState(new ContentChangedPlayerState);
                    });
                juce::NativeMessageBox::showYesNoBox(
                    juce::MessageBoxIconType::QuestionIcon, "",
                    "Do you want to re-extract chords with the new parameter values?",
                    nullptr, callback);
            }
            else
                setState(new ContentChangedPlayerState);
        }
    }
}

ChordVisualizer* MainContentComponent::getVisualizer()
{
    return visualizer.get();
}

juce::Label* MainContentComponent::getPositionLabel()
{
    return &positionLabel;
}

juce::Slider* MainContentComponent::getSlider()
{
    return &slider;
}

juce::ImageButton* MainContentComponent::getOpenButton()
{
    return &openButton;
}

juce::ImageButton* MainContentComponent::getSettingsButton()
{
    return &settingsButton;
}

juce::ImageButton* MainContentComponent::getPlayButton()
{
    return &playButton;
}

juce::ImageButton* MainContentComponent::getStopButton()
{
    return &stopButton;
}

juce::Image* MainContentComponent::getPlayImage()
{
    return &playImage;
}

juce::Image* MainContentComponent::getPauseImage()
{
    return &pauseImage;
}

juce::Image* MainContentComponent::getPlayPauseImage()
{
    return &playPauseImage;
}

juce::AudioTransportSource* MainContentComponent::getTransportSource()
{
    return &transportSource;
}

void MainContentComponent::readFile(juce::File& file)
{
    try
    {
        errorMessage.clear();
        auto reader = formatManager.createReaderFor(file);
        if (reader == nullptr)
            throw std::exception(loadingFileErrStr.c_str());

        auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true); // reader is now owned and managed by newSource
        length = reader->lengthInSamples;
        numChannels = reader->numChannels;
        sampleRate = reader->sampleRate;
        if (length <= 0 || numChannels == 0 || sampleRate <= 0.0)
            throw std::exception(readingFileErrStr.c_str());

        audioArray.resize(numChannels);
        audioData.resize(numChannels);
        for (int i = 0; i < numChannels; i++)
        {
            audioArray[i].resize(length);
            audioData[i] = audioArray[i].data();
        }

        if (!reader->read(audioData.data(), numChannels, 0, length))
        {
            throw std::exception(readingFileErrStr.c_str());
        }

        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset(newSource.release());
    }
    catch (const std::exception& ex)
    {
        errorMessage = std::string(ex.what());
        sendChangeMessage();
    }
}

void MainContentComponent::extract(bool needReadFile)
{
    try
    {
        errorMessage.clear();
        if (needReadFile)
            readFile(chooser.getResult());
        if (errorMessage.empty())
            chordInfos = extractor->extract(audioArray, sampleRate, properties);
    }
    catch (const std::exception& e)
    {
        errorMessage = std::string(e.what());
    }
    sendChangeMessage();
}
