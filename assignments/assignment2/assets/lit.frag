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
}fs_in;

// 2D texture sampler
uniform sampler2D _MainTex;
uniform sampler2D normalMap;
uniform sampler2D shadowMap;

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

float ShadowCalculation(vec4 fragPosLightSpace)
{
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// transform to 0-1
	projCoords = projCoords * 0.5 + 0.5;
	// get closest depth value
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	// get depth of current frag
	float currentDepth = projCoords.z;
	// is frag in shadow?
	float shadow = currentDepth > closestDepth ? 1.0 : 0.0;

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

	// shadow
	float shadow = ShadowCalculation(fs_in.FragPosLightSpace);

	// combination of specular and diffuse reflection
	//vec3 lightColor = (_Material.Kd * diffuseFactor + _Material.Ks * specularFactor) * _LightColor;

	// add ambient light
	//lightColor += _AmbientColor * _Material.Ka;

	vec3 lightColor = (((_AmbientColor * _Material.Ka) + (1.0 - shadow)) * (_Material.Kd * diffuseFactor + _Material.Ks * specularFactor)) * _LightColor;

	vec3 objectColor = texture(_MainTex, fs_in.TexCoord).rgb;
	FragColor = vec4(objectColor * lightColor, 1.0);

//	vec3 color = texture(_MainTex, fs_in.TexCoord).rgb;
//    // ambient
//    vec3 ambient = 0.15 * lightColor;
//    // diffuse
//    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.FragPos);
//    float diff = max(dot(lightDir, normal), 0.0);
//    vec3 diffuse = diff * lightColor;
//    // specular
//    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.FragPos);
//    float spec = 0.0;
//    vec3 halfwayDir = normalize(lightDir + viewDir);  
//    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
//    vec3 specular = spec * lightColor;    
//    // calculate shadow
//    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);       
//    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
//    
//    FragColor = vec4(lighting, 1.0);
}
