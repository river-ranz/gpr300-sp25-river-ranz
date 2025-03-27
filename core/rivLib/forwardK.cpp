#include "forwardK.h"

namespace riv
{
	void riv::SolveFK(Joint* joint)
	{
		if (joint->parent == nullptr)
		{
			joint->globalTransform = joint->localTransform;
		}
		else
		{
			joint->globalTransform = joint->parent->globalTransform * joint->localTransform;
			for (int i = 0; i <= joint->numChildren; i++)
			{
				SolveFK(joint->children[i]);
			}
		}
	}

	void riv::SolveFK(Skeleton hierarchy)
	{
		for (int i = 0; i < hierarchy.nodeCount; i++)
		{
			if (hierarchy.joints[i]->parentIndex == -1) { hierarchy.joints[i]->globalTransform = hierarchy.joints[i]->localTransform; }
			else
			{
				hierarchy.joints[i]->globalTransform = hierarchy.joints[hierarchy.joints[i]->parentIndex]->globalTransform * hierarchy.joints[i]->localTransform;
			}
		}
	}
}