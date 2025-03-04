#pragma once
#include <cmath>
#include <glm/glm.hpp>

namespace riv
{
	class vec2
	{
	public:
		float x = 0.0f, y = 0.0f;
	};

	class vec3
	{
	public:
		float x = 0.0f, y = 0.0f, z = 0.0f;
		float magnitude(vec3 vec)
		{
			float sum = (vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z);
			return sqrt(sum);
		}
	};

	class vec4
	{
	public:
		float x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f;
	};

	vec3 lerp(vec3 a, vec3 b, float t);
	vec3 invLerp(vec3 a, vec3 b, float x);

	float clamp(float val, float min, float max);

	float dot(vec3 a, vec3 b);
	float dot(vec3 a, float b);
	vec3 cross(vec3 a, vec3 b);
}