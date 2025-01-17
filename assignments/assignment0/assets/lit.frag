#version 450

// the color of this fragment
out vec4 FragColor;

// interpolated fragment
in vec3 Normal;

void main()
{
	// shade with 0-1 Normal
	FragColor = vec4(Normal * 0.5 + 0.5, 1.0);

}
