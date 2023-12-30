#version 330 core
in vec3 FragPos;
in vec3 TexCoords;
in vec3 TangentLightPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;

out vec4 FragColor;

uniform sampler2D mytexture;
uniform sampler2D normalMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
    vec2 longitudeLatitude = vec2((atan(TexCoords.y, TexCoords.x) / 3.1415926 + 1.0) * 0.5,
                                  (asin(TexCoords.z) / 3.1415926 + 0.5));
    // Retrieve the normal from the normal map and transform it to the range [-1, 1]
    vec3 normal = texture2D(normalMap,longitudeLatitude).xyz * 2.0 - 1.0;
    normal = normalize(normal);

    // get diffuse color
    vec3 color = texture(mytexture, longitudeLatitude).rgb;
    // ambient
    vec3 ambient = color;
    // diffuse
    vec3 lightDir = normalize(TangentLightPos - TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    // specular
    vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 specular = vec3(0.2) * spec;


    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
