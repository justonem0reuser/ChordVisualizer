# Chord Visualizer

This project is a simple example of working with an existing VAMP plugin inside a JUCE application. It allows to recognize chords and show them on a keyboard while playing an audio file. 

An audio file should be loaded and fully processed before playing. This is not a self-contained application as it used NNLS Chordino VAMP plugin to chord extraction. If it is not installed, the application can open the VAMP plugin pack web page for user to download and install it manually.

Prerequisites for building the application:
1. JUCE framework installed (https://juce.com) - the framework for audio applications and plugins development.
2. VAMP Plugin SDK built (https://www.vamp-plugins.org/develop.html) - contains the VAMP plugin host.

You should also have Chordino VAMP plugin installed as a part of VAMP plugin pack (https://www.vamp-plugins.org/pack.html). It doesn't affect the building process, but you will not be able to use the application without it.

Building the application:
1. Open ChordVisualizer.jucer file using the Projucer (the IDE tool for creating and managing JUCE projects).
2. In "File -> Global paths" window choose your OS and correct "Path to JUCE" and "JUCE modules" according to your JUCE directories.
3. In "Settings -> Header Search Path" correct the path name for your VAMP Plugin SDK header files (there should be \\vamp, \\vamp-hostsdk and \\vamp-sdk subfolders inside your folder).
4. From the Exporters list choose the exporter for your OS, then go to the build configuration and enter the path name for your vamp-sdk.lib and vamp-hostsdk.lib files in "Extra Library Search Path" field.
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