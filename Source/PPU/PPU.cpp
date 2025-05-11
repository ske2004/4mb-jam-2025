#include "Basics/Std.hpp"
#include "PPU.hpp"
#include "_DirectX.cpp"

void PPU::Init(HWND hWnd)
{
    DX_Init(hWnd);
}

void PPU::DrawRect(rect r, rgba c)
{
    dx_vertex v[6] = { 0 };
    v[0].x = r.X;
    v[0].y = r.Y;

    v[1].x = r.X + r.W;
    v[1].y = r.Y;
    
    v[2].x = r.X;
    v[2].y = r.Y + r.H;
    
    v[3] = v[2];
    v[4] = v[1];

    v[5].x = r.X + r.W;
    v[5].y = r.Y + r.H;

    for (int i = 0; i < 4; i++)
    {
        v[i].rhw = 1.0f;
        v[i].color = DX_RGBAToInternal(c);
    }

    DX_Append(v, 6);
}

void PPU::Swap()
{
    DX_EndFrame();
}

void PPU::Shutdown()
{
    DX_Shutdown();
}


