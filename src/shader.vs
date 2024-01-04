#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec3 FragPos;
out vec2 TexCoords;
out mat3 TBN;

uniform mat4 model;
uniform mat4 viewProjection;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    mat3 normalMatrix = mat3(model);

    FragPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = viewProjection * vec4(FragPos, 1.0);
    TexCoords = aTexCoords;

    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    vec3 B = cross(N, T);

    TBN = mat3(T, B, N);
}