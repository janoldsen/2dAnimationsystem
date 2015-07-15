#include "Renderer.h"

#include "SDL.h"

float2 pixelToPoint(View* view, int2 pixel)
{
	
	float2 point = rotate({ (float)pixel.x, (float)pixel.y }, view->angle) * float2{ 1 / view->scale.x, 1 / view->scale.y } +view->position;

	return point;
}

int2 pointToPixel(View* view, float2 point)
{
	point = rotate((point - view->position) * view->scale, -view->angle);

	int2 pixel = { (int)point.x, (int)point.y };
	return pixel;
}

void drawLine(SDL_Renderer*  pRenderer, View* pView, float2 p1, float2 p2)
{

	p1 = rotate((p1 - pView->position) * pView->scale, -pView->angle);
	p2 = rotate((p2 - pView->position) * pView->scale, -pView->angle);

	SDL_RenderDrawLine(pRenderer, (int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y);
}

void drawRect(SDL_Renderer* pRenderer, View* pView, intRect rect)
{
	floatRect fRect = { (float)rect.x, (float)rect.y, (float)rect.width, (float)rect.height };
	drawRect(pRenderer, pView, fRect);
}

void fillRect(SDL_Renderer* pRenderer, View* pView, intRect rect)
{
	floatRect fRect = { (float)rect.x, (float)rect.y, (float)rect.width, (float)rect.height };
	fillRect(pRenderer, pView, fRect);
}

void drawRect(SDL_Renderer* pRenderer, View* pView, floatRect rect)
{
	// todo rotate rects

	rect.pos = rotate((rect.pos - pView->position) * pView->scale, -pView->angle);
	rect.extend = rect.extend * pView->scale;

	SDL_Rect intRect = { (int)rect.x, (int)rect.y, (int)rect.width, (int)rect.height };
	SDL_RenderDrawRect(pRenderer, &intRect);
}

void fillRect(SDL_Renderer* pRenderer, View* pView, floatRect rect)
{
	// todo rotate rects

	rect.pos = rotate((rect.pos - pView->position) * pView->scale, -pView->angle);
	rect.extend = rect.extend * pView->scale;

	SDL_Rect intRect = { (int)rect.x, (int)rect.y, (int)rect.width, (int)rect.height };
	SDL_RenderFillRect(pRenderer, &intRect);
}