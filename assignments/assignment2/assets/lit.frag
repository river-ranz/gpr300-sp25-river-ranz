#version 450

// the color of this fragment
out vec4 FragColor;

// interpolated fragment
in Surface
{
	vec2 TexCoord;
	vec3 TangentLightPos;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
	vec4 FragPosLightSpace;
	vec3 FragPos;
}fs_in;

// 2D texture sampler
uniform sampler2D _MainTex;
uniform sampler2D normalMap;
uniform sampler2D shadowMap;

// white light
uniform vec3 _LightColor = vec3(1.0);

uniform vec3 _AmbientColor = vec3(0.3, 0.4, 0.46);

uniform float _Bias;

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

float ShadowCalculation(vec4 fragPosLightSpace, float bias)
{
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// transform to 0-1
	projCoords = projCoords * 0.5 + 0.5;

	if (projCoords.z > 1.0)
    {
		return 0.0;
	}

	// get closest depth value
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	// get depth of current frag
	float currentDepth = projCoords.z;

	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 9.0;

	return shadow;
}

void main()
{
	vec3 normal = texture(normalMap, fs_in.TexCoord).rgb;
	normal = normalize(normal * 2.0 - 1.0);

	// transforms from [-1,1] to [0,1]
	vec3 rgb_normal = normal * 0.5 + 0.5;

	// light pointing straight down
	vec3 toLight = -fs_in.TangentLightPos;
	float diffuseFactor = max(dot(rgb_normal, toLight), 0.0);

	// direction toward eye
	vec3 toEye = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);

	// blinn-phong uses half angle
	vec3 h = normalize(toLight + toEye);
	float specularFactor = pow(max(dot(normal, h), 0.0), _Material.Shininess);

	vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.FragPos);

	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), _Bias);  

	// shadow
	float shadow = ShadowCalculation(fs_in.FragPosLightSpace, bias);

	vec3 lightColor = (((_AmbientColor * _Material.Ka) + (1.0 - shadow)) * (_Material.Kd * diffuseFactor + _Material.Ks * specularFactor)) * _LightColor;

	vec3 objectColor = texture(_MainTex, fs_in.TexCoord).rgb;
	FragColor = vec4(objectColor * lightColor, 1.0);
}
