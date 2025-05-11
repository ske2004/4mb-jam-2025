#ifndef PPU_HPP
#define PPU_HPP

#include "Basics/Std.hpp"

namespace PPU
{

void Init(HWND hWnd);
void DrawRect(rect r, rgba c);
void Swap();
void Shutdown();

};

#endif

