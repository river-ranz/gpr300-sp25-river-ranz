#pragma once

#include <stdio.h>
#include <vector>

#include "rivMath.h"

namespace riv
{
	class Vec3Key
	{
	public:
		float time = 0.0f; // 0-max
		vec3 value;
	};

	class AnimationClip // animation data
	{
	public:
		float duration = 0.0f; // 0-duration
		// times in ascending order
		std::vector<Vec3Key> positionKeys; // position keyframes
		std::vector<Vec3Key> rotationKeys; // rotation keyframes
		std::vector<Vec3Key> scaleKeys; // scale keyframes
	};

	class Animator // plays back
	{
	public:
		AnimationClip* clip;
		bool isPlaying, isLoop;
		float playbackSpeed; // negative is backwards
		float playbackTime; // current time in animation

		vec3 posAnim();
		vec3 rotAnim();
		vec3 scaleAnim();
	};

}