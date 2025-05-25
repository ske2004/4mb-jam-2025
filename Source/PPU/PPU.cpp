#include "Basics/Std.hpp"
#include "PPU.hpp"
#include "_DirectX.cpp"
#include "Font.cpp"

void PPU::Init(HWND hWnd)
{
    DX_Init(hWnd);
}

void PPU::Shutdown()
{
    DX_Shutdown();
}


void PPU::Swap()
{
    DX_EndFrame();
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

// TODO: this is sloth
void PPU::DrawDbgText(const char *Text, v2f Pos, rgba Color)
{
    f32 XInit = Pos.X;

    for (usz I = 0; Text[I]; I++)
    {
        if (Text[I] == '\n') 
        {
            Pos.X = XInit;
            Pos.Y += 8;
            continue;
        }
        if (Text[I] == '\t') 
        {
            Pos.X += 8 * 4;
            continue;
        }

        usz Glyph = Text[I];

        for (usz Y = 0; Y < 8; Y++)
        {
            u8 Bitmap = sys_font_cyrillic[Glyph*8+(7-Y)];
            for (usz X = 0; X < 8; X++)
            {
                if (Bitmap&(1<<X))
                {
                    DrawRect({Pos.X+X, Pos.Y+Y, 1, 1}, Color);
                }
            }
        }

        Pos.X += 8;
    }
}
