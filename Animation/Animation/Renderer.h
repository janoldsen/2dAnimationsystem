#ifndef RENDERER_H
#define RENDERER_H

#include "MyMath.h"
#include "structArray.h"
#include "MyMath.h"
#include "SDL.h"




struct View
{
	float2 position;
	float2 scale;
	float angle;
};


float2 pixelToPoint(View* view, int2 pixel);
int2 pointToPixel(View* view, float2 point);

void drawLine(SDL_Renderer*  pRenderer, View* pView, float2 p1, float2 p2);
void drawRect(SDL_Renderer* pRenderer, View* pView, intRect rect);
void fillRect(SDL_Renderer* pRenderer, View* pView, intRect rect);
void drawRect(SDL_Renderer*  pRenderer, View* pView, floatRect rect);
void fillRect(SDL_Renderer*  pRenderer, View* pView, floatRect rect);


#endif