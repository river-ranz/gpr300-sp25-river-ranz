#version 450

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;

uniform vec3 colorAvg;
uniform int grayscale;
uniform int blur;

const float offset = 1.0 / 300.0; 

// for blur
float kernel[9] = float[](
    1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16  
);

void main()
{
    if (blur != 0)
    {
	    vec2 offsets[9] = vec2[](
            vec2(-offset,  offset), // top left
            vec2( 0.0f,    offset), // top center
            vec2( offset,  offset), // top right
            vec2(-offset,  0.0f),   // center left
            vec2( 0.0f,    0.0f),   // center center
            vec2( offset,  0.0f),   // center right
            vec2(-offset, -offset), // bottom left
            vec2( 0.0f,   -offset), // bottom center
            vec2( offset, -offset)  // bottom right    
        );

        vec3 sampleTex[9];
        for(int i = 0; i < 9; i++)
        {
            sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
        }
        vec3 col = vec3(0.0);
        for(int i = 0; i < 9; i++)
            col += sampleTex[i] * kernel[i];

        FragColor = vec4(col, 1.0);
    }
    else { FragColor = texture(screenTexture, TexCoords); }

	if (grayscale != 0)
	{
		float average = colorAvg.r * FragColor.r + colorAvg.g * FragColor.g + colorAvg.b * FragColor.b;
		FragColor = vec4(average, average, average, 1.0);
	}
}