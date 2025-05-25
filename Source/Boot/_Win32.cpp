#include "_Win32.hpp"

#ifdef __cplusplus
extern "C" {
#endif
int _fltused=0; // it should be a single underscore since the double one is the mangled name
#ifdef __cplusplus
}
#endif

static win32_setup GLB_Setup;

static void __AdvanceCursor(win32_audio *Audio)
{
    Audio->Index_Samples++;
    if (Audio->Index_Samples >= Audio->BufferSize_Samples)
    {
        Audio->Index_Samples = 0;
    }
}

static void __FillBuffer(win32_setup *Setup, DWORD SamplesToWrite)
{
    win32_audio *Audio = &Setup->Audio;

    DWORD Cursor_Bytes = Audio->Index_Samples * Audio->BytesPerSample;
    DWORD Write_Bytes = SamplesToWrite * Audio->BytesPerSample;

    void *Region1, *Region2;
    DWORD Bytes1, Bytes2;
    if (SUCCEEDED(Audio->SecondaryBuffer->Lock(Cursor_Bytes, Write_Bytes, &Region1, &Bytes1, &Region2, &Bytes2, 0)))
    {
        int Count1 = Bytes1/Audio->BytesPerSample,
            Count2 = Bytes2/Audio->BytesPerSample;

        for (int i = 0; i < Count1; i++)
        {
            audio_sample Sample = CallbackGetSample(Setup);
            ((audio_sample*)Region1)[i] = Sample;
            __AdvanceCursor(Audio);
        }
        for (int i = 0; i < Count2; i++)
        {
            audio_sample Sample = CallbackGetSample(Setup);
            ((audio_sample*)Region2)[i] = Sample;
            __AdvanceCursor(Audio);
        }
        Audio->SecondaryBuffer->Unlock(Region1, Bytes1, Region2, Bytes2);
    }
}

static DWORD WINAPI __AudioThread(LPVOID Param)
{
    win32_setup *Setup = (win32_setup*)Param;
    win32_audio *Audio = &Setup->Audio;

    while (1)
    {
        DWORD _PlayCursor, _WriteCursor;
        if (SUCCEEDED(Audio->SecondaryBuffer->GetCurrentPosition(&_PlayCursor, &_WriteCursor)))
        {
            DWORD PlayCursor = _PlayCursor/Audio->BytesPerSample;
            DWORD WriteCursor = (Audio->Index_Samples % Audio->BufferSize_Samples);
            DWORD TargetCursor = (PlayCursor + Audio->Ahead_Samples) % Audio->BufferSize_Samples;
            DWORD SamplesToWrite = 0;
            if (TargetCursor < WriteCursor)
            {
                SamplesToWrite = (Audio->BufferSize_Samples - WriteCursor) + TargetCursor;
            }
            else
            {
                SamplesToWrite = TargetCursor - WriteCursor;
            }

            __FillBuffer(Setup, SamplesToWrite);
        }

        Sleep(5);
    }

    return 0;
}

static void __DoFrame()
{
    CallbackFrame(&GLB_Setup);
}

