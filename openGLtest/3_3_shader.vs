#version 330 core
layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aColor;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

//out vec3 ourColor;
// out vec3 ourPosition; //if you want fragment shader to read position.
out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos; //For the fragment shader to calculate the direction of light ray

//uniform mat4 transform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

//uniform vec3 lightPos; // View space calculation: we now define the uniform in the vertex shader and pass the 'view space' lightpos to the fragment shader. lightPos is passed in as world space coord.
//out vec3 LightPos;

void main()
{
    //gl_Position = vec4(aPos, 1.0); 
    //gl_Position = transform * vec4(aPos, 1.0f);
    gl_Position = projection * view * model * vec4(aPos, 1.0); //If w is still 1 then the persepctive division that OpenGL automatically does won't do anything
    
    
    //ourColor = aColor;
    // ourPosition = aPos;

    TexCoords = aTexCoords;

    //Normal = aNormal; //Transfer normal vector data to fragment shader
    Normal = mat3(transpose(inverse(model))) * aNormal; //Using normal matrix to fix normal vectors of a triangle transformed non-uniformly.
    FragPos = vec3(model * vec4(aPos, 1.0)); //apply model matrix to vertex position to get frag pos in world space.
    //LightPos = vec3(view * vec4(lightPos, 1.0)); //view-space light position
}