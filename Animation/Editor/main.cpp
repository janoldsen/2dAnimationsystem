#include "SDL.h"

#include "GUI.h"
#include <stdio.h>


#include "Editor.h"
#include "DebugDraw.h"

int main(int argc, char** argv)
{

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_CreateWindowAndRenderer(800, 600, 0, &window, &renderer);


	EditorData editor;
	initEditor(&editor);


	float2 p = { 0, 100 };
	float2 r0 = { 1, 0 };
	float2 r1 = normalize({ 0, -1 });

	
	
	int lastTime = SDL_GetTicks();
	float t = 0;
	bool running = true;	

	float lastAngle = 0.0f;
	while (running)
	{

		int thisTime = SDL_GetTicks();
		float dt = (thisTime - lastTime) / 1000.0f;
		lastTime = thisTime;



		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				running = false;
				break;
			}
			updateEditor(event, &editor);

			char title[128];
			sprintf(title, "Skeleton: %s | Frame: %s", editor.skeletonName, editor.frameName);
			
			SDL_SetWindowTitle(window, title);
			
		}

		
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		renderEditor(renderer, &editor);

		DEBUG_drawDebug(renderer);

		SDL_RenderPresent(renderer);

	}

	SDL_DestroyWindow(window);

	return 0;
}