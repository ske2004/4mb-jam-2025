#include "Boot/_Win32.hpp"
#include "PPU/PPU.hpp"

struct vec2i
{
    i32 X, Y;
};

typedef struct
{
    vec2i PlayerPos;
} game_state;

game_state GameState = {};

win32_setup_info CallbackSetup()
{
    win32_setup_info Setup = { 0 };
    Setup.Title = "Meong";
    Setup.Width = 640*2;
    Setup.Height = 480*2;
    Setup.DpiAware = true;

    GameState.PlayerPos = { 2, 0 };

    return Setup;
}



void CallbackFrame(win32_setup *Setup)
{
    u32 FieldWidth = 5;
    u32 FieldHeight = 12;
    
    PPU::DrawRect({ 0, 0, 640, 480 }, 0x000077FF);

    for (int i = 0; i < FieldWidth*FieldHeight; i++)
    {
        i32 iX = int(i%FieldWidth);
        i32 iY = int(i/FieldWidth);

        f32 X = iX*32;
        f32 Y = iY*32;

        if (iX % 2 && iY % 2)
        {
            PPU::DrawRect({ X, Y, 30, 30 }, 0xFFFFFF88);
        }
        else 
        {
            PPU::DrawRect({ X, Y, 30, 30 }, 0xFFFFFFFF);
        }
    }

    f32 X = GameState.PlayerPos.X*32;
    f32 Y = GameState.PlayerPos.Y*32;

    PPU::DrawRect({ X, Y, 30, 30 }, 0xFF0000FF);

    PPU::Swap();
}

LRESULT CallbackEvent(win32_setup *Setup, HWND Window, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch (Msg)
    {
    case WM_CREATE:
        PPU::Init(Window);
        break;
    case WM_KEYDOWN:
        if (wParam == VK_LEFT)
        {
            GameState.PlayerPos.X -= 1;
        }
        else if (wParam == VK_RIGHT)
        {
            GameState.PlayerPos.X += 1;
        }
        else if (wParam == VK_UP)
        {
            GameState.PlayerPos.Y -= 1;
        }
        else if (wParam == VK_DOWN)
        {
            GameState.PlayerPos.Y += 1;
        }
        break;
    }

    return DefWindowProcA(Window, Msg, wParam, lParam);
}

// Samples served at a rate of 44100 Hz
// Warning: This runs on a different thread, expect race conditions!
audio_sample CallbackGetSample(win32_setup *Setup)
{
    return { 0, 0 };
}

void CallbackTeardown(win32_setup *Setup)
{
    PPU::Shutdown();
}


