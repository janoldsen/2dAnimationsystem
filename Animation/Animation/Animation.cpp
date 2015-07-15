
#include "Animation.h"
#include <memory>
#include "structArray.h"

#include "DebugDraw.h"


#include <assert.h>




void drawSkeleton(SDL_Renderer* renderer, View* pView, float2 position, float angle, Bone* bones, int numBones)
{
	Transform *intermediates = new Transform[numBones];
	
	intermediates[0].position = position + rotate(bones[0].jointPos, angle);
	intermediates[0].angle = rotate(float2{ cos(angle), sin(angle) }, bones[0].jointAngle);


	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

	float2 jointPos = intermediates[0].position;

	floatRect jointRender = { jointPos.x - 5.0f, jointPos.y - 5.0f, 10, 10 };
	drawRect(renderer, pView, jointRender);

	float2 xAxis = (jointPos + rotate(float2{ 10, 0 }, intermediates[0].angle));
	float2 yAxis = (jointPos + rotate(float2{ 0, 10 }, intermediates[0].angle));
	
	drawLine(renderer, pView, jointPos, xAxis);
	drawLine(renderer, pView, jointPos, yAxis);

	
	for (int i = 1; i < numBones; i++)
	{

		intermediates[i].position = intermediates[bones[i].parentJoint].position + rotate(bones[i].jointPos, intermediates[bones[i].parentJoint].angle);
		intermediates[i].angle = rotate(intermediates[bones[i].parentJoint].angle, bones[i].jointAngle);
		jointPos = intermediates[i].position;

		jointRender = { jointPos.x - 5.0f, jointPos.y - 5.0f, 10, 10 };
		drawRect(renderer, pView, jointRender);

		xAxis = jointPos + rotate(float2{ 10, 0 }, intermediates[i].angle);
		yAxis = jointPos + rotate(float2{ 0, 10 }, intermediates[i].angle);

		drawLine(renderer, pView, intermediates[i].position, xAxis);
		drawLine(renderer, pView, intermediates[i].position, yAxis);


		float angle = -atan2(intermediates[bones[i].parentJoint].position.y - intermediates[i].position.y, intermediates[bones[i].parentJoint].position.x - intermediates[i].position.x);
		
		float2 bonePoint1Render = jointPos + rotate({ 0, 5 }, angle);
		float2 bonePoint2Render = jointPos + rotate({ 0, -5 }, angle);
		float2 parentJointPos = intermediates[bones[i].parentJoint].position;

	
		drawLine(renderer, pView, parentJointPos, bonePoint1Render);
		drawLine(renderer, pView, intermediates[bones[i].parentJoint].position, bonePoint2Render);


	}

	delete[] intermediates;
}

void drawSkeleton(SDL_Renderer* renderer, View* pView, float2 position, float angle, Skeleton* skeleton)
{
	Transform *intermediates = new Transform[skeleton->numBones];

	intermediates[0].position = position + rotate(skeleton->jointPositions[0], angle);
	intermediates[0].angle = rotate(float2{ cos(angle), sin(angle) }, skeleton->jointAngles[0]);


	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

	float2 jointPos = intermediates[0].position;

	floatRect jointRender = { jointPos.x - 5.0f, jointPos.y - 5.0f, 10, 10 };
	drawRect(renderer, pView, jointRender);

	float2 xAxis = (jointPos + rotate(float2{ 10, 0 }, intermediates[0].angle));
	float2 yAxis = (jointPos + rotate(float2{ 0, 10 }, intermediates[0].angle));

	drawLine(renderer, pView, jointPos, xAxis);
	drawLine(renderer, pView, jointPos, yAxis);


	for (int i = 1; i < skeleton->numBones; i++)
	{

		intermediates[i].position = intermediates[skeleton->parentJoints[i]].position + rotate(skeleton->jointPositions[i], intermediates[skeleton->parentJoints[i]].angle);
		intermediates[i].angle = rotate(intermediates[skeleton->parentJoints[i]].angle, skeleton->jointAngles[i]);
		jointPos = intermediates[i].position;

		jointRender = { jointPos.x - 5.0f, jointPos.y - 5.0f, 10, 10 };
		drawRect(renderer, pView, jointRender);

		xAxis = jointPos + rotate(float2{ 10, 0 }, intermediates[i].angle);
		yAxis = jointPos + rotate(float2{ 0, 10 }, intermediates[i].angle);

		drawLine(renderer, pView, intermediates[i].position, xAxis);
		drawLine(renderer, pView, intermediates[i].position, yAxis);


		float angle = -atan2(intermediates[skeleton->parentJoints[i]].position.y - intermediates[i].position.y, intermediates[skeleton->parentJoints[i]].position.x - intermediates[i].position.x);

		float2 bonePoint1Render = jointPos + rotate({ 0, 5 }, angle);
		float2 bonePoint2Render = jointPos + rotate({ 0, -5 }, angle);
		float2 parentJointPos = intermediates[skeleton->parentJoints[i]].position;


		drawLine(renderer, pView, parentJointPos, bonePoint1Render);
		drawLine(renderer, pView, intermediates[skeleton->parentJoints[i]].position, bonePoint2Render);


	}

	delete[] intermediates;
}

