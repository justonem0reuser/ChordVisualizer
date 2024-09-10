#include <iostream>
#include <JuceHeader.h>
#include "ChordinoFeatureExtractor.h"
#include "ExtractorExceptions.h"

ChordinoFeatureExtractor::ChordinoFeatureExtractor():
    sampleRate(0),
    needTerminate(false)
{
    pluginLoader = Vamp::HostExt::PluginLoader::getInstance();
    if (pluginLoader == nullptr)
        throw PluginLoaderErrorException();
    plugin.reset(pluginLoader->loadPlugin(pluginKey, 44100.0f));
    if (plugin == nullptr)
        throw PluginNotInstalledException();
}

std::vector<ChordInfo>* ChordinoFeatureExtractor::extract(
    std::vector<std::vector<float>>& audioArray,
    double sampleRate,
    juce::ValueTree& properties)
{
    chordInfos.clear();
    std::vector<ChordInfo> result;
        plugin.reset(pluginLoader->loadPlugin(pluginKey, (float)sampleRate));
        if (plugin == nullptr)
            throw std::exception(pluginLoadErrStr.c_str());

        int fftOrder = properties.getProperty("fftOrder"),
            blockSize = 1 << fftOrder,
            stepSize = std::min(blockSize,
                static_cast<int>(properties.getProperty("stepSize")));

        plugin->setParameter("boostn", properties.getProperty("boostn")); // increase likelihood of no chord
        plugin->setParameter("rollon", properties.getProperty("rollon"));
        plugin->setParameter("whitening", properties.getProperty("whitening"));
        plugin->setParameter("s", properties.getProperty("s"));
        plugin->setParameter("useNNLS", properties.getProperty("useNNLS"));
        plugin->setParameter("tuningmode", properties.getProperty("tuningmode"));
        plugin->setParameter("usehartesyntax", properties.getProperty("usehartesyntax"));

        if (!plugin->initialise(1, stepSize, blockSize))
            throw std::exception(pluginInitErrStr.c_str());

        auto channelNumber = audioArray.size(); // > 0 here
        auto length = audioArray[0].size();
        std::vector<float> monoChannel(length, 0.0f);
        for (int i = 0; i < length; i++)
        {
            float val = 0.0f;
            for (int j = 0; j < channelNumber; j++)
                val += audioArray[j][i];
            val /= channelNumber;
            monoChannel[i] = val;
        }
        std::vector<float> fftBuffer(2 * blockSize);

        juce::dsp::FFT forwardFFT(fftOrder);
        juce::dsp::WindowingFunction<float> window(
            blockSize,
            juce::dsp::WindowingFunction<float>::WindowingMethod::hann);
        int frameNumber = 0,
            startSampleNumber = -blockSize / 2,
            midSampleNumber = 0,
            endSampleNumber = startSampleNumber + blockSize;
        while (endSampleNumber < length && !needTerminate)
        {
            if (startSampleNumber < 0)
            {
                juce::zeromem(fftBuffer.data(), -startSampleNumber * sizeof(float));
                std::memcpy(
                    fftBuffer.data() - startSampleNumber,
                    monoChannel.data(),
                    (blockSize + startSampleNumber) * sizeof(float));
            }
            else
                std::memcpy(
                    fftBuffer.data(),
                    monoChannel.data() + startSampleNumber,
                    blockSize * sizeof(float));
            juce::zeromem(fftBuffer.data() + blockSize, blockSize * sizeof(float));
            window.multiplyWithWindowingTable(fftBuffer.data(), blockSize);
            forwardFFT.performRealOnlyForwardTransform(fftBuffer.data(), true);
            auto rt = _VampHost::Vamp::RealTime::frame2RealTime(midSampleNumber, sampleRate);
            std::vector<float*> fftBuffers{ fftBuffer.data() };
            plugin->process(fftBuffers.data(), rt);
            frameNumber++;
            startSampleNumber += stepSize;
            midSampleNumber += stepSize;
            endSampleNumber += stepSize;
        }
        if (!needTerminate)
            processFeatures(plugin->getRemainingFeatures());
        return &chordInfos;
}

void ChordinoFeatureExtractor::sendTerminateMessage()
{
    needTerminate = true;
}

void ChordinoFeatureExtractor::processFeatures(
    _VampHost::Vamp::Plugin::FeatureSet& features)
{
    if (features.size() == 4)
    {
        int numbersIndex = 0;
        std::vector<_VampHost::Vamp::Plugin::Feature>
            noteNames = features[0],
            noteNumbers = features[1];
        int numIndex = 0;
        int i = 0;
        while (i < noteNames.size() && !needTerminate)
        {
            ChordInfo item;
            std::vector<int> notes;
            int minNote = 128, // excluding bass note
                maxNote = -1;
            item.timestamp = noteNames[i].timestamp.sec +
                noteNames[i].timestamp.nsec / 1e9;
            item.name = noteNames[i].label == "N" ? 
                "" : noteNames[i].label;
            while (numIndex < noteNumbers.size() &&
                noteNumbers[numIndex].timestamp == noteNames[i].timestamp)
            {
                int note = (int)noteNumbers[numIndex].values[0];
                if (i > 0 && minNote > note)
                    minNote = note;
                if (maxNote < note)
                    maxNote = note;
                notes.push_back(note);
                numIndex++;
            }
            if (notes.size() > 0)
            {
                // mapping to 24-key keyboard
                int bassNote = notes[0]; // bass note
                int bassOctaveCNote = (bassNote / 12) * 12;
                bassNote -= bassOctaveCNote;
                item.notes.insert(bassNote);
                for (int j = 1; j < notes.size(); j++)
                {
                    int note = notes[j];
                    while (note - 12 > bassNote)
                        note -= 12;
                    item.notes.insert(note);
                }
            }
            chordInfos.push_back(item);
            i++;
        }
    }
}

