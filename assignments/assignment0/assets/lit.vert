#version 450

// vertex attributes

// vertex position in model space
layout(location = 0)  in vec3 vPos;
layout(location = 1) in vec3 vNormal;

// vertex texture coordinate (UV)
layout(location = 2) in vec2 vTexCoord;

// model -> world matrix
uniform mat4 _Model;

// combined view -> projection matrix
uniform mat4 _ViewProjection;

out Surface
{
	// output to next shader
	vec3 WorldPos;
	vec3 WorldNormal;
	vec2 TexCoord;
}vs_out;

void main()
{
	// transform vertex position to world space
	vs_out.WorldPos = vec3(_Model * vec4(vPos, 1.0));

	// transform vertex normal to world space using normal matrix
	vs_out.WorldNormal = transpose(inverse(mat3(_Model))) * vNormal;
	vs_out.TexCoord = vTexCoord;

	// transform vertex position to homogeneous clip space
	gl_Position = _ViewProjection * _Model * vec4(vPos, 1.0);

}
