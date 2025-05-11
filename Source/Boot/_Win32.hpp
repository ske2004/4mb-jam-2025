#ifndef _WIN32_HPP
#define _WIN32_HPP

#include "Basics/Std.hpp"
#include <windows.h>
#include <dsound.h>

typedef struct {
    IDirectSoundBuffer *SecondaryBuffer;
    HANDLE Thread;

    DWORD BytesPerSample;

    DWORD BufferSize_Samples;
    DWORD Index_Samples;
    DWORD Ahead_Samples;
} win32_audio;

typedef struct {
    HWND Window;
    win32_audio Audio;
    UINT_PTR FrameTimer;
} win32_setup;

typedef struct {
    short Left;
    short Right;
} audio_sample;

typedef struct {
    const char *Title;
    bool DpiAware;
    size_t Width;
    size_t Height;
} win32_setup_info;

win32_setup_info CallbackSetup();
LRESULT CallbackEvent(win32_setup *Setup, HWND Window, UINT Msg, WPARAM wParam, LPARAM lParam);
void CallbackFrame(win32_setup *Setup);
audio_sample CallbackGetSample(win32_setup *Setup);
void CallbackTeardown(win32_setup *Setup);

#endif
