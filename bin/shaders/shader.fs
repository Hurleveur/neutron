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

void main()
{
    vec3 tangent_space_normal = normalize(texture2D(material.normal, TexCoords).rgb * 2.0 - 1.0);
//    normal.y = -normal.y;
    vec3 tangent_space_light_direction = TBN * normalize(light.position - FragPos);

    float diffuse = dot(tangent_space_normal, tangent_space_light_direction);
//    vec3 diffuse = texture(material.diffuse, TexCoords).rgb;
//    diffuse *= light.diffuse * diff;

    // specular
//    vec3 viewDir = normalize(viewPos - FragPos);
//    vec3 reflectDir = reflect(-lightDir, normal);
    // use default shininess
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 4.0);
//    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;

//    FragColor = vec4((ambient + diffuse + specular), 1.0);
    FragColor = vec4(diffuse, diffuse, diffuse, 1.0);
}
