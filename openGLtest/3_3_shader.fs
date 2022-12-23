#version 330 core
out vec4 FragColor;

//in vec3 ourColor;
//in vec3 ourPosition;
in vec2 TexCoords;
in vec3 Normal; //passed from vertex shader
in vec3 FragPos;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float mixValue;

//uniform vec3 lightColor; // Replaced with Light struct
//uniform vec3 lightPos;
//uniform vec3 objectColor; // Replaced with Material struct
uniform vec3 viewPos; //Camera position in world. Not needed if calculating in view space, since cam pos is always (0,0,0) in view space

//in vec3 LightPos; //from vs, in view space coords

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D emission;
    float shininess;
}; 
uniform Material material;

//uniform Light light;

struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
uniform DirLight dirLight;
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);

struct PointLight {    
    vec3 position;
    
    float constant; //attenuation terms
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
#define NR_POINT_LIGHTS 4  
uniform PointLight pointLights[NR_POINT_LIGHTS];
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cosCutoff; //spotlight inner cone
    float cosOuterCutoff; //spotlight's outer cone

    vec3 ambient; //usually low, vec3(0.1), equivalent to vec3(0.1, 0.1, 0.1)
    vec3 diffuse; // the exact color we'd like the light to have
    vec3 specular; //usually vec3(1.0), or same as diffuse?

    // Attenuation terms
    float constant;
    float linear;
    float quadratic;
};
uniform SpotLight spotLight;
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    //FragColor = vec4(ourPosition, 1.0);    // note how the position value is linearly interpolated to get all the different colors
    //FragColor = texture(ourTexture, TexCoord); //texture function returns color, given the sampler and coord.
    //FragColor = texture(texture1, TexCoord) * vec4(ourColor, 1.0); //DISCO!!! using "ourColor" uniform
    //FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), mixValue); //Mix two textures!
    //FragColor = mix(texture(texture1, TexCoord), texture(texture2, vec2(1.0 - TexCoord.x, TexCoord.y)), 0.2); //Horizonally flip only 1 texture.
    //FragColor = texture(texture1, TexCoord) * vec4(lightColor, 1.0); //shine light on texture.

 


    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos); //frag-to-camera

    vec3 result;
    // phase 1: Directional lighting
    result = CalcDirLight(dirLight, norm, viewDir);
    // phase 2: Point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    
    // phase 3: Spot light
    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    
    
        // emission
    //vec3 emission = texture(material.emission, TexCoords).rgb;

    FragColor = vec4(result, 1.0);

}

// Global "sunlight" shining on everything in a direction
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction); //global "sunlight" direction, flipped
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));
    return (ambient + diffuse + specular);
}  

// Point light in all directions with attenuation
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos); //frag-to-light
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
} 

// point light with cone
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos); //frag-to-light
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    


    // light cone with soft edges
    float cosTheta = dot(lightDir, normalize(-light.direction)); //relative fragment position in cone. larger if fragment is closer to spotlight center
    float softWidth = (light.cosCutoff - light.cosOuterCutoff); //larger the greater distance between the cones (width of the soft edges)
    float intensity = clamp((cosTheta - light.cosOuterCutoff) / softWidth, 0.0, 1.0);
    
        // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}