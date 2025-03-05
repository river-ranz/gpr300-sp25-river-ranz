#include "rivMath.h"

namespace riv
{
	vec3 lerp(vec3 a, vec3 b, float t)
	{
		t = clamp(t, 0.0f, 1.0f);
		if (t == 0.0f) { return a; }
		if (t == 1.0f) { return b; }

		vec3 lerped;
		lerped.x = (a.x * (1.0f - t)) + (b.x * t);
		lerped.y = (a.y * (1.0f - t)) + (b.y * t);
		lerped.z = (a.z * (1.0f - t)) + (b.z * t);

		return lerped;
	}

	float invLerp(float a, float b, float x)
	{
		return (x - a) / (b - a);
	}

	float clamp(float val, float min, float max)
	{
		if (val < min) { val = min; }
		if (val > max) { val = max; }
		return val;
	}

	float dot(vec3 a, vec3 b)
	{
		return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
	}

	float dot(vec3 a, float b)
	{
		return (a.x * b) + (a.y * b) + (a.z * b);
	}

	vec3 cross(vec3 a, vec3 b)
	{
		vec3 crossed;
		crossed.x = (a.y * b.z) - (a.z * b.y);
		crossed.y = (a.z * b.x) - (a.x * b.z);
		crossed.z = (a.x * b.y) - (a.y * b.x);

		return crossed;
	}

	float easeInSine(float x)
	{
		return 1 - std::cos((x * 3.14159265) / 2);
	}
}