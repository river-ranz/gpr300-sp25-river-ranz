#include "animation.h"
#include <cmath>

void Animator::playAnimation()
{
	posAnim();
	rotAnim();
	scaleAnim();
	
	glm::mix
	pos = nextP;
	rot = nextR;
	scale = nextS;
}

void Animator::posAnim()
{
	// find next position
	for (int i = 0; i < clip->positionKeys.size(); i++)
	{
		if (clip->positionKeys[i].time >= playbackTime)
		{
			nextP = clip->positionKeys[i];
			if (i != 0) { prevP = clip->positionKeys[i - 1]; }
			break;
		}
	}
}

void Animator::rotAnim()
{
	// find next rotation
	for (int i = 0; i < clip->rotationKeys.size(); i++)
	{
		if (clip->rotationKeys[i].time >= playbackTime)
		{
			nextR = clip->rotationKeys[i];
			if (i != 0) { prevR = clip->rotationKeys[i - 1]; }
			break;
		}
	}
}

void Animator::scaleAnim()
{
	// find next scale
	for (int i = 0; i < clip->scaleKeys.size(); i++)
	{
		if (clip->scaleKeys[i].time >= playbackTime)
		{
			nextS = clip->scaleKeys[i];
			if (i != 0) { prevS = clip->scaleKeys[i - 1]; }
			break;
		}
	}
}

