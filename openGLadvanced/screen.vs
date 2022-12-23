

#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    mat2 reflect; //reflect across y axis like a mirror
    reflect[0] = vec2(-1, 0);
    reflect[1] = vec2(0, 1);
    TexCoords = reflect * aTexCoords;

    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
}  

