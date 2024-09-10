/*
  ==============================================================================

    EssentiaFeatureExtractor.cpp
    Created: 3 Jul 2024 4:10:27pm
    Author:  -

  ==============================================================================
*/

#include "EssentiaFeatureExtractor.h"
#include <string>
EssentiaFeatureExtractor::EssentiaFeatureExtractor()
{
    essentia::init();
}

EssentiaFeatureExtractor::~EssentiaFeatureExtractor()
{
    stop();
    essentia::shutdown();
}

void EssentiaFeatureExtractor::extract(float** array, unsigned int channelNumber, long long length, double sampleRate,
    vector<string>& chords, vector<double>& timestamps, vector<double>& strength)
{
    prepare(length, sampleRate);
    process(array, channelNumber, length);
    stop();
    chords = this->chords;
    timestamps.resize(ticks.size());
    strength.resize(this->strength.size());
    for (int i = 0; i < strength.size(); i++)
    {
        timestamps[i] = ticks[i];
        strength[i] = this->strength[i];
    }
    timestamps[ticks.size() - 1] = ticks[ticks.size() - 1];
}

void EssentiaFeatureExtractor::prepare(int bufferSize, double sampleRate)
{
    this->sampleRate = (essentia::Real)sampleRate;
    audioBuffer.resize(bufferSize);
    essentia::standard::AlgorithmFactory& factory = 
        essentia::standard::AlgorithmFactory::instance();

    beatTrackerMultiFeature = factory.create(
        "BeatTrackerMultiFeature");
    frameCutter = factory.create(
        "FrameCutter",
        "frameSize", 2048,
        "hopSize", 1024,
        "startFromZero", true);
    //    "silentFrames", "noise");
    windowing = factory.create(
        "Windowing",
        "type", "blackmanharris62");
    spectrum = factory.create(
        "Spectrum");
    spectralPeaks = factory.create(
        "SpectralPeaks",
        "orderBy", "magnitude",
        "magnitudeThreshold", 0.00001,
        "minFrequency", 20,
        "maxFrequency", 3500,
        "maxPeaks", 60);
    hpcp = factory.create(
        "HPCP");
    chordsDetectionBeats = factory.create(
        "ChordsDetectionBeats",
        "hopSize", 1024,
        "sampleRate", sampleRate);

    beatTrackerMultiFeature->input("signal").set(audioBuffer);
    beatTrackerMultiFeature->output("ticks").set(ticks);
    beatTrackerMultiFeature->output("confidence").set(confidence);
    frameCutter->input("signal").set(audioBuffer);
    frameCutter->output("frame").set(cuttedFrames);
    windowing->input("frame").set(cuttedFrames);
    windowing->output("frame").set(windowedFrames);
    spectrum->input("frame").set(windowedFrames);
    spectrum->output("spectrum").set(spectrumVec);
    spectralPeaks->input("spectrum").set(spectrumVec);
    spectralPeaks->output("frequencies").set(frequencies);
    spectralPeaks->output("magnitudes").set(magnitudes);
    hpcp->input("frequencies").set(frequencies);
    hpcp->input("magnitudes").set(magnitudes);
    hpcp->output("hpcp").set(pcp);
    chordsDetectionBeats->input("pcp").set(pcpvec);
    chordsDetectionBeats->input("ticks").set(ticks);
    chordsDetectionBeats->output("chords").set(chords);
    chordsDetectionBeats->output("strength").set(strength);
}

void EssentiaFeatureExtractor::stop()
{
    if (beatTrackerMultiFeature != nullptr)
    {
        delete beatTrackerMultiFeature;
        beatTrackerMultiFeature = nullptr;
    }
    if (frameCutter != nullptr)
    {
        delete frameCutter; 
        frameCutter = nullptr;
    }
    if (windowing != nullptr) 
    {
        delete windowing; 
        windowing = nullptr;
    }
    if (spectrum != nullptr) 
    {
        delete spectrum; 
        spectrum = nullptr;
    }
    if (spectralPeaks != nullptr) 
    { 
        delete spectralPeaks; 
        spectralPeaks = nullptr;
    }
    if (hpcp != nullptr) 
    {
        delete hpcp; 
        hpcp = nullptr;
    }
    if (chordsDetectionBeats != nullptr)
    {
        delete chordsDetectionBeats;
        chordsDetectionBeats = nullptr;
    }
    pcpvec.clear();
}

void EssentiaFeatureExtractor::process(float** buffer, unsigned int channelNumber, int size)
{
    if (size > 0 && channelNumber > 0 && buffer != nullptr)
    {
        // to mono
        for (int i = 0; i < size; i++)
        {
            audioBuffer[i] = 0.f;
            for (int j = 0; j < channelNumber; j++)
                audioBuffer[i] += buffer[j][i];
            audioBuffer[i] /= channelNumber;
        }

        beatTrackerMultiFeature->compute();
        while (true)
        //if (cuttedFrames.size() == size)
        {
            frameCutter->compute();
            if (cuttedFrames.size() == 0)
                break;
            windowing->compute();
            spectrum->compute();
            spectralPeaks->compute();
            hpcp->compute();
            pcpvec.push_back(pcp);
        }
        chordsDetectionBeats->compute();
    }
}
