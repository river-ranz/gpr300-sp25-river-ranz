#pragma once
#include <glm/glm.hpp>

namespace riv
{
	struct Joint
	{
		glm::mat4 localTransform, globalTransform;
		Joint* parent;
		Joint** children;
		unsigned int numChildren, parentIndex = -1;
	};

	struct Skeleton
	{
		Joint* joints;
		unsigned int nodeCount;
	};

	void SolveFK(Joint* joint);
	void SolveFK(Skeleton hierarchy);
}