void sortSkeleton(Bone* bones, int numBones, int* newIds)
{

	struct SortStruct
	{
		int id;
		int depth;
	};

	
	SortStruct* toSort = new SortStruct[numBones];

	for (int i = 0; i < numBones; ++i)
	{
		toSort[i].id = i;
		toSort[i].depth = 0;
		
		Bone* bone = &bones[i];

		while (bone->parentJoint != -1)
		{
			toSort[i].depth++;
			bone = &bones[bone->parentJoint];
		}

	}

	qsort(toSort, numBones, sizeof(SortStruct), [](const void* a, const void* b)->int{return ((SortStruct*)a)->depth > ((SortStruct*)b)->depth; });

	Bone* temp = new Bone[numBones];
	memcpy(temp, bones, numBones * sizeof(Bone));

	for (int i = 0; i < numBones; i++)
	{
		bones[i] = temp[toSort[i].id];

		if (newIds)
		{
			newIds[toSort[i].id]= i;
		}


		for (int j = 0; j < numBones; ++j)
		{
			if (toSort[j].id == bones[i].parentJoint)
			{
				bones[i].parentJoint = j;
				break;
			}
		}
	}



	delete[] temp;
	delete[] toSort;
}



Transform getBoneWorldTransform(Bone* skeleton, int numBones, int bone)
{
	Transform transform = { { 0, 0 }, { 1, 0 } };

	StructArray<int> hierarchy(6);
	hierarchy.push(bone);

	while (skeleton[bone].parentJoint != -1)
	{
		hierarchy.push(skeleton[bone].parentJoint);
		bone = skeleton[bone].parentJoint;
	}

	for (int i = hierarchy.size() - 1; i >= 0; i--)
	{
		transform.position += rotate(skeleton[hierarchy[i]].jointPos, transform.angle);
		transform.angle =  rotate(transform.angle, skeleton[hierarchy[i]].jointAngle);

	}

	return transform;
}

Transform getBoneWorldTransform(Skeleton* skeleton, int bone)
{
	Transform transform = { { 0, 0 }, { 1, 0 } };

	StructArray<int> hierarchy(6);
	hierarchy.push(bone);

	while (skeleton->parentJoints[bone] != -1)
	{
		hierarchy.push(skeleton->parentJoints[bone]);
		bone = skeleton->parentJoints[bone];
	}

	for (int i = hierarchy.size() - 1; i >= 0; i--)
	{
		transform.position += rotate(skeleton->jointPositions[hierarchy[i]], transform.angle);
		transform.angle = rotate(transform.angle, skeleton->jointAngles[hierarchy[i]]);

	}

	return transform;
}


Transform getBoneWorldTransform(Skeleton* skeleton, KeyFrame* frame, int bone)
{
	Transform transform = { { 0, 0 }, { 1, 0 } };

	StructArray<int> hierarchy(6);
	hierarchy.push(bone);

	while (skeleton->parentJoints[bone] != -1)
	{
		hierarchy.push(skeleton->parentJoints[bone]);
		bone = skeleton->parentJoints[bone];
	}

	for (int i = hierarchy.size() - 1; i >= 0; i--)
	{
		float2 angle;
		for (int j = 0; j < frame->numBones; ++j)
		{
			if (frame->names[j] == skeleton->names[hierarchy[i]])
			{
				angle = frame->jointAngles[j];
				break;
			}
		}


		transform.position += rotate(skeleton->jointPositions[hierarchy[i]], transform.angle);
		transform.angle = rotate(transform.angle, angle);

	}

	return transform;
}

