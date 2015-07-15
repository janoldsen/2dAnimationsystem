#include "Entity.h"
#include "MyMath.h"





void updateEntity(Entity* entity, float dt)
{


	float crouchT;
	if (entity->crouchSpeed)
	{
		entity->height = MIN(MAX(entity->height + entity->crouchSpeed * dt, 0.0f), 1.0f);
		
		static const float b = 2;
		static const float a = 5 * PI / (2 * exp(b));

		
		float x = 1.0f - entity->height;

		if (entity->crouchSpeed < 0.0f)
		{
			float x = 1.0f - entity->height;
			crouchT = 1 - cos(a*exp(x * b) * x) * (exp(-5 * x));
		}
		else
		{
			float x = entity->height;
			crouchT = cos(a*exp((x)* b) * (x)) * (exp(-5 * (x)));
		}

		if (entity->height == 0.0f || entity->height == 1.0f)
			entity->crouchSpeed = 0.0f;
	}


	switch (entity->state)
	{
	case Entity::STAND:
	{
		if (entity->height == 1.0f)
		{
			if (entity->morphing)
				entity->morphing = !animateMorphSkeleton(&entity->skeleton, &entity->standAnim, &entity->morphFrame, dt);

			if (!entity->morphing)
				animateSkeleton(&entity->skeleton, &entity->standAnim, dt);
		}
		else if (entity->height == 0.0f)
		{
			if (entity->morphing)
				entity->morphing = !animateMorphSkeleton(&entity->skeleton, &entity->crouchAnim, &entity->morphFrame, dt);

			if (!entity->morphing)
				animateSkeleton(&entity->skeleton, &entity->crouchAnim, dt);
		}
		else
		{
			if (entity->morphing)
			{
				entity->morphing &= !(animateMorphFrames(&entity->standAnim, &entity->morphFrame, dt, &entity->skeleton, &entity->standFrame) == 0);
				entity->morphing &= !(animateMorphFrames(&entity->crouchAnim, &entity->morphFrame, dt, &entity->skeleton, &entity->crouchFrame) == 0);
			}

			if (!entity->morphing)
			{
				animateFrames(&entity->standAnim, dt, &entity->skeleton, &entity->standFrame);
				animateFrames(&entity->crouchAnim, dt, &entity->skeleton, &entity->crouchFrame);
			}

			blendFramesSkeleton(&entity->skeleton, &entity->standFrame, &entity->crouchFrame, crouchT);
		}
		break;
	}
	case Entity::WALK:
	{
		if (entity->height == 1.0f)
		{
			if (entity->morphing)
				entity->morphing = !animateMorphSkeleton(&entity->skeleton, &entity->walkAnim, &entity->morphFrame, dt);

			if (!entity->morphing)
				animateSkeleton(&entity->skeleton, &entity->walkAnim, dt);
		}
		else if (entity->height == 0.0f)
		{
			if (entity->morphing)
				entity->morphing = !animateMorphSkeleton(&entity->skeleton, &entity->crouchWalkAnim, &entity->morphFrame, dt);

			if (!entity->morphing)
				animateSkeleton(&entity->skeleton, &entity->crouchWalkAnim, dt);
		}
		else
		{
			if (entity->morphing)
			{
				entity->morphing &= !(animateMorphFrames(&entity->walkAnim, &entity->morphFrame, dt, &entity->skeleton, &entity->standFrame) == 0);
				entity->morphing &= !(animateMorphFrames(&entity->crouchWalkAnim, &entity->morphFrame, dt, &entity->skeleton, &entity->crouchFrame) == 0);
			}

			if (!entity->morphing)
			{
				animateFrames(&entity->walkAnim, dt, &entity->skeleton, &entity->standFrame);
				animateFrames(&entity->crouchWalkAnim, dt, &entity->skeleton, &entity->crouchFrame);
			}

			blendFramesSkeleton(&entity->skeleton, &entity->standFrame, &entity->crouchFrame, crouchT);
		}


		break;
	}
	case Entity::RUN:
	{
		if (entity->height == 1.0f)
		{
			if (entity->morphing)
				entity->morphing = !animateMorphSkeleton(&entity->skeleton, &entity->runAnim, &entity->morphFrame, dt);

			if (!entity->morphing)
				animateSkeleton(&entity->skeleton, &entity->runAnim, dt);
		}
		else if (entity->height == 0.0f)
		{
			if (entity->morphing)
				entity->morphing = !animateMorphSkeleton(&entity->skeleton, &entity->crouchWalkAnim, &entity->morphFrame, dt);

			if (!entity->morphing)
				animateSkeleton(&entity->skeleton, &entity->crouchWalkAnim, dt);
		}
		else
		{
			if (entity->morphing)
			{
				entity->morphing &= !(animateMorphFrames(&entity->runAnim, &entity->morphFrame, dt, &entity->skeleton, &entity->standFrame) == 0);
				entity->morphing &= !(animateMorphFrames(&entity->crouchWalkAnim, &entity->morphFrame, dt, &entity->skeleton, &entity->crouchFrame) == 0);
			}

			if (!entity->morphing)
			{
				animateFrames(&entity->runAnim, dt, &entity->skeleton, &entity->standFrame);
				animateFrames(&entity->crouchWalkAnim, dt, &entity->skeleton, &entity->crouchFrame);
			}

			blendFramesSkeleton(&entity->skeleton, &entity->standFrame, &entity->crouchFrame, crouchT);
		}

		break;
	}
	case Entity::JOG:
	{
		if (entity->height == 1.0f)
		{
			if (entity->morphing)
			{
				KeyFrame walk;
				walk.jointAngles = new float2[entity->skeleton.numBones];
				walk.names = new int[entity->skeleton.numBones];
				walk.numBones = entity->skeleton.numBones;

				KeyFrame run;
				run.jointAngles = new float2[entity->skeleton.numBones];
				run.names = new int[entity->skeleton.numBones];
				run.numBones = entity->skeleton.numBones;

				KeyFrame blend;
				blend.jointAngles = new float2[entity->skeleton.numBones];
				blend.names = new int[entity->skeleton.numBones];
				blend.numBones = entity->skeleton.numBones;

				entity->morphing &= !(animateMorphFrames(&entity->walkAnim, &entity->morphFrame, dt, &entity->skeleton, &walk) == 0);
				entity->morphing &= !(animateMorphFrames(&entity->runAnim, &entity->morphFrame, dt, &entity->skeleton, &run) == 0);

				setSkeletonFrame(&entity->skeleton, blendFrames(&walk, &run, entity->walkRunBlend, &entity->skeleton, &blend));

				delete[] walk.names;
				delete[] walk.jointAngles;
				delete[] run.jointAngles;
				delete[] run.names;
				delete[] blend.jointAngles;
				delete[] blend.names;
			}


			if (!entity->morphing)
			{
				AnimationInstance* anims[2] = { &entity->walkAnim, &entity->runAnim };
				animateBlendSkeleton(&entity->skeleton, anims, 2, &entity->walkRunBlend, dt);
			}

		}
		else if (entity->height == 0.0f)
		{
			if (entity->morphing)
				entity->morphing = !animateMorphSkeleton(&entity->skeleton, &entity->crouchWalkAnim, &entity->morphFrame, dt);

			if (!entity->morphing)
				animateSkeleton(&entity->skeleton, &entity->crouchWalkAnim, dt);
		}
		else
		{
			KeyFrame walk;
			walk.jointAngles = new float2[entity->skeleton.numBones];
			walk.names = new int[entity->skeleton.numBones];
			walk.numBones = entity->skeleton.numBones;

			KeyFrame run;
			run.jointAngles = new float2[entity->skeleton.numBones];
			run.names = new int[entity->skeleton.numBones];
			run.numBones = entity->skeleton.numBones;

			if (entity->morphing)
			{
				entity->morphing &= !(animateMorphFrames(&entity->walkAnim, &entity->morphFrame, dt, &entity->skeleton, &walk) == 0);
				entity->morphing &= !(animateMorphFrames(&entity->runAnim, &entity->morphFrame, dt, &entity->skeleton, &run) == 0);

				entity->morphing &= !(animateMorphFrames(&entity->crouchWalkAnim, &entity->morphFrame, dt, &entity->skeleton, &entity->crouchFrame) == 0);

				blendFrames(&walk, &run, entity->walkRunBlend, &entity->skeleton, &entity->standFrame);
			}

			if (!entity->morphing)
			{
				AnimationInstance* anims[2] = { &entity->walkAnim, &entity->runAnim };
				animateBlendFrames(anims, entity->walkRunBlend, dt, &entity->skeleton, &entity->standFrame);
				animateFrames(&entity->crouchWalkAnim, dt, &entity->skeleton, &entity->crouchFrame);
			}


			blendFramesSkeleton(&entity->skeleton, &entity->standFrame, &entity->crouchFrame, crouchT);


			delete[] walk.names;
			delete[] walk.jointAngles;
			delete[] run.jointAngles;
			delete[] run.names;
		}

		break;
	}
	}


	// update origin


	if (entity->height == 1.0f)
	{
		switch (entity->state)
		{
		case Entity::STAND: entity->origin = entity->standOrigin; break;
		case Entity::WALK: entity->origin = entity->walkOrigin; break;
		case Entity::RUN: entity->origin = entity->runOrigin; break;
		case Entity::JOG: entity->origin = interpolate(entity->walkOrigin, entity->runOrigin, entity->walkRunBlend); break;
		}
	}
	else if (entity->height == 0.0f)
	{
		switch (entity->state)
		{
		case Entity::STAND: entity->origin = entity->crouchOrigin; break;
		case Entity::WALK:
		case Entity::RUN: 
		case Entity::JOG: entity->origin = entity->crouchWalkOrigin; break;
		}
	}
	else
	{
		switch (entity->state)
		{
		case Entity::STAND: entity->origin = interpolate(entity->standOrigin, entity->crouchOrigin, crouchT); break;
		case Entity::WALK: entity->origin = interpolate(entity->walkOrigin, entity->crouchWalkOrigin, crouchT); break;
		case Entity::RUN: entity->origin = interpolate(entity->runOrigin, entity->crouchWalkOrigin, crouchT); break;
		case Entity::JOG:entity->origin = interpolate(interpolate(entity->walkOrigin, entity->runOrigin, entity->walkRunBlend), entity->crouchWalkOrigin, crouchT); break;
		}
	}
}


