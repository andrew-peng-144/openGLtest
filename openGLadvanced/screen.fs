

#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;


const float offset = 1.0 / 300.0; //represents how far to sample the adjacent pixels, in normalized device coords [0,1]
void main()
{

    //offset applied to each of the 8 adjacent pixels
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );
    // sharpen
    float kernel[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );
    // blur
    //float kernel[9] = float[](
    //     1.0 / 16, 2.0 / 16, 1.0 / 16,
    //     2.0 / 16, 4.0 / 16, 2.0 / 16,
    //     1.0 / 16, 2.0 / 16, 1.0 / 16 
    //);
     //edge detection
    // float kernel[9] = float[](
    //     1,1,1,1,-8,1,1,1,1
    // );

    //identity
    //float kernel[9] = float[](
    //    0,0,0,0,1,0,0,0,0
    //);
    
    vec3 sampleTex[9]; //sample the 8 adjacent(+offset) pixels plus the pixel itself
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    }
    vec3 color = vec3(0.0);
    for(int i = 0; i < 9; i++)
        color += sampleTex[i] * kernel[i];
    
    //vec3 yescolor = texture(screenTexture, TexCoords).rgb;
    FragColor = vec4(color, 1.0);

}

