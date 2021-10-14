#version 330 core
#define NR_POINT_LIGHTS 10  
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 

struct Light {
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    float cutOff;
    bool isDirectional;
};

in vec3 FragPos;  
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 viewPos;
uniform Material material;
uniform Light lights[NR_POINT_LIGHTS];
uniform int lightNumber;
uniform sampler2D texture_diffuse1;

vec3 CalcDirLight(Light light, vec3 normal) {
    vec3 lightDir = normalize(light.position - FragPos);
    // check if lighting is inside the spotlight cone
    float theta = dot(lightDir, normalize(-light.direction)); 
    
    if(theta > light.cutOff) {    
        // ambient
        vec3 ambient = light.ambient * texture(texture_diffuse1, TexCoord).rgb;
        
        // diffuse 
        vec3 norm = normalize(Normal);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff * texture(texture_diffuse1, TexCoord).rgb;  
        
        // specular
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = light.specular * spec * texture(texture_diffuse1, TexCoord).rgb;  
        
        // attenuation
        float distance = length(light.position - FragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

        //ambient  *= attenuation; // remove attenuation from ambient, as otherwise at large distances the light would be darker inside than outside the spotlight due the ambient term in the else branche
        diffuse   *= attenuation;
        specular *= attenuation;   
            
        return (ambient + diffuse + specular);
    }
    else 
    {
        // else, use ambient light so scene isn't completely dark outside the spotlight.
        return vec3(0.0f);
    }
}  

vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos) {

    // ambiant
    vec3 ambient = light.ambient * material.ambient;

    // diffuse 
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);  

    // attenuation
    float distance = 1.0f; //length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation; 

    return (ambient + diffuse + specular);
} 

void main() {
    // ambient
    vec3 result = vec3(0.0f, 0.0f, 0.0f);

    for(int i = 0; i < lightNumber; i++) {
        if (lights[i].isDirectional) {
            result += CalcDirLight(lights[i], Normal);
        } else {
            result += CalcPointLight(lights[i], Normal, FragPos);
        }
    }

	FragColor = texture(texture_diffuse1, TexCoord) * vec4(result, 1.0f);
}