float2 getNeutralBoneWorldPos(Bone* skeleton, int numBones, int bone)
{
	float2 pos = skeleton[bone].jointPos;

	while (skeleton[bone].parentJoint != -1)
	{
		bone = skeleton[bone].parentJoint;
		pos += skeleton[bone].jointPos;
	}

	return pos;
}


float2 getNeutralBoneWorldPos(Skeleton* skeleton, int bone)
{
	float2 pos = skeleton->jointPositions[bone];

	while (skeleton->parentJoints[bone] != -1)
	{
		bone = skeleton->parentJoints[bone];
		pos += skeleton->jointPositions[bone];
	}

	return pos;
}



void animateCCDIK(Bone* skeleton, Constraint* constraints, int numBones, int targetBone, float2 targetLocation)
{


	int bone = targetBone;

	while (skeleton[bone].parentJoint != -1)
	{
		bone = skeleton[bone].parentJoint;

		float2 effectorPos = getBoneWorldTransform(skeleton, numBones, targetBone).position;
		float2 currPos = getBoneWorldTransform(skeleton, numBones, bone).position;

		float angle = asin(((effectorPos.x - currPos.x)*(targetLocation.y - currPos.y) - (effectorPos.y - currPos.y)*(targetLocation.x - currPos.x)) /
			(length(effectorPos - currPos) * length(targetLocation - currPos)));


		skeleton[bone].jointAngle = getRotation(targetLocation - currPos, effectorPos - currPos);


	}

}


void animateCCDIKSelection(Bone* skeleton, Constraint* constraints, int numBones, int* selection, int selectionSize, int targetBone, float2 targetLocation)
{


	int bone = targetBone;

	while (skeleton[bone].parentJoint != -1)
	{
		bone = skeleton[bone].parentJoint;

		bool selected = false;
		for (int i = 0; i < selectionSize; i++)
		{
			if (selection[i] == bone)
				selected = true;
		}

		if (!selected)
			continue;


		float2 effectorPos = getBoneWorldTransform(skeleton, numBones, targetBone).position;
		float2 currPos = getBoneWorldTransform(skeleton, numBones, bone).position;

		float2 angle = getRotation(targetLocation - currPos, effectorPos - currPos);
		
		skeleton[bone].jointAngle = rotate(skeleton[bone].jointAngle, angle);
	}

}


void animateSkeleton(Skeleton* skeleton, AnimationInstance* anim, float dt)
{
	// DEBUG
	static float lastX = 0;
	static float lastY = 0;
	//
	

	float _t = 1/(anim->animation->frameLengths[anim->currFrame] * anim->length) * dt;

	if (anim->t + _t  > 1.0)
	{
		dt -= (1.0f - anim->t) / ((1/anim->animation->frameLengths[anim->currFrame] * anim->length));
		anim->currFrame = (anim->currFrame + 1) % anim->animation->numFrames;

		anim->t = 1/(anim->animation->frameLengths[anim->currFrame] * anim->length) * dt;
		
		// DEBUG
		DEBUG_clear();
		lastX = 0;
		lastY = 0;
		//

	}
	else
	{
		anim->t += _t;
	}

	float t = (anim->animation->functions[anim->currFrame])(anim->t);

	//DEBUG
	DEBUG_drawLine(int2{ lastX * 100.0f, 100.0f - lastY * 100.0f }, int2{ anim->t * 100.0f, 100.0f - t * 100.0f });

	lastX = anim->t;
	lastY = t;

	//

	int nextFrame = (anim->currFrame + 1) % anim->animation->numFrames;

	for (int i = 0; i < skeleton->numBones; ++i)
	{
		float2* r0 = &skeleton->jointAngles[i];
		float2* r1 = &skeleton->jointAngles[i];
		
		for (int j = 0; j < anim->animation->frames[anim->currFrame].numBones; ++j)
		{
			if (anim->animation->frames[anim->currFrame].names[j] == skeleton->names[i])
			{
				r0 = &anim->animation->frames[anim->currFrame].jointAngles[j];
				break;
			}
		}

		for (int j = 0; j < anim->animation->frames[nextFrame].numBones; ++j)
		{
			if (anim->animation->frames[nextFrame].names[j] == skeleton->names[i])
			{
				r1 = &anim->animation->frames[nextFrame].jointAngles[j];
				break;
			}
		}


		if (*r0 != *r1)
		{
			skeleton->jointAngles[i] = slerp(*r0, *r1, t);
		}
		else
		{
			skeleton->jointAngles[i] = *r0;
		}

	}


}

