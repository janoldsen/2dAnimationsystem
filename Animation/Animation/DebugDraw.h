#ifndef DEBUG_DRAW_H
#define DEBUG_DRAW_H

#include "SDL.h"
#include "MyMath.h"

void DEBUG_drawDebug(SDL_Renderer* renderer);

void DEBUG_drawLine(int2 p1, int2 p2);

void DEBUG_clear();


#endif
