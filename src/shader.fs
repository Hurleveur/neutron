#version 330 core
in vec2 TexCoords;
in vec3 FragPos;
in mat3 TBN;

out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D normal;
    sampler2D specular;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main() {
    vec3 normal = normalize(texture2D(material.normal,TexCoords).rgb * 2.0 - 1.0);
    normal = normalize(TBN * normal);
    vec3 lightDir = normalize(light.position - FragPos);

    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    // use default shininess
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;

    FragColor = vec4((ambient + diffuse + specular), 1.0);
}