bool animateMorphSkeleton(Skeleton* skeleton, AnimationInstance* anim, KeyFrame* morphFrame, float dt)
{
	// DEBUG
	static float lastX = 0;
	static float lastY = 0;
	//


	float _t = 1 / (anim->animation->frameLengths[anim->currFrame] * anim->length) * dt;

	if (anim->t + _t  > 1.0)
	{
		dt -= (1.0f - anim->t) / ((1 / anim->animation->frameLengths[anim->currFrame] * anim->length));
		anim->currFrame = (anim->currFrame + 1) % anim->animation->numFrames;

		anim->t = 1 / (anim->animation->frameLengths[anim->currFrame] * anim->length) * dt;

		// DEBUG
		DEBUG_clear();
		lastX = 0;
		lastY = 0;
		//

		return true;
	}
	else
	{
		anim->t += _t;
	}

	float t = (anim->animation->functions[anim->currFrame])(anim->t);

	//DEBUG
	DEBUG_drawLine(int2{ lastX * 100.0f, 100.0f - lastY * 100.0f }, int2{ anim->t * 100.0f, 100.0f - t * 100.0f });

	lastX = anim->t;
	lastY = t;

	//

	int nextFrame = (anim->currFrame + 1) % anim->animation->numFrames;

	for (int i = 0; i < skeleton->numBones; ++i)
	{
		float2* r0 = &skeleton->jointAngles[i];
		float2* r1 = &skeleton->jointAngles[i];

		for (int j = 0; j < morphFrame->numBones; ++j)
		{
			if (morphFrame->names[j] == skeleton->names[i])
			{
				r0 = &morphFrame->jointAngles[j];
				break;
			}
		}

		for (int j = 0; j < anim->animation->frames[nextFrame].numBones; ++j)
		{
			if (anim->animation->frames[nextFrame].names[j] == skeleton->names[i])
			{
				r1 = &anim->animation->frames[nextFrame].jointAngles[j];
				break;
			}
		}


		if (*r0 != *r1)
		{
			skeleton->jointAngles[i] = slerp(*r0, *r1, t);
		}
		else
		{
			skeleton->jointAngles[i] = *r0;
		}

	}

	return false;
}





void animateBlendSkeleton(Skeleton* skeleton, AnimationInstance** anims, int numAnims, float* blendFactors, float dt)
{
	// DEBUG
	static float lastX = 0;
	static float lastY = 0;
	//

	float* ts = new float[numAnims];

	for (int i = 0; i < numAnims; ++i)
	{
		float _dt = dt;
		float _t = 1 / (anims[i]->animation->frameLengths[anims[i]->currFrame] * anims[i]->length) * _dt;

		if (anims[i]->t + _t > 1.0)
		{
			_dt -= (1.0f - anims[i]->t) / ((1 / anims[i]->animation->frameLengths[anims[i]->currFrame] * anims[i]->length));
			anims[i]->currFrame = (anims[i]->currFrame + 1) % anims[i]->animation->numFrames;

			anims[i]->t = 1 / (anims[i]->animation->frameLengths[anims[i]->currFrame] * anims[i]->length) * _dt;

			// DEBUG
			DEBUG_clear();
			lastX = 0;
			lastY = 0;
			//

		}
		else
		{
			anims[i]->t += _t;
		}

		ts[i] = (anims[i]->animation->functions[anims[i]->currFrame])(anims[i]->t);

		//DEBUG
		DEBUG_drawLine(int2{ lastX * 100.0f, 100.0f - lastY * 100.0f }, int2{ anims[i]->t * 100.0f, 100.0f - ts[i] * 100.0f });

		lastX = anims[i]->t;
		lastY = ts[i];
		//
	}
	for (int j = 0; j < skeleton->numBones; ++j)
	{
		float2* angles = new float2[numAnims];


		for (int i = 0; i < numAnims; ++i)
		{
			int nextFrame = (anims[i]->currFrame + 1) % anims[i]->animation->numFrames;

			float2* r0 = &skeleton->jointAngles[j];
			float2* r1 = &skeleton->jointAngles[j];

			for (int k = 0; k < anims[i]->animation->frames[anims[i]->currFrame].numBones; ++k)
			{
				if (anims[i]->animation->frames[anims[i]->currFrame].names[k] == skeleton->names[j])
				{
					r0 = &anims[i]->animation->frames[anims[i]->currFrame].jointAngles[k];
					break;
				}
			}

			for (int k = 0; k < anims[i]->animation->frames[nextFrame].numBones; ++k)
			{
				if (anims[i]->animation->frames[nextFrame].names[k] == skeleton->names[j])
				{
					r1 = &anims[i]->animation->frames[nextFrame].jointAngles[k];
					break;
				}
			}

			if (*r0 != *r1)
			{
				angles[i] = slerp(*r0, *r1, ts[i]);
			}
			else
			{
				angles[i] = *r0;
			}
		}

		float2 angle = angles[0];
		for (int i = 1; i < numAnims; ++i)
		{
			angle = slerp(angle, angles[i], blendFactors[i - 1]);
		}

		skeleton->jointAngles[j] = angle;
		delete[] angles;
	}

	delete[] ts;
}

