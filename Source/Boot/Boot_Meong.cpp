#include "Boot/_Win32.hpp"
#include "PPU/PPU.hpp"

struct game_state
{
    i32 Scroll;
    v2i PlayerPos;
};

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

u32 GetTileId(i32 X, i32 Y)
{
    return X % 2 && Y % 2 ? 1 : 0;
}

void DrawTile(u32 TileId, v2f Pos)
{
    switch (TileId)
    {
    case 0: PPU::DrawRect({Pos.X, Pos.Y, 30, 30}, 0xFFFFFFFF);
    case 1: PPU::DrawRect({Pos.X, Pos.Y, 30, 30}, 0xFFFFFF88);
    }
}

bool IsDanger(u32 TileId)
{
    return TileId == 1;
}

void Respawn()
{
    GameState.PlayerPos.X = 2;
    GameState.PlayerPos.Y = 0;
    GameState.Scroll = 0;
}

void Update()
{
    u32 TileId = GetTileId(GameState.PlayerPos.X, GameState.PlayerPos.Y);

    if (IsDanger(TileId))
    {
        Respawn();
    }
}

void Draw()
{
    u32 FieldWidth = 5;
    u32 FieldHeight = 12;

    PPU::DrawRect({ 0, 0, 640, 480 }, 0x000077FF);

    for (u32 i = 0; i < FieldWidth*FieldHeight; i++)
    {
        i32 iX = int(i%FieldWidth);
        i32 iY = int(i/FieldWidth);

        f32 X = (f32)iX*32;
        f32 Y = -(f32)iY*32 + FieldHeight * 32;

        u32 TileId = GetTileId(iX, iY + GameState.Scroll);
        DrawTile(TileId, {X, Y});
    }

    f32 X = (f32)GameState.PlayerPos.X*32;
    f32 Y = -(f32)GameState.PlayerPos.Y*32 + FieldHeight*32 - GameState.Scroll*32;

    PPU::DrawRect({ X, Y, 30, 30 }, 0xFF0000FF);

    PPU::DrawDbgText("You move from square to square,\n"
                     "hoping that the next square\n"
                     "\twon't make you a square", {200, 20}, 0xFFFF00FF);

    PPU::Swap();
}

void CallbackFrame(win32_setup *Setup)
{
    Update();
    Draw();
    (void)Setup;
}

LRESULT CallbackEvent(win32_setup *Setup, HWND Window, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    (void)Setup;

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
            GameState.PlayerPos.Y += 1;

            if (GameState.PlayerPos.Y + GameState.Scroll > 4)
            {
                GameState.Scroll -= 1;
            }
        }
        else if (wParam == VK_DOWN)
        {
            GameState.PlayerPos.Y -= 1;
        }
        break;
    }

    return DefWindowProcA(Window, Msg, wParam, lParam);
}

// Samples served at a rate of 44100 Hz
// Warning: This runs on a different thread, expect race conditions!
audio_sample CallbackGetSample(win32_setup *Setup)
{
    (void)Setup;
    return { 0, 0 };
}

void CallbackTeardown(win32_setup *Setup)
{
    (void)Setup;
    PPU::Shutdown();
}


