#ifndef APP_H
#define APP_H

#include "Animation.h"
#include "Entity.h"

struct App
{
	Entity player;

	Animation standAnimation;
	Animation runAnimation;
	Animation walkAnimation;
	Animation crouchAnimation;
	Animation crouchWalkAnimation;

	char* names[64];
};


void initApp(App* app);

void loadMirroredFrames(const char* fileName, KeyFrame* frame0, KeyFrame* frame1, char** names, int numNames);

void loadWalkAnimation(const char** filenames, Animation* animation, char** names, int numNames);

void loadFrame(const char* fileName, KeyFrame* frame, char** names, int numNames);

#endif