KeyFrame* animateFrames(AnimationInstance* anim, float dt, const Skeleton* skeletonRef, KeyFrame* keyFrame)
{

	float _t = 1 / (anim->animation->frameLengths[anim->currFrame] * anim->length) * dt;

	if (anim->t + _t  > 1.0)
	{
		dt -= (1.0f - anim->t) / ((1 / anim->animation->frameLengths[anim->currFrame] * anim->length));
		anim->currFrame = (anim->currFrame + 1) % anim->animation->numFrames;

		anim->t = 1 / (anim->animation->frameLengths[anim->currFrame] * anim->length) * dt;

	}
	else
	{
		anim->t += _t;
	}

	float t = (anim->animation->functions[anim->currFrame])(anim->t);


	int nextFrame = (anim->currFrame + 1) % anim->animation->numFrames;

	keyFrame->numBones = skeletonRef->numBones;

	for (int i = 0; i < skeletonRef->numBones; ++i)
	{
		float2* r0 = &skeletonRef->jointAngles[i];
		float2* r1 = &skeletonRef->jointAngles[i];

		for (int j = 0; j < anim->animation->frames[anim->currFrame].numBones; ++j)
		{
			if (anim->animation->frames[anim->currFrame].names[j] == skeletonRef->names[i])
			{
				r0 = &anim->animation->frames[anim->currFrame].jointAngles[j];
				break;
			}
		}

		for (int j = 0; j < anim->animation->frames[nextFrame].numBones; ++j)
		{
			if (anim->animation->frames[nextFrame].names[j] == skeletonRef->names[i])
			{
				r1 = &anim->animation->frames[nextFrame].jointAngles[j];
				break;
			}
		}


		if (*r0 != *r1)
		{
			keyFrame->jointAngles[i] = slerp(*r0, *r1, t);
		}
		else
		{
			keyFrame->jointAngles[i] = *r0;
		}

		keyFrame->names[i] = skeletonRef->names[i];

	}

	return keyFrame;
}

