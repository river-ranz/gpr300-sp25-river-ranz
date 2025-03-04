#include "animation.h"

namespace riv
{
	vec3 Animator::posAnim()
	{
		Vec3Key next, prev;
		// find next position
		for (int i = 0; i < clip->positionKeys.size(); i++)
		{
			if (clip->positionKeys[i].time >= playbackTime)
			{
				next = clip->positionKeys[i];
				if (i != 0) { prev = clip->positionKeys[i - 1]; }
				break;
			}
		}
		return lerp(prev.value, next.value, playbackTime);
	}

	vec3 Animator::rotAnim()
	{
		Vec3Key next, prev;
		// find next rotation
		for (int i = 0; i < clip->rotationKeys.size(); i++)
		{
			if (clip->rotationKeys[i].time >= playbackTime)
			{
				next = clip->rotationKeys[i];
				if (i != 0) { prev = clip->rotationKeys[i - 1]; }
				break;
			}
		}
		return lerp(prev.value, next.value, playbackTime);
	}

	vec3 Animator::scaleAnim()
	{
		Vec3Key next, prev;
		// find next scale
		for (int i = 0; i < clip->scaleKeys.size(); i++)
		{
			if (clip->scaleKeys[i].time >= playbackTime)
			{
				next = clip->scaleKeys[i];
				if (i != 0) { prev = clip->scaleKeys[i - 1]; }
				break;
			}
		}
		return lerp(prev.value, next.value, playbackTime);
	}


}