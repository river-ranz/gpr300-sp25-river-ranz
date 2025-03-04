#include "rivMath.h"

namespace riv
{
	vec3 lerp(vec3 a, vec3 b, float t)
	{
		t = clamp(t, 0.0f, 1.0f);
		if (t == 0.0f) { return a; }
		if (t == 1.0f) { return b; }

		vec3 lerped;
		lerped.x = (a.x + (b.x - a.x) * t);
		lerped.y = (a.y + (b.y - a.y) * t);
		lerped.z = (a.z + (b.z - a.z) * t);

		return lerped;
	}

	float clamp(float val, float min, float max)
	{
		if (val < min) { val = min; }
		if (val > max) { val = max; }
		return val;
	}

	float dot(vec3 a, vec3 b)
	{
		float dotted = (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
		return dotted;
	}

	float dot(vec3 a, float b)
	{
		float dotted = (a.x * b) + (a.y * b) + (a.z * b);
		return dotted;
	}

	vec3 cross(vec3 a, vec3 b)
	{
		vec3 crossed;
		crossed.x = (a.y * b.z) - (a.z * b.y);
		crossed.y = (a.z * b.x) - (a.x * b.z);
		crossed.z = (a.x * b.y) - (a.y * b.x);

		return crossed;
	}
}