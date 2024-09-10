#pragma once
#include <exception>

class PluginLoaderErrorException : public std::exception
{
public:
    const char* what() const override
    {
        return whatStr.getCharPointer();
    }
private:
    const juce::String whatStr = "Cannot get VAMP plugin loader";
};

class PluginNotInstalledException : public std::exception
{
public:
    const char* what() const override
    {
        return whatStr.getCharPointer();
    }
private:
    const juce::String whatStr = "NNLS::chordino plugin is not installed";
};
