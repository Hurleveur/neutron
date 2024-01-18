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

uniform vec3 viewPos; // this is the camera position technically, so TODO rename that
uniform Material material;
uniform Light light; // TODO: no need for this technically

void main()
{
    vec3 tangent_space_normal = normalize(texture2D(material.normal, TexCoords).rgb * 2.0 - 1.0);
    vec3 lightDir = normalize(light.position - FragPos);
    vec3 tangent_space_light_direction = TBN * lightDir;

    // fake Oren-Nayar diffuse attenuation
    float light_intensity = pow(dot(tangent_space_normal, tangent_space_light_direction), 0.8);
    vec3 diffuse = light_intensity * texture(material.diffuse, TexCoords).rgb;

    // specular
    vec3 viewDir = TBN * normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-tangent_space_light_direction, tangent_space_normal);
    // use default shininess
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 4.0);
    vec3 specular = spec * texture(material.specular, TexCoords).rgb;

//    FragColor = vec4((ambient + diffuse + specular), 1.0);
    FragColor = vec4(diffuse + specular, 1.0);
}
