#include "App.h"




void initApp(App* app)
{

	// load skeleton
	const SkeletonSave* skeleSave = loadSkeleton("basic.skl");
	Skeleton skeleton;
	skeleton.numBones = skeleSave->numBones;
	skeleton.jointPositions = new float2[skeleSave->numBones];
	skeleton.jointAngles = new float2[skeleSave->numBones];
	skeleton.parentJoints = new int[skeleSave->numBones];
	skeleton.names = new int[skeleSave->numBones];

	memcpy(skeleton.jointPositions, skeleSave->jointPos, sizeof(float2) * skeleton.numBones);
	memcpy(skeleton.parentJoints, skeleSave->parentJoints, sizeof(int) * skeleton.numBones);

	char* name = skeleSave->names;
	for (int i = 0; i < skeleSave->numBones; ++i)
	{
		skeleton.jointAngles[i] = float2{ 1, 0 };
		skeleton.names[i] = i;

		app->names[i] = new char[skeleSave->nameLen[i] + 1];
		app->names[i][skeleSave->nameLen[i]] = 0;
		memcpy(app->names[i], name, skeleSave->nameLen[i]);

		name += skeleSave->nameLen[i];
	}

	// load animations
	app->runAnimation.numFrames = 4;
	app->runAnimation.frames = new KeyFrame[4];
	app->runAnimation.functions = new Function*[4];
	app->runAnimation.frameLengths = new float[4];
	{
		const char* fileNames[] = { "run1.key", "run2.key" };
		loadWalkAnimation(fileNames, &app->runAnimation, app->names, skeleton.numBones);
	}

	
	app->walkAnimation.numFrames = 4;
	app->walkAnimation.frames = new KeyFrame[4];
	app->walkAnimation.functions = new Function*[4];
	app->walkAnimation.frameLengths = new float[4];
	{
		const char* fileNames[] = { "key1.key", "key2.key" };
		loadWalkAnimation(fileNames, &app->walkAnimation, app->names, skeleton.numBones);
	}

	app->crouchWalkAnimation.numFrames = 4;
	app->crouchWalkAnimation.frames = new KeyFrame[4];
	app->crouchWalkAnimation.functions = new Function*[4];
	app->crouchWalkAnimation.frameLengths = new float[4];
	{
		const char* fileNames[] = { "cwalk1.key", "cwalk2.key" };
		loadWalkAnimation(fileNames, &app->crouchWalkAnimation, app->names, skeleton.numBones);
	}

	app->standAnimation.numFrames = 1;
	app->standAnimation.frames = new KeyFrame[1];
	app->standAnimation.functions = new Function*[1];
	app->standAnimation.frameLengths = new float[1];
	app->standAnimation.frameLengths[0] = 1.0f;
	app->standAnimation.functions[0] = [](float t){return t; };
	loadFrame("stand.key", app->standAnimation.frames, app->names, skeleton.numBones);

	app->crouchAnimation.numFrames = 1;
	app->crouchAnimation.frames = new KeyFrame[1];
	app->crouchAnimation.functions = new Function*[1];
	app->crouchAnimation.frameLengths = new float[1];
	app->crouchAnimation.frameLengths[0] = 1.0f;
	app->crouchAnimation.functions[0] = [](float t){return t; };
	loadFrame("crouch.key", app->crouchAnimation.frames, app->names, skeleton.numBones);

	

	// init player
	app->player.velocity = 0.0f;
	app->player.skeleton = skeleton;
	app->player.morphing = false;
	{
		app->player.morphFrame.jointAngles = new float2[skeleton.numBones];
		app->player.morphFrame.names = new int[skeleton.numBones];
		app->player.morphFrame.numBones = skeleton.numBones;

		memcpy(app->player.morphFrame.names, skeleton.names, sizeof(int) * skeleton.numBones);
	}
	app->player.height = 1.0f;
	app->player.crouchSpeed = 0.0f;
	{
		app->player.standFrame.jointAngles = new float2[skeleton.numBones];
		app->player.standFrame.names = new int[skeleton.numBones];
		app->player.standFrame.numBones = skeleton.numBones;
	}
	{
		app->player.crouchFrame.jointAngles = new float2[skeleton.numBones];
		app->player.crouchFrame.names = new int[skeleton.numBones];
		app->player.crouchFrame.numBones = skeleton.numBones;
	}
	{
		app->player.standAnim.animation = &app->standAnimation;
		app->player.standAnim.currFrame = 0;
		app->player.standAnim.length = 0.3f;
		app->player.standAnim.t = 0.0f;
	}
	{
		app->player.crouchAnim.animation = &app->crouchAnimation;
		app->player.crouchAnim.currFrame = 0;
		app->player.crouchAnim.length = 0.3f;
		app->player.crouchAnim.t = 0.0f;
	}
	{
		app->player.walkAnim.animation = &app->walkAnimation;
		app->player.walkAnim.currFrame = 0;
		app->player.walkAnim.length = 0.0f;
		app->player.walkAnim.t = 0.0f;
	}
	{
		app->player.runAnim.animation = &app->runAnimation;
		app->player.runAnim.currFrame = 0;
		app->player.runAnim.length = 0.0f;
		app->player.runAnim.t = 0.0f;
	}
	{
		app->player.crouchWalkAnim.animation = &app->crouchWalkAnimation;
		app->player.crouchWalkAnim.currFrame = 0;
		app->player.crouchWalkAnim.length = 0.0f;
		app->player.crouchWalkAnim.t = 0.0f;
	}
	{

		float2 hip, foot, knee;

		for (int i = 0; i < skeleton.numBones; ++i)
		{
			if (strcmp(app->names[skeleton.names[i]], "hip") == 0)
			{
				hip = getNeutralBoneWorldPos(&skeleton, i);
			}
			else if (strcmp(app->names[skeleton.names[i]], "foot_r") == 0)
			{
				foot = getNeutralBoneWorldPos(&skeleton, i);
			}
			else if (strcmp(app->names[skeleton.names[i]], "knee_r") == 0)
			{
				knee = getNeutralBoneWorldPos(&skeleton, i);
			}

			app->player.walkRadius = length(foot - knee);
			app->player.runRadius = length(foot - hip);
		}
	}
	app->player.origin = float2{ 0.0f, 0.0f };
	app->player.state = Entity::STAND;
	app->player.walkRunBlend = 0.0f;


	// animation origins
	{
		for (int i = 0; i < skeleton.numBones; ++i)
		{
			if (strcmp(app->names[i], "foot_r") == 0)
			{
				app->player.standOrigin = getBoneWorldTransform(&skeleton, &app->standAnimation.frames[0], i).position;
				app->player.walkOrigin = getBoneWorldTransform(&skeleton, &app->walkAnimation.frames[0], i).position;
				app->player.runOrigin = getBoneWorldTransform(&skeleton, &app->runAnimation.frames[2], i).position;
				app->player.crouchOrigin = getBoneWorldTransform(&skeleton, &app->crouchAnimation.frames[0], i).position;
				app->player.crouchWalkOrigin = getBoneWorldTransform(&skeleton, &app->crouchWalkAnimation.frames[2], i).position;

			}
		}
	}

}


