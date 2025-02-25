#pragma once

#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <vector>

class Vec3Key
{
public:
	float time; // 0-max
	glm::vec3 value;
};

class AnimationClip // animation data
{
public:
	float duration; // 0-duration
	// times in ascending order
	std::vector<Vec3Key> positionKeys; // position keyframes
	std::vector<Vec3Key> rotationKeys; // rotation keyframes
	std::vector<Vec3Key> scaleKeys; // scale keyframes
};

class Animator // plays back
{
	AnimationClip* clip;
	bool isPlaying, isLoop;
	float playbackSpeed; // negative is backwards
	float playbackTime; // current time in animation
};