KeyFrame* animateMorphFrames(AnimationInstance* anim, KeyFrame* morphFrame, float dt, const Skeleton* skeletonRef, KeyFrame* keyFrame)
{

	float _t = 1 / (anim->animation->frameLengths[anim->currFrame] * anim->length) * dt;

	if (anim->t + _t  > 1.0)
	{
		dt -= (1.0f - anim->t) / ((1 / anim->animation->frameLengths[anim->currFrame] * anim->length));
		anim->currFrame = (anim->currFrame + 1) % anim->animation->numFrames;

		anim->t = 1 / (anim->animation->frameLengths[anim->currFrame] * anim->length) * dt;

		return 0;

	}
	else
	{
		anim->t += _t;
	}

	float t = (anim->animation->functions[anim->currFrame])(anim->t);


	int nextFrame = (anim->currFrame + 1) % anim->animation->numFrames;

	keyFrame->numBones = skeletonRef->numBones;

	for (int i = 0; i < skeletonRef->numBones; ++i)
	{
		float2* r0 = &skeletonRef->jointAngles[i];
		float2* r1 = &skeletonRef->jointAngles[i];

		for (int j = 0; j < morphFrame->numBones; ++j)
		{
			if (morphFrame->names[j] == skeletonRef->names[i])
			{
				r0 = &morphFrame->jointAngles[j];
				break;
			}
		}

		for (int j = 0; j < anim->animation->frames[nextFrame].numBones; ++j)
		{
			if (anim->animation->frames[nextFrame].names[j] == skeletonRef->names[i])
			{
				r1 = &anim->animation->frames[nextFrame].jointAngles[j];
				break;
			}
		}


		if (*r0 != *r1)
		{
			keyFrame->jointAngles[i] = slerp(*r0, *r1, t);
		}
		else
		{
			keyFrame->jointAngles[i] = *r0;
		}

		keyFrame->names[i] = skeletonRef->names[i];

	}

	return keyFrame;
}


KeyFrame* animateBlendFrames(AnimationInstance** anims, float blendFactor, float dt, const Skeleton* skeletonRef, KeyFrame* keyFrame)
{
	float ts[2];

	for (int i = 0; i < 2; ++i)
	{
		float _dt = dt;
		float _t = 1 / (anims[i]->animation->frameLengths[anims[i]->currFrame] * anims[i]->length) * _dt;

		if (anims[i]->t + _t > 1.0)
		{
			_dt -= (1.0f - anims[i]->t) / ((1 / anims[i]->animation->frameLengths[anims[i]->currFrame] * anims[i]->length));
			anims[i]->currFrame = (anims[i]->currFrame + 1) % anims[i]->animation->numFrames;

			anims[i]->t = 1 / (anims[i]->animation->frameLengths[anims[i]->currFrame] * anims[i]->length) * _dt;
		}
		else
		{
			anims[i]->t += _t;
		}

		ts[i] = (anims[i]->animation->functions[anims[i]->currFrame])(anims[i]->t);
	}

	keyFrame->numBones = skeletonRef->numBones;

	for (int j = 0; j < skeletonRef->numBones; ++j)
	{
		float2 angles[2];


		for (int i = 0; i < 2; ++i)
		{
			int nextFrame = (anims[i]->currFrame + 1) % anims[i]->animation->numFrames;

			float2* r0 = &skeletonRef->jointAngles[j];
			float2* r1 = &skeletonRef->jointAngles[j];

			for (int k = 0; k < anims[i]->animation->frames[anims[i]->currFrame].numBones; ++k)
			{
				if (anims[i]->animation->frames[anims[i]->currFrame].names[k] == skeletonRef->names[j])
				{
					r0 = &anims[i]->animation->frames[anims[i]->currFrame].jointAngles[k];
					break;
				}
			}

			for (int k = 0; k < anims[i]->animation->frames[nextFrame].numBones; ++k)
			{
				if (anims[i]->animation->frames[nextFrame].names[k] == skeletonRef->names[j])
				{
					r1 = &anims[i]->animation->frames[nextFrame].jointAngles[k];
					break;
				}
			}

			if (*r0 != *r1)
			{
				angles[i] = slerp(*r0, *r1, ts[i]);
			}
			else
			{
				angles[i] = *r0;
			}
		}

		
		float2 angle = slerp(angles[0], angles[1], blendFactor);

		keyFrame->jointAngles[j] = angle;
		keyFrame->names[j] = skeletonRef->names[j];
	}

	return keyFrame;
}


void blendFramesSkeleton(Skeleton* skeleton, KeyFrame* frame0, KeyFrame* frame1, float blendFactor)
{
	for (int i = 0; i < skeleton->numBones; ++i)
	{

		float2* r0 = &skeleton->jointAngles[i];
		float2* r1 = &skeleton->jointAngles[i];


		for (int j = 0; j < frame0->numBones; ++j)
		{
			if (frame0->names[j] == skeleton->names[i])
			{
				r0 = &frame0->jointAngles[j];
				break;
			}
		}

		for (int j = 0; j < frame1->numBones; ++j)
		{
			if (frame1->names[j] == skeleton->names[i])
			{
				r1 = &frame1->jointAngles[j];
				break;
			}
		}

		if (*r0 != *r1)
		{
			skeleton->jointAngles[i] = slerp(*r0, *r1, blendFactor);
		}
		else
		{
			skeleton->jointAngles[i] = *r0;
		}
	}

}