void loadMirroredFrames(const char* fileName, KeyFrame* frame0, KeyFrame* frame1, char** names, int numNames)
{

	const KeyFrameSave* frameSave = loadKeyFrame(fileName);

	frame0->numBones = frameSave->numBones;
	frame1->numBones = frameSave->numBones;

	frame0->jointAngles = new float2[frameSave->numBones];
	frame1->jointAngles = new float2[frameSave->numBones];

	memcpy(frame0->jointAngles, frameSave->jointAngles, sizeof(float2) * frameSave->numBones);
	memcpy(frame1->jointAngles, frameSave->jointAngles, sizeof(float2) * frameSave->numBones);

	frame0->names = new int[frameSave->numBones];
	frame1->names = new int[frameSave->numBones];

	char* _name = frameSave->names;
	for (int i = 0; i < frameSave->numBones; ++i)
	{
		int len = frameSave->nameLen[i];
		char* name = new char[len + 1];
		char* mirroredName = new char[len + 1];

		memcpy(name, _name, frameSave->nameLen[i]);
		memcpy(mirroredName, _name, frameSave->nameLen[i]);

		name[len] = 0;
		mirroredName[len] = 0;

		char* c;
		if ((c = strstr(name, "_r")) != 0)
			*(c + 1) = 'l';
		else if ((c = strstr(name, "_l")) != 0)
			*(c + 1) = 'r';

		for (int j = 0; j < numNames; ++j)
		{
			if (strcmp(name, names[j]) == 0)
				frame0->names[i] = j;

			if (strcmp(mirroredName, names[j]) == 0)
				frame1->names[i] = j;
		}

		delete[] mirroredName;
		delete[] name;
		_name += frameSave->nameLen[i];
	}


}




void loadWalkAnimation(const char** filenames, Animation* animation, char** names, int numNames)
{

	for (int i = 0; i < 2; ++i)
	{
		loadMirroredFrames(filenames[i], &animation->frames[i], &animation->frames[i + 2], names, numNames);

		animation->frameLengths[i] = 1 / 4.0f;
		animation->frameLengths[i+2] = 1 / 4.0f;

	}

	auto func0 = [](float t){ t = (t*0.5) + 0.5f;	return (3 * t*t - 2 * t*t*t - 0.5f) * 2; };
	auto func1 = [](float t){ t = (t*0.5);	return (3 * t*t - 2 * t*t*t - 0) * 2; };

	animation->functions[0] = func0;
	animation->functions[2] = func0;
	animation->functions[1] = func1;
	animation->functions[3] = func1;

	//for (int i = 0; i < walkAnimation.numFrames; ++i)
	//{
	//	animation->frameLengths[i] = 1 / (float)walkAnimation.numFrames;
	//	animation->functions[i] = [](float t){return t < 0.5 ? 0.0f : (t - 0.5f)*2.0f; };
	//}
}



void loadFrame(const char* fileName, KeyFrame* frame, char** names, int numNames)
{
	const KeyFrameSave* frameSave = loadKeyFrame(fileName);

	if (frameSave == 0)
		return;

	frame->numBones = frameSave->numBones;

	frame->jointAngles = new float2[frameSave->numBones];

	memcpy(frame->jointAngles, frameSave->jointAngles, sizeof(float2) * frameSave->numBones);

	frame->names = new int[frameSave->numBones];

	char* _name = frameSave->names;
	for (int i = 0; i < frameSave->numBones; ++i)
	{
		int len = frameSave->nameLen[i];
		char* name = new char[len + 1];
		name[len] = 0;

		memcpy(name, _name, frameSave->nameLen[i]);
		

		for (int j = 0; j < numNames; ++j)
		{
			if (strcmp(name, names[j]) == 0)
			{
				frame->names[i] = j;
				break;
			}
		}

		delete[] name;

		_name += frameSave->nameLen[i];
	}

}