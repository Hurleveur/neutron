#version 330 core
in vec3 FragPos;
in vec3 TexCoords;
in vec2 TexCoords2;
in vec3 TangentLightPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;

out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D normalMap;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 Normal;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main() {
    vec3 normal = texture2D(material.normalMap,TexCoords2).xyz * 2.0 - 1.0;
    vec3 norm = normalize(normal);
    // ambient
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords2).rgb;

    // diffuse
    vec3 norm2 = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);// * normalize(TangentLightPos - TangentFragPos);
    //vec3 lightDir = normalize(TangentLightPos - TangentFragPos);
    float diff = max(dot(norm2, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords2).rgb;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    //vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    //float spec2 = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords2).rgb;

    FragColor = vec4((ambient + diffuse + specular*10*diffuse), 1.0);
}
