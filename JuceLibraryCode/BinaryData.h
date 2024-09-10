/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   open_png;
    const int            open_pngSize = 1098;

    extern const char*   pause_png;
    const int            pause_pngSize = 1220;

    extern const char*   play_png;
    const int            play_pngSize = 1437;

    extern const char*   playPause_png;
    const int            playPause_pngSize = 1411;

    extern const char*   settings_png;
    const int            settings_pngSize = 2544;

    extern const char*   stop_png;
    const int            stop_pngSize = 1346;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 6;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
