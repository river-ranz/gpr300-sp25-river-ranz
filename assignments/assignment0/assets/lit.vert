#version 450

// vertex attributes

// vertex position in model space
layout(location = 0)  in vec3 vPos;
layout(location = 1) in vec3 vNormal;

// model -> world matrix
uniform mat4 _Model;

// combined view -> projection matrix
uniform mat4 _ViewProjection;

// output to next shader
out vec3 Normal;

void main()
{
	Normal = vNormal;

	// transform vertex position to homogeneous clip space
	gl_Position = _ViewProjection * _Model * vec4(vPos, 1.0);

}
