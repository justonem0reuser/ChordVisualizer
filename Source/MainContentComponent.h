#pragma once

#include <JuceHeader.h>
#include "ChordVisualizer.h"
#include "ChordinoFeatureExtractor.h"
#include "PlayerStates/PlayerState.h"
#include "SettingsWindow.h"
#include "ParameterInfo.h"
#include "CVLookAndFeel.h"

class PlayerState;

class MainContentComponent   : public juce::AudioAppComponent,
                               public juce::ChangeBroadcaster,
                               public juce::ChangeListener,
                               public juce::ComponentListener,
                               public juce::Timer
{
public:
    MainContentComponent();
    ~MainContentComponent() override;

    void paint(juce::Graphics&) override;
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    void resized() override;
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    void timerCallback() override;
    void componentVisibilityChanged(Component& component) override;
    void setState(PlayerState* state);
    void setButtonImage(juce::ImageButton* button, 
                        juce::Image& image);
    void resetCurrentIndex();
    ChordVisualizer* getVisualizer();
    juce::Label* getPositionLabel();
    juce::Slider* getSlider();
    juce::ImageButton* getOpenButton();
    juce::ImageButton* getSettingsButton();
    juce::ImageButton* getPlayButton();
    juce::ImageButton* getStopButton();
    juce::Image* getPlayImage();
    juce::Image* getPauseImage();
    juce::Image* getPlayPauseImage();
    juce::AudioTransportSource* getTransportSource();

private:

#if JUCE_WINDOWS
    const std::string openPageCmdStr = "start";
#elif JUCE_MAC
    const std::string openPageCmdStr = "xdg-open";
#else
    const std::string openPageCmdStr = "open";
#endif
    const std::string pluginAddressStr = "https://www.vamp-plugins.org/pack.html";
    const std::string pluginAddressCmdStr = openPageCmdStr + " " + pluginAddressStr;
    const std::string loadingFileErrStr = "Unknown audio file format";
    const std::string pluginNotInstalledErrStr = "NNLS::chordino plugin is not installed. Do you want to open VAMP plugin pack web page to install it manually?";
    const std::string readingFileErrStr = "Cannot read audio file";
    const std::string noChordsErrStr = "No chords are extracted";

    const int imageButtonSize = 32;
    const int imageButtonDistance = 10;
    const int positionLabelWidth = 200;
    const int chooserFlags = juce::FileBrowserComponent::openMode |
                             juce::FileBrowserComponent::canSelectFiles;

    std::unique_ptr <ChordinoFeatureExtractor> extractor;
    std::unique_ptr <ChordVisualizer> visualizer;

    juce::ImageButton openButton, playButton, stopButton, settingsButton;
    juce::Image openImage, playImage, pauseImage, playPauseImage, stopImage, settingsImage;
    juce::Label positionLabel;
    juce::Slider slider;

    juce::FileChooser chooser;

    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    std::vector<ParameterInfo> parameterInfos;
    juce::ValueTree properties; 
    juce::ValueTree unchangedProperties;
    std::unique_ptr<SettingsWindow> settingsWindow;
    std::unique_ptr<PlayerState> state;
    std::vector<ChordInfo>* chordInfos;
    CVLookAndFeel laf;
    std::vector<float*> audioData;
    std::vector<std::vector<float>> audioArray;
    juce::int64 length;
    unsigned int numChannels;
    double sampleRate;
    int curIndex;
    bool isDragging;
    std::unique_ptr<std::thread> extractThread;
    std::string errorMessage;
    double audioLength;

    void tryInitializeExtractor();
    void fillDefaultProperties();
    void catchException(const std::exception& e);
    void openButtonClicked();
    void settingsButtonClicked();
    void readFile(juce::File &file);
    void extract(bool needReadFile);
    static juce::String getTimeStr(double sec);
    void visualizePosition(double pos);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};
