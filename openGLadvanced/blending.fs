

#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{   
    //FragColor = vec4(vec3(gl_FragCoord.z), 1.0); //depth buffer visualized
    
    vec4 texColor = texture(texture1, TexCoords); //keep all 4 color components
    //if(texColor.a < 0.1)
    //    discard; //disables early depth testing?
    //FragColor = vec4(vec3(texture(texture1, TexCoords)) + vec3(0, 0, 0.4), 1.0);
    //FragColor = vec4(vec3(1.0 - texColor), 1.0); //inversion
    FragColor = texColor;
}

