
#include "DebugDraw.h"

#include "structArray.h"


struct LineDrawCall
{
	int2 p1;
	int2 p2;
};

StructArray<LineDrawCall> lineDrawCalls;


void DEBUG_drawDebug(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);


	for (int i = 0; i < lineDrawCalls.size(); ++i)
	{
		SDL_RenderDrawLine(renderer, lineDrawCalls[i].p1.x, lineDrawCalls[i].p1.y, lineDrawCalls[i].p2.x, lineDrawCalls[i].p2.y);
	}

}

void DEBUG_drawLine(int2 p1, int2 p2)
{
	lineDrawCalls.push({ p1, p2 });
}

void DEBUG_clear()
{
	lineDrawCalls.clear();
}