#pragma once
#include <vamp-hostsdk/PluginLoader.h>
#include <vamp-hostsdk/Plugin.h>
#include "ChordInfo.h"

class ChordinoFeatureExtractor 
{
public:
    ChordinoFeatureExtractor();
    std::vector<ChordInfo>* extract(
        std::vector<std::vector<float>>& audioArray,
        double sampleRate,
        juce::ValueTree& properties);
    void sendTerminateMessage();

private:
    const std::string pluginKey = "nnls-chroma:chordino";
    const std::string pluginLoaderGetErrStr = "Cannot get VAMP plugin loader instance";
    const std::string pluginLoadErrStr = "Cannot load nnls-chroma chordino plugin with the given file sample rate";
    const std::string pluginInitErrStr = "Cannot initialize nnls-chroma chordino plugin with the given values of FFT order and step size";
    
    _VampHost::Vamp::HostExt::PluginLoader* pluginLoader;
    std::unique_ptr<Vamp::Plugin> plugin;
    double sampleRate;
    std::vector<ChordInfo> chordInfos;
    bool needTerminate;

    void processFeatures(
        _VampHost::Vamp::Plugin::FeatureSet &features); 
}; 