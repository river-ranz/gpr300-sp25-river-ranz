#version 450

// vertex attributes

// vertex position in model space
layout(location = 0)  in vec3 vPos;
layout(location = 1) in vec3 vNormal;

// vertex texture coordinate (UV)
layout(location = 2) in vec2 vTexCoord;

layout(location = 3) in vec3 vTangent;

// model -> world matrix
uniform mat4 _Model;

// combined view -> projection matrix
uniform mat4 _ViewProjection;

uniform mat4 lightSpaceMatrix;

vec3 T = normalize(vec3(_Model * vec4(vTangent, 0.0)));
vec3 N = normalize(vec3(_Model * vec4(vNormal, 0.0)));
vec3 B = cross(N, T);

uniform vec3 lightPos;
uniform vec3 viewPos;

out Surface
{
	// output to next shader
	vec2 TexCoord;
	vec3 TangentLightPos;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
	vec3 FragPos;
	vec4 FragPosLightSpace;
}vs_out;

void main()
{
	// transform vertex position to world space
	// transform vertex normal to world space using normal matrix
	vs_out.TexCoord = vTexCoord;

	vs_out.FragPos = vec3(_Model * vec4(vPos, 1.0));

	mat3 TBN = transpose(mat3(T, B, N));

	vs_out.TangentLightPos = TBN * lightPos;
	vs_out.TangentViewPos = TBN * viewPos;
	vs_out.TangentFragPos = TBN * vec3(_Model * vec4(vPos, 1.0));
	vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);

	// transform vertex position to homogeneous clip space
	gl_Position = _ViewProjection * _Model * vec4(vPos, 1.0);

}