KeyFrame* blendFrames(KeyFrame* frame0, KeyFrame* frame1, float blendFactor, const Skeleton* skeletonRef, KeyFrame* keyFrame)
{
	keyFrame->numBones = skeletonRef->numBones;

	for (int i = 0; i < skeletonRef->numBones; ++i)
	{
		
		float2* r0 = &skeletonRef->jointAngles[i];
		float2* r1 = &skeletonRef->jointAngles[i];


		for (int j = 0; j < frame0->numBones; ++j)
		{
			if (frame0->names[j] == skeletonRef->names[i])
			{
				r0 = &frame0->jointAngles[j];
				break;
			}
		}

		for (int j = 0; j < frame1->numBones; ++j)
		{
			if (frame1->names[j] == skeletonRef->names[i])
			{
				r1 = &frame1->jointAngles[j];
				break;
			}
		}

		if (*r0 != *r1)
		{
			keyFrame->jointAngles[i] = slerp(*r0, *r1, blendFactor);
		}
		else
		{
			keyFrame->jointAngles[i] = *r0;
		}

		keyFrame->names[i] = skeletonRef->names[i];

	}

	return keyFrame;
}

void setSkeletonFrame(Skeleton* skeleton, KeyFrame* keyFrame)
{
	for (int i = 0; i < skeleton->numBones; ++i)
	{
		for (int j = 0; j < keyFrame->numBones; ++j)
		{
			if (keyFrame->names[j] == skeleton->names[i])
			{
				skeleton->jointAngles[i] = keyFrame->jointAngles[j];
				break;
			}
		}

	}
}

struct SkeletonHeader
{
	int numBones;
	int parentjoints;
	int jointPos;
	int nameLen;
	int names;
	int padding[8];
};




void saveSkeleton(const char* fileName, Bone* bones, Name* names, int numBones)
{
	SkeletonSave save;
	save.parentJoints = new int[numBones];
	save.jointPos = new float2[numBones];
	save.nameLen = new int[numBones];

	int totalNameLength = 0;
	for (int i = 0; i < numBones; ++i)
	{
		save.parentJoints[i] = bones[i].parentJoint;
		save.jointPos[i] = bones[i].jointPos;
		save.nameLen[i] = strlen(names[bones[i].name].string);

		totalNameLength += save.nameLen[i];
	}

	save.names = new char[totalNameLength];

	char* namePos = save.names;
	for (int i = 0; i < numBones; ++i)
	{
		memcpy(namePos, names[bones[i].name].string, save.nameLen[i]);
		namePos += save.nameLen[i];
	}


	SkeletonHeader header = { 0 };
	header.numBones = numBones;
	header.parentjoints = sizeof(header);
	header.jointPos = header.parentjoints + numBones * sizeof(int);
	header.nameLen = header.jointPos + numBones * sizeof(float2);
	header.names = header.nameLen + numBones * sizeof(int);


	FILE* file = fopen(fileName, "wb");

	fwrite(&header, sizeof(SkeletonHeader), 1, file);
	fwrite(save.parentJoints, sizeof(int), numBones, file);
	fwrite(save.jointPos, sizeof(float2), numBones, file);
	fwrite(save.nameLen, sizeof(int), numBones, file);
	fwrite(save.names, sizeof(char), totalNameLength, file);
	
	fclose(file);

	delete[] save.parentJoints;
	delete[] save.jointPos;
	delete[] save.nameLen;
	delete[] save.names;
}

static SkeletonSave gSkeletonSave = {0, new int[0], new float2[0], new int[0], new char[0] };