static LRESULT CALLBACK __WndProc(
    HWND   hWnd,
    UINT   Msg,
    WPARAM wParam,
    LPARAM lParam
)
{
    switch (Msg) {
    case WM_TIMER:
        if (wParam == GLB_Setup.FrameTimer)
        {
            __DoFrame();
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
    case WM_DESTROY:
        TerminateThread(GLB_Setup.Audio.Thread, 0);
        CallbackTeardown(&GLB_Setup);
        PostQuitMessage(0);
        break;
    }

    return CallbackEvent(&GLB_Setup, hWnd, Msg, wParam, lParam);
}

static void __FatalError(const char *Message)
{
    MessageBoxA(NULL, Message, "Fatal Error", MB_OK);
    ExitProcess(1);
}

static void __InitializeDirectsound(win32_setup *Setup)
{
    const int HzRate = 44100;
    const int BufferSize = HzRate*2*2;

    IDirectSound *DirectSound;
    if (FAILED(DirectSoundCreate(NULL, &DirectSound, NULL)))
    {
        __FatalError("DirectSoundCreate failed");
    }

    if (FAILED(DirectSound->SetCooperativeLevel(Setup->Window, DSSCL_PRIORITY)))
    {
        __FatalError("SetCooperativeLevel failed");
    }

    DSBUFFERDESC BufferDesc = { 0 };
    BufferDesc.dwSize = sizeof BufferDesc;
    BufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER;

    IDirectSoundBuffer *PrimaryBuffer;
    if (FAILED(DirectSound->CreateSoundBuffer(&BufferDesc, &PrimaryBuffer, NULL)))
    {
        __FatalError("CreateSoundBuffer failed");
    }

    WAVEFORMATEX Format = { 0 };
    Format.wFormatTag = WAVE_FORMAT_PCM;
    Format.nChannels = 2;
    Format.nSamplesPerSec = HzRate;
    Format.wBitsPerSample = 16;
    Format.nBlockAlign = Format.nChannels*Format.wBitsPerSample/8;
    Format.nAvgBytesPerSec = Format.nSamplesPerSec*Format.nBlockAlign;

    if (FAILED(PrimaryBuffer->SetFormat(&Format)))
    {
        __FatalError("SetFormat failed");
    }

    static DSBUFFERDESC PlaybackBufferDesc = { 0 };
    PlaybackBufferDesc.dwSize = sizeof PlaybackBufferDesc;
    PlaybackBufferDesc.dwBufferBytes = BufferSize;
    PlaybackBufferDesc.lpwfxFormat = &Format;
    PlaybackBufferDesc.dwFlags = DSBCAPS_GLOBALFOCUS|DSBCAPS_CTRLPOSITIONNOTIFY|DSBCAPS_GETCURRENTPOSITION2;

    IDirectSoundBuffer *PlaybackBuffer;
    if (FAILED(DirectSound->CreateSoundBuffer(&PlaybackBufferDesc, &PlaybackBuffer, NULL)))
    {
        __FatalError("CreateSoundBuffer failed");
    }

    Setup->Audio.SecondaryBuffer = PlaybackBuffer;
    Setup->Audio.BytesPerSample = 2 * Format.nChannels;
    Setup->Audio.BufferSize_Samples = BufferSize / Setup->Audio.BytesPerSample;
    Setup->Audio.Ahead_Samples = HzRate/10;
    Setup->Audio.Index_Samples = 0;
    __FillBuffer(Setup, Setup->Audio.Ahead_Samples*10);

    Setup->Audio.Thread = CreateThread(NULL, 0, __AudioThread, Setup, 0, NULL);
    Setup->Audio.SecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
}

#ifndef _CONSOLE
int WinMainCRTStartup()
#else
int mainCRTStartup()
#endif
{
    win32_setup_info SetupInfo = CallbackSetup();

    if (SetupInfo.DpiAware)
    {
        SetProcessDPIAware();
    }

    WNDCLASSA WindowClass = { 0 };
    WindowClass.lpszClassName = "SmallWin32";
    WindowClass.lpfnWndProc = __WndProc;

    // NOTE(ske): No error handling, but not really necessary.
    RegisterClassA(&WindowClass);

    // NOTE(ske): No error handling, but not really necessary.
    HWND Window = CreateWindowA(
        WindowClass.lpszClassName,
        SetupInfo.Title,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        SetupInfo.Width, SetupInfo.Height,
        NULL,
        NULL,
        NULL,
        NULL
    );

    GLB_Setup.FrameTimer = 1;
    SetTimer(Window, GLB_Setup.FrameTimer, 16, NULL);

    GLB_Setup.Window = Window;
    __InitializeDirectsound(&GLB_Setup);

    static MSG Msg = { 0 };
    while (GetMessageA(&Msg, NULL, 0, 0))
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    ExitProcess(0);
}