void setVelocity(Entity* entity, float v)
{

	Entity::State oldState = entity->state;

	if (v == 0.0f)
	{
		entity->state = Entity::STAND;

	}
	else if (v < 200.0f)
	{
		entity->state = Entity::WALK;

		entity->walkRunBlend = 0.0f;

		entity->walkAnim.length = 1.0f / ((v) / (0.6*PI*entity->walkRadius));
		entity->crouchWalkAnim.length = 1.0f / ((v) / (0.6*PI*entity->walkRadius));

	}
	else if (v > 300.0f)
	{
		entity->state = Entity::RUN;

		entity->walkRunBlend = 1.0f;
		entity->runAnim.length = 1.0f / ((v) / (0.6*PI*entity->runRadius));
		entity->crouchWalkAnim.length = 1.0f / ((v) / (0.6*PI*entity->walkRadius));

	}
	else
	{
		entity->state = Entity::JOG;

		entity->walkRunBlend = (v - 200.0f) / (float)(300.0f - 200.0f);
		float length = interpolate(1.0f / ((v) / (0.6*PI*entity->walkRadius)), 1.0f / ((v) / (0.6*PI*entity->runRadius)), entity->walkRunBlend);


		if (oldState == Entity::RUN)
			entity->walkAnim.t = entity->runAnim.t;

		if (oldState == Entity::WALK)
			entity->runAnim.t = entity->walkAnim.t;

		entity->runAnim.length = length;
		entity->walkAnim.length = length;

		entity->crouchWalkAnim.length = 1.0f / ((v) / (0.6*PI*entity->walkRadius));


	}


	if (entity->state != oldState)
	{
		entity->morphing = true;
		memcpy(entity->morphFrame.jointAngles, entity->skeleton.jointAngles, sizeof(float2) * entity->skeleton.numBones);

		if (entity->state == Entity::STAND && entity->state == Entity::RUN || oldState == Entity::WALK || oldState == Entity::JOG)
		{
			entity->runAnim.currFrame = 0;
			entity->runAnim.t = 0.0f;
			entity->walkAnim.currFrame = 0;
			entity->walkAnim.t = 0.0f;
			entity->crouchWalkAnim.currFrame = 0;
			entity->crouchWalkAnim.t = 0.0f;

		}
		else if (oldState == Entity::STAND)
		{
			entity->standAnim.t = 0.0f;
		}
	}

	entity->velocity = v;
}



void drawEntity(Entity* entity, View* view, SDL_Renderer* renderer)
{
	
	float2 origin = entity->origin;
	origin.x = 0;
	drawSkeleton(renderer, view, float2{ 0, +120 } - origin, 0, &entity->skeleton);

	SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
	floatRect rect;
	rect.pos = origin - float2{5.0f, 5.0f };
	rect.extend = float2{10.0f, 10.0f };
	drawRect(renderer, view, rect);
}

