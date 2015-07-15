#ifndef EDITOR_H
#define EDITOR_H

#include "structArray.h"
#include "Animation.h"
#include "Renderer.h"
#include "Events.h"

#include "GUI.h"

struct BoneHandle
{
	floatRect rect;
};

struct EditorData
{
	View view;

	GUI* gui;

	LowEventStack lowEventStack;

	StructArray<int> selectedBones;
	int grabbedBone;
	StructArray<BoneHandle> bones;
	
	StructArray<Bone> skeleton;
	StructArray<Constraint> constraints;
	StructArray<Name> names;

	int newBone;

	char skeletonName[64];
	char frameName[64];
};



void initEditor(EditorData* data);
void shutdownEditor(EditorData* data);

void updateBones(EditorData* data);

void updateEditor(SDL_Event event, EditorData* data);
void renderEditor(SDL_Renderer* renderer, EditorData* data);

#endif