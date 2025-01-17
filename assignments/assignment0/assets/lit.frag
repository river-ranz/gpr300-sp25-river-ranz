#version 450

// the color of this fragment
out vec4 FragColor;

// interpolated fragment
in Surface
{
	// vertex position in world space
	vec3 WorldPos;
	// vertex normal in world space
	vec3 WorldNormal;
	vec2 TexCoord;
}fs_in;

// 2D texture sampler
uniform sampler2D _MainTex;

uniform vec3 _EyePos;

// light pointing straight down
uniform vec3 _LightDirection = vec3(0.0, -1.0, 0.0);

// white light
uniform vec3 _LightColor = vec3(1.0);

uniform vec3 _AmbientColor = vec3(0.3, 0.4, 0.46);

struct Material
{
	// ambient coefficient (0-1)
	float Ka;
	// diffuse coefficient (0-1)
	float Kd;
	// specular coefficient (0-1)
	float Ks;
	// affects size of specular highlight
	float Shininess;
};

uniform Material _Material;

void main()
{
	// make sure fragment normal is still length 1 after interpolation
	vec3 normal = normalize(fs_in.WorldNormal);
	// light pointing straight down
	vec3 toLight = -_LightDirection;
	float diffuseFactor = max(dot(normal, toLight), 0.0);

	// direction toward eye
	vec3 toEye = normalize(_EyePos - fs_in.WorldPos);

	// blinn-phong uses half angle
	vec3 h = normalize(toLight + toEye);
	float specularFactor = pow(max(dot(normal, h), 0.0), _Material.Shininess);

	// combination of specular and diffuse reflection
	vec3 lightColor = (_Material.Kd * diffuseFactor + _Material.Ks * specularFactor) * _LightColor;

	// add ambient light
	lightColor += _AmbientColor * _Material.Ka;

	vec3 objectColor = texture(_MainTex, fs_in.TexCoord).rgb;
	FragColor = vec4(objectColor * lightColor, 1.0);
}