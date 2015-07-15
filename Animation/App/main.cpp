#include "SDL.h"

#include "GUI.h"
#include <stdio.h>
#include "Animation.h"
#include "DebugDraw.h"
#include "App.h"


int main(int argc, char** argv)
{

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_CreateWindowAndRenderer(800, 600, 0, &window, &renderer);

	App app;

	initApp(&app);

	const int numPoints = 10;
	float points[numPoints] = { 0 };
	for (int i = 0; i < numPoints; ++i)
	{
		points[i] = i * 800 / numPoints;
	}
	
	int lastTime = SDL_GetTicks();
	float t = 0;
	bool running = true;

	float fpsCounter = 0;
	int fps = 0;

	while (running)
	{
		int thisTime = SDL_GetTicks();
		double dt = (thisTime - lastTime) / 1000.0;

		if (dt == 0.0)
			continue;

		//dt *= 0.1;

		lastTime = thisTime;

		fpsCounter += dt;
		fps++;
		if (fpsCounter > 1.0f)
		{
			printf("FPS: %d\n", fps);
			fpsCounter -= 1.0f;
			fps = 0;
		}



		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				running = false;
				break;
			case SDL_MOUSEMOTION:
				break;

			case SDL_KEYDOWN:
				switch(event.key.keysym.scancode)
				{
				case SDL_SCANCODE_KP_0:
					setVelocity(&app.player, 0.0f);
					printf("%f\n", app.player.velocity);
					break;
				case SDL_SCANCODE_KP_1:
					setVelocity(&app.player, app.player.velocity + 250.0f);
					printf("%f\n", app.player.velocity);
					break;

				case SDL_SCANCODE_KP_2:
					if ( app.player.crouchSpeed == 0.0f)
						app.player.crouchSpeed = -2.0;

					printf("%f\n", app.player.crouchSpeed);
					break;
				case SDL_SCANCODE_KP_8:
					if (app.player.crouchSpeed == 0.0f)
						app.player.crouchSpeed = 1.5;
					printf("%f\n", app.player.crouchSpeed);
					break;

				}

				break;
			}
		}

		if (SDL_GetKeyboardState(0)[SDL_SCANCODE_KP_PLUS])
		{
			setVelocity(&app.player, app.player.velocity + 200.0f * dt);
			printf("%f\n", app.player.velocity);
		}
		else if (SDL_GetKeyboardState(0)[SDL_SCANCODE_KP_MINUS])
		{
			setVelocity(&app.player, MAX(app.player.velocity - 200.0f * dt, 0.0f));
			printf("%f\n", app.player.velocity);
		}

		updateEntity(&app.player, dt);


		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		View view = { { -400, -300 }, { 1, 1 }, 0 };


		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

		drawEntity(&app.player, &view, renderer);

		SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);

			
		
		for (int i = 0; i < numPoints; ++i)
		{
			points[i] = fmod((points[i] + app.player.velocity * dt), 800);
			SDL_RenderDrawPoint(renderer, 800 - points[i], 450);
		}
		

		//DEBUG_drawDebug(renderer);

		SDL_RenderPresent(renderer);

	}

	SDL_DestroyWindow(window);

	return 0;
}