const SkeletonSave* loadSkeleton(const char* fileName)
{
	SkeletonHeader header;
	
	FILE* file = fopen(fileName, "rb");

	if (!file)
		return 0;


	fread(&header, sizeof(SkeletonHeader), 1, file);

	delete[] gSkeletonSave.parentJoints;
	delete[] gSkeletonSave.jointPos;
	delete[] gSkeletonSave.nameLen;
	delete[] gSkeletonSave.names;

	gSkeletonSave.numBones = header.numBones;

	gSkeletonSave.parentJoints = new int[gSkeletonSave.numBones];
	gSkeletonSave.jointPos = new float2[gSkeletonSave.numBones];
	gSkeletonSave.nameLen = new int[gSkeletonSave.numBones];


	fseek(file, header.parentjoints, SEEK_SET);
	fread(gSkeletonSave.parentJoints, sizeof(int), gSkeletonSave.numBones, file);
	fseek(file, header.jointPos, SEEK_SET);
	fread(gSkeletonSave.jointPos, sizeof(float2), gSkeletonSave.numBones, file);
	fseek(file, header.nameLen, SEEK_SET);
	fread(gSkeletonSave.nameLen, sizeof(int), gSkeletonSave.numBones, file);
	
	int totalNameLength = 0;
	for (int i = 0; i < gSkeletonSave.numBones; ++i)
		totalNameLength += gSkeletonSave.nameLen[i];

	gSkeletonSave.names = new char[totalNameLength];

	fseek(file, header.names, SEEK_SET);
	fread(gSkeletonSave.names, sizeof(char), totalNameLength, file);
	
	fclose(file);


	return &gSkeletonSave;
}


struct KeyFrameHeader
{
	int numBones;
	int jointAngle;
	int nameLen;
	int names;
	int padding[8];
};


void saveKeyFrame(const char* fileName, Bone* bones, Name* names, int numBones)
{
	KeyFrameSave save;
	save.jointAngles = new float2[numBones];
	save.nameLen = new int[numBones];

	int totalNameLength = 0;
	for (int i = 0; i < numBones; ++i)
	{
		save.jointAngles[i] = bones[i].jointAngle;
		save.nameLen[i] = strlen(names[bones[i].name].string);

		totalNameLength += save.nameLen[i];
	}

	save.names = new char[totalNameLength];

	char* namePos = save.names;
	for (int i = 0; i < numBones; ++i)
	{
		memcpy(namePos, names[bones[i].name].string, save.nameLen[i]);
		namePos += save.nameLen[i];
	}

	KeyFrameHeader header = { 0 };
	header.numBones = numBones;
	header.jointAngle = sizeof(header);
	header.nameLen = header.jointAngle + sizeof(float2) * numBones;
	header.names = header.nameLen + sizeof(int) * numBones;

	FILE* file = fopen(fileName, "wb");

	fwrite(&header, sizeof(KeyFrameHeader), 1, file);
	fwrite(save.jointAngles, sizeof(float2), numBones, file);
	fwrite(save.nameLen, sizeof(int), numBones, file);
	fwrite(save.names, sizeof(char), totalNameLength, file);

	fclose(file);

	delete[] save.jointAngles;
	delete[] save.nameLen;
	delete[] save.names;
}

static KeyFrameSave gKeyFrameSave = { 0, new float2[0], new int[0], new char[0] };

const KeyFrameSave* loadKeyFrame(const char* fileName)
{
	KeyFrameHeader header;

	FILE* file = fopen(fileName, "rb");

	if (!file)
		return 0;


	fread(&header, sizeof(KeyFrameHeader), 1, file);


	delete[] gKeyFrameSave.jointAngles;
	delete[] gKeyFrameSave.nameLen;
	delete[] gKeyFrameSave.names;

	gKeyFrameSave.numBones = header.numBones;

	gKeyFrameSave.jointAngles = new float2[gKeyFrameSave.numBones];
	gKeyFrameSave.nameLen = new int[gKeyFrameSave.numBones];


	fseek(file, header.jointAngle, SEEK_SET);
	fread(gKeyFrameSave.jointAngles, sizeof(float2), gKeyFrameSave.numBones, file);
	fseek(file, header.nameLen, SEEK_SET);
	fread(gKeyFrameSave.nameLen, sizeof(int), gKeyFrameSave.numBones, file);

	int totalNameLength = 0;
	for (int i = 0; i < gKeyFrameSave.numBones; ++i)
		totalNameLength += gKeyFrameSave.nameLen[i];

	gKeyFrameSave.names = new char[totalNameLength];

	fseek(file, header.names, SEEK_SET);
	fread(gKeyFrameSave.names, sizeof(char), totalNameLength, file);

	fclose(file);

	return &gKeyFrameSave;

}
