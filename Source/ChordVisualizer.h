#pragma once

#include <JuceHeader.h>

class ChordVisualizer  : public juce::Component
{
public:
    ChordVisualizer();
    ~ChordVisualizer() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void setChord(std::string name, std::set<int>& notes);
    int getKeyboardX();
    int getKeyboardWidth();

private:
    const static int octaveNumber = 2;

    const static int 
        keysInOctave = 12,
        blackKeysInOctave = 5,
        whiteKeysInOctave = keysInOctave - blackKeysInOctave,
        keyNumber = octaveNumber * keysInOctave,
        blackKeyNumber = octaveNumber * blackKeysInOctave;

    const static int chromaTimer = 2,
                     margins = 10,
                     minTextHeight = 30;

    const float octaveWidth = 300.f,
        keyboardWidth = octaveWidth * octaveNumber + 2.f,
        keyboardHeight = 250.f,
        whiteKeyWidth = octaveWidth / whiteKeysInOctave,
        blackKeyWidth = 27.f,
        blackKeyHeight = 164.f;

    const juce::Colour backgroundColour = juce::Colours::black,
                       keyColour = juce::Colours::green,
                       keyContourColour = juce::Colours::darkgrey,
                       textColour = juce::Colours::lawngreen;
    const std::array<float, blackKeysInOctave> blackKeysRelPos1Octave
    { 
        0.6f, 1.775f, 3.6f, 4.6875f, 5.775f, 
    };
    const std::array<bool, keysInOctave> isWhite1Octave
    { 
        true, false, true, false, true, true, false, true, false, true, false, true,
    };
    const std::map<char, int> chordFirstNotes
    {
        { 'A', 9 },
        { 'B', 11 },
        { 'C', 0 },
        { 'D', 2 },
        { 'E', 4 },
        { 'F', 5 },
        { 'G', 7 },
    };

    std::array<float, blackKeyNumber> blackKeysRelPos;
    std::array<bool, keyNumber> isWhite;
    std::array<int, keyNumber> indicesInBlackWhiteArrays;
    std::array<juce::Path, keyNumber> keyPathes;

    float scale;
    int keyboardX0, keyboardY0, textHeight;
    std::string currentChord;
    std::set<int> chordNotes;
    juce::Rectangle<float> textRect;

    juce::Path getKeyPath(int keyIndex);
    void drawKey(juce::Graphics& g, juce::Path path, juce::Colour colour);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChordVisualizer)
};
