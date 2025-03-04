#pragma once

#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glm/fwd.hpp>

#include <vector>

// because the glm::vec3 is giving me errors for no reason >:(
class vec3
{
	float x = 0.0f, y = 0.0f, z = 0.0f;
};

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

	void playAnimation();
	vec3 posAnim();
	vec3 rotAnim();
	vec3 scaleAnim();
};
