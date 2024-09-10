#include <JuceHeader.h>
#include "ChordVisualizer.h"
#include "MainContentComponent.h"

//==============================================================================
ChordVisualizer::ChordVisualizer():
    currentChord(""),
    keyboardX0(0),
    keyboardY0(0),
    scale(1.f),
    textHeight(minTextHeight)
{
    for (int i = 0; i < blackKeyNumber; i++)
    {
        blackKeysRelPos[i] = blackKeysRelPos1Octave[i % blackKeysInOctave]
            + i / blackKeysInOctave * whiteKeysInOctave;
    }
    
    int curIndInBlackArray = 0,
        curIndInWhiteArray = 0;
    for (int i = 0; i < keyNumber; i++)
    {
        isWhite[i] = isWhite1Octave[i % keysInOctave];
        if (isWhite[i])
            indicesInBlackWhiteArrays[i] = curIndInWhiteArray++;
        else
            indicesInBlackWhiteArrays[i] = curIndInBlackArray++;
    }
}

ChordVisualizer::~ChordVisualizer()
{
}

void ChordVisualizer::paint(juce::Graphics& g)
{
    g.fillAll(backgroundColour);
    g.setOpacity(1.0f);

    for (int i = 0; i < keyNumber; i++)
    {
        bool isActive = chordNotes.find(i) != chordNotes.end();
        juce::Colour colour = isActive ? juce::Colours::green :
            isWhite[i] ? juce::Colours::white : juce::Colours::black;
        drawKey(g, keyPathes[i], colour);
    }

    g.setColour(textColour);
    g.setFont(textHeight - 2);
    g.drawText(currentChord, textRect, juce::Justification::centred);
}

void ChordVisualizer::resized()
{
    int w = getWidth(),
        h = getHeight(),
        mw = w - 2 * margins,
        mh = h - 3 * margins - minTextHeight;

    if (mw <= 0 || mh <= 0)
        scale = keyboardX0 = keyboardY0 = 0;
    else if ((float)mw / mh >= keyboardWidth / keyboardHeight)
    {
        scale = mh / keyboardHeight;
        keyboardX0 = margins + 0.5f * (mw - keyboardWidth * scale);
    }
    else
    {
        scale = mw / keyboardWidth;
        keyboardX0 = margins;
    }
    keyboardY0 = h - margins - keyboardHeight * scale;

    for (int i = 0; i < keyNumber; i++)
        keyPathes[i] = getKeyPath(i);

    textHeight = std::min(keyboardY0 - 2 * margins, mw / 5);
    textRect.setBounds(margins, keyboardY0 - margins - textHeight, mw, textHeight);
}

void ChordVisualizer::setChord(
    std::string name, std::set<int>& notes)
{
        currentChord = name;
        chordNotes = notes;
        repaint();
}

int ChordVisualizer::getKeyboardX()
{
    return keyboardX0;
}

int ChordVisualizer::getKeyboardWidth()
{
    return (int)(keyboardWidth * scale);
}

juce::Path ChordVisualizer::getKeyPath(int keyIndex)
{
    juce::Path path;
    int i = indicesInBlackWhiteArrays[keyIndex];
    if (isWhite[keyIndex]) // white note
    {
        int x0 = keyboardX0 + (int)(i * whiteKeyWidth * scale),
            xn = keyboardX0 + (int)((i + 1) * whiteKeyWidth * scale),
            y0 = keyboardY0 + (int)(keyboardHeight * scale),
            yBlack = keyboardY0 + (int)(blackKeyHeight * scale);
        path.startNewSubPath(x0, y0);
        if (keyIndex == 0 || isWhite[keyIndex - 1])
            path.lineTo(x0, keyboardY0);
        else
        {
            int leftBlackKeyIndex = 
                indicesInBlackWhiteArrays[keyIndex - 1];
            int x1 = keyboardX0 + (int)(scale *
                (blackKeysRelPos[leftBlackKeyIndex] * whiteKeyWidth +
                blackKeyWidth));
            path.lineTo(x0, yBlack);
            path.lineTo(x1, yBlack);
            path.lineTo(x1, keyboardY0);
        }
        if (keyIndex == keyNumber - 1 || isWhite[keyIndex + 1])
            path.lineTo(xn, keyboardY0);
        else
        {
            int rightBlackKeyIndex = 
                indicesInBlackWhiteArrays[keyIndex + 1];
            int x2 = keyboardX0 + (int)(scale * 
                blackKeysRelPos[rightBlackKeyIndex] * whiteKeyWidth);
            path.lineTo(x2, keyboardY0);
            path.lineTo(x2, yBlack);
            path.lineTo(xn, yBlack);
        }
        path.lineTo(xn, y0);
    }
    else // black note
    {
        int x0 = keyboardX0 + (int)(blackKeysRelPos[i] * whiteKeyWidth * scale),
            y0 = keyboardY0,
            x1 = x0 + (int)(blackKeyWidth * scale),
            y1 = keyboardY0 + (int)(blackKeyHeight * scale);
        path.startNewSubPath(x0, y0);
        path.lineTo(x1, y0);
        path.lineTo(x1, y1);
        path.lineTo(x0, y1);
    }
    path.closeSubPath();
    return path;
}

void ChordVisualizer::drawKey(juce::Graphics& g, juce::Path path, juce::Colour colour)
{
    g.setColour(colour);
    g.fillPath(path);
    g.setColour(keyContourColour);
    g.strokePath(path, juce::PathStrokeType(1));
}