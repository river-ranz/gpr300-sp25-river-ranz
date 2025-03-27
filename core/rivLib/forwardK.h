#pragma once
#include <glm/glm.hpp>

namespace riv
{
	struct Joint
	{
		glm::mat4 localTransform, globalTransform;
		Joint* parent = nullptr;
		Joint** children = nullptr;
		unsigned int numChildren = 0, parentIndex = -1;
	};

	struct Skeleton
	{
		Joint** joints = nullptr;
		unsigned int nodeCount = 0;
	};

	void SolveFK(Joint* joint);
	void SolveFK(Skeleton hierarchy);
}