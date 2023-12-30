#version 330 core
in vec3 FragPos;
in vec3 TexCoords;
in vec3 TangentLightPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;

out vec4 FragColor;
uniform sampler2D normalMap;

struct Material {
    sampler2D diffuse;
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
    vec2 longitudeLatitude = vec2((atan(TexCoords.y, TexCoords.x) / 3.1415926 + 1.0) * 0.5,
                                  (asin(TexCoords.z) / 3.1415926 + 0.5));
    vec3 normal = texture2D(normalMap,longitudeLatitude).xyz * 2.0 - 1.0;
    vec3 norm = normalize(normal);
    // ambient
    vec3 ambient = texture(material.diffuse, longitudeLatitude).rgb;

    // diffuse
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, longitudeLatitude).rgb;

    // specular
    vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, longitudeLatitude).rgb;

    FragColor = vec4((ambient + diffuse + specular), 1.0);
}
