/*
  ==============================================================================

    EssentiaFeatureExtractor.h
    Created: 3 Jul 2024 4:10:27pm
    Author:  -

  ==============================================================================
*/

#pragma once

#include "IFeatureExtractor.h"

#include "essentia\algorithmfactory.h"
#include "essentia\essentiamath.h"
#include "essentia\pool.h"


//using namespace essentia;
////using namespace streaming;
//using namespace standard;
using namespace std;
class EssentiaFeatureExtractor: public IFeatureExtractor
{
public:
    EssentiaFeatureExtractor();
    ~EssentiaFeatureExtractor();
    void extract(float** array, unsigned int channelNumber, long long length, double sampleRate,
        vector<string>& chords, vector<double>& timestamps, vector<double>& strength) override;
    void prepare(int bufferSize, double sampleRate) override;
    void stop() override;
    void process(float** buffer, unsigned int channelNumber, int size) override;
private:
    essentia::Real sampleRate;
    essentia::Real confidence;
    vector<vector<essentia::Real>> pcpvec;
    vector<essentia::Real> 
        audioBuffer,
        cuttedFrames,
        windowedFrames,
        spectrumVec,
        frequencies,
        magnitudes,
        pcp,
        ticks,
        strength;
    vector<string> chords;
    essentia::standard::Algorithm 
        *frameCutter = nullptr,
        *windowing = nullptr,
        *spectrum = nullptr,
        *spectralPeaks = nullptr,
        *hpcp = nullptr,
        *beatTrackerMultiFeature = nullptr,
        *chordsDetectionBeats = nullptr;
    //essentia::Pool pool;
};

