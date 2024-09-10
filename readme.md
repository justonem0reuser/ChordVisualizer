# Chord Visualizer

This project is a simple example of working with an existing VAMP plugin inside a JUCE application. It allows to recognize chords and show them on a keyboard while playing an audio file. 

This is neither real-time (an audio file should be loaded and fully processed before playing), nor self-contained application (it requires some pre-installed and pre-built components for building and using).

Prerequisites:
1. JUCE framework installed (https://juce.com) - the framework for audio applications and plugins development.
2. VAMP Plugin SDK built (https://www.vamp-plugins.org/develop.html) - contains the VAMP plugin host.
3. Chordino VAMP plugin installed (http://www.isophonics.net/nnls-chroma) - the chord extraction VAMP plugin.

Building the application:
1. Open ChordVisualizer.jucer file using the Projucer (the IDE tool for creating and managing JUCE projects).
2. In "File -> Global paths" window choose your OS and correct "Path to JUCE" and "JUCE modules" according to your JUCE directories.
3. In "Settings -> Header Search Path" enter the path name for your VAMP Plugin SDK header files (there should be \\vamp, \\vamp-hostsdk and \\vamp-sdk subfolders inside your folder).
4. From the Exporters list choose the exporter for your OS, then go to the build configuration that you want to use and enter the path name for your vamp-sdk.lib and vamp-hostsdk.lib files in "Extra Library Search Path" field.
5. Select the exporter that your need, press "Save and open in IDE" button.
6. Build the solution.

The user interface is rather easy and self-explained. Opening and playing files and changing settings is performed using four buttons in the top. Chords are written in standard notation and shown on the keyboard while playing the file.

Some explanation is needed regarding the settings. The default settings is a good starting point. The most of information about parameters is given on the http://www.isophonics.net/nnls-chroma web page. Unfortunately, there is an inconvenience between the code and the description, namely:
1. "HMM (Viterbi decoding)" parameter is not used anymore, it is TRUE by default.
2. "Chroma normalisation" parameter is not used for this plugin.
3. "Use Chris Harte's chord syntax" parameter is added: it determines the text chord notation type.

The "Step size" parameter determines the time precision of the chords time binding: the time error is (step size / sample rate) seconds.
The second parameter of importance is "Boost likelihood of NO CHORD label": if this parameter value is too low, false chords can be determined even from the non-harmonic fragments; on the other hand, too high values can prevent the algorithm from determining even real chords.

Images for button icons were taken from Blend Icons repository (https://blendicons.com).

Comments, bug reports, questions are appreciated.

#JUCE
#VAMP
#Audio
#DSP
#Digital Signal Processing
#Chords
#C++