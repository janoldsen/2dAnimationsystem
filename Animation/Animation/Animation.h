
#ifndef ANIMATION_H
#define ANIMATION_H

#include "MyMath.h"
#include "Renderer.h"

struct Bone
{
	float2 jointPos;
	float2 jointAngle;
	int parentJoint;
	int name;
};




struct Skeleton
{
	int numBones;
	float2* jointPositions;
	float2* jointAngles;
	int* parentJoints;
	int* names;
};


struct KeyFrame
{
	int numBones;
	float2* jointAngles;
	int* names;
};


typedef float(Function)(float t);

struct Animation
{
	int numFrames;
	float* frameLengths;
	KeyFrame* frames;
	Function** functions;
};

struct AnimationInstance
{
	int currFrame;
	float t;
	float length;
	const Animation* animation;
};


struct Name
{
	char string[64];
};

struct Constraint
{
	float minAngle;
	float maxAngle;
};

struct Transform
{
	float2 position;
	float2 angle;
};


struct SkeletonSave
{
	int numBones;
	int* parentJoints;
	float2* jointPos;
	int* nameLen;
	char* names;
};


struct KeyFrameSave
{
	int numBones;
	float2* jointAngles;
	int* nameLen;
	char* names;
};


// expects a presorted sekeleton
void drawSkeleton(SDL_Renderer* renderer, View* pView, float2 position, float angle, Bone* bones, int numBones);
void drawSkeleton(SDL_Renderer* renderer, View* pView, float2 position, float angle, Skeleton* skeleton);

Transform getBoneWorldTransform(Bone* skeleton, int numBones, int bone);
Transform getBoneWorldTransform(Skeleton* skeleton, int bone);
Transform getBoneWorldTransform(Skeleton* skeleton, KeyFrame* frame,  int bone);

float2 getNeutralBoneWorldPos(Skeleton* skeleton, int bone);

void sortSkeleton(Bone* bones, int numBones, int* ids);
void animateIK(Bone* skeleton, int numBones, int targetBone, float2 targetLocation);
void animateCCDIK(Bone* skeleton, Constraint* constraints, int numBones, int targetBone, float2 targetLocation);
void animateCCDIKSelection(Bone* skeleton, Constraint* constraints, int numBones, int* selection, int selectionSize, int targetBone, float2 targetLocation);

void animateSkeleton(Skeleton* skeleton, AnimationInstance* animationInstance, float dt);
// returns true if morph complete
bool animateMorphSkeleton(Skeleton* skeleton, AnimationInstance* animationInstance, KeyFrame* morphFrame, float dt);
void animateBlendSkeleton(Skeleton* skeleton, AnimationInstance** anims, int numAnims, float* blendFactors, float dt);

KeyFrame* animateFrames(AnimationInstance* anim, float dt, const Skeleton* skeletonRef, KeyFrame* keyFrame);
// returns 0 if morph complete
KeyFrame* animateMorphFrames(AnimationInstance* anim, KeyFrame* morphFrame, float dt, const Skeleton* skeletonRef, KeyFrame* keyFrame);
KeyFrame* animateBlendFrames(AnimationInstance** anims, float blendFactor, float dt, const Skeleton* skeletonRef, KeyFrame* keyFrame);
void blendFramesSkeleton(Skeleton* skeleton, KeyFrame* frame0, KeyFrame* frame1, float blendFactor);
KeyFrame* blendFrames(KeyFrame* frame0, KeyFrame* frame1, float blendFactor, const Skeleton* skeletonRef, KeyFrame* keyFrame);


void setSkeletonFrame(Skeleton* skeleton, KeyFrame* frame);

void saveSkeleton(const char* fileName, Bone* bones, Name* names, int numBones);
const SkeletonSave* loadSkeleton(const char* fileName);


void saveKeyFrame(const char* fileName, Bone* bones, Name* names, int numBones);
const KeyFrameSave* loadKeyFrame(const char* fileName);

#endif