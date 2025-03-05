#include "animation.h"

namespace riv
{
	vec3 Animator::posAnim(vec3 currentPos)
	{
		Vec3Key next, prev;
		// find next position
		for (int i = 0; i < clip->positionKeys.size(); i++)
		{
			// positive play speed
			if (playbackSpeed > 0 && clip->positionKeys[i].time >= playbackTime)
			{
				next = clip->positionKeys[i];
				if (i != 0) { prev = clip->positionKeys[i - 1]; }
				return lerp(prev.value, next.value, invLerp(prev.time, next.time, playbackTime));
			}
			// negative play speed
			if (playbackSpeed < 0 && clip->positionKeys[i].time >= (clip->duration - playbackTime))
			{
				next = clip->positionKeys[i];
				if (i != 0) { prev = clip->positionKeys[i - 1]; }
				return lerp(prev.value, next.value, invLerp(prev.time, next.time, playbackTime));
			}
		}
		return currentPos;
	}

	vec3 Animator::rotAnim(vec3 currentRot)
	{
		Vec3Key next, prev;
		// find next rotation
		for (int i = 0; i < clip->rotationKeys.size(); i++)
		{
			if (clip->rotationKeys[i].time >= playbackTime)
			{
				next = clip->rotationKeys[i];
				if (i != 0) { prev = clip->rotationKeys[i - 1]; }
				return lerp(prev.value, next.value, invLerp(prev.time, next.time, playbackTime));
			}
			if (playbackSpeed < 0 && clip->rotationKeys[i].time >= (clip->duration - playbackTime))
			{
				next = clip->rotationKeys[i];
				if (i != 0) { prev = clip->rotationKeys[i - 1]; }
				return lerp(prev.value, next.value, invLerp(prev.time, next.time, playbackTime));
			}
		}
		return currentRot;
	}

	vec3 Animator::scaleAnim(vec3 currentScale)
	{
		Vec3Key next, prev;
		prev.value.x = 1; prev.value.y = 1; prev.value.z = 1;
		// find next scale
		for (int i = 0; i < clip->scaleKeys.size(); i++)
		{
			if (clip->scaleKeys[i].time >= playbackTime)
			{
				next = clip->scaleKeys[i];
				if (i != 0) { prev = clip->scaleKeys[i - 1]; }
				next.value.y = next.value.x;
				next.value.z = next.value.x;
				return lerp(prev.value, next.value, invLerp(prev.time, next.time, playbackTime));
			}
			if (playbackSpeed < 0 && clip->scaleKeys[i].time >= (clip->duration - playbackTime))
			{
				next = clip->scaleKeys[i];
				if (i != 0) { prev = clip->scaleKeys[i - 1]; }
				next.value.y = next.value.x;
				next.value.z = next.value.x;
				return lerp(prev.value, next.value, invLerp(prev.time, next.time, playbackTime));
			}
		}
		return currentScale;
	}


}