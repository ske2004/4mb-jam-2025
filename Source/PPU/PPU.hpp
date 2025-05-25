#ifndef PPU_HPP
#define PPU_HPP

#include "Basics/Std.hpp"

namespace PPU
{

void Init(HWND hWnd);
void Shutdown();

void Swap();

void DrawRect(rect r, rgba c);
void DrawDbgText(const char *Text, v2f Pos, rgba Color);

};

#endif

