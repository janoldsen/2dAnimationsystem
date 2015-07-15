#ifndef ENTITY_H
#define ENTITY_H

#include "Animation.h"
#include "SDL.h"
#include "MyMath.h"


struct Entity
{
	float velocity;
	
	Skeleton skeleton;
	
	bool morphing;
	KeyFrame morphFrame;

	float height;
	float crouchSpeed;
	KeyFrame standFrame;
	KeyFrame crouchFrame;

	float2 standOrigin;
	float2 walkOrigin;
	float2 runOrigin;
	float2 crouchOrigin;
	float2 crouchWalkOrigin;

	AnimationInstance standAnim;
	AnimationInstance walkAnim;
	AnimationInstance runAnim;

	AnimationInstance crouchAnim;
	AnimationInstance crouchWalkAnim;

	float walkRadius;
	float runRadius;

	float2 origin;

	enum State
	{
		STAND,
		WALK,
		JOG,
		RUN,
	} state;

	float walkRunBlend;

};


void updateEntity(Entity* entity, float dt);
void setVelocity(Entity* entity, float v);
void setCrouchSpeed(Entity* entity, float v);
void drawEntity(Entity* entity, View* view, SDL_Renderer* renderer);


#endif