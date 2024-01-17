#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;

out vec3 FragPos;
out vec2 TexCoords;
out mat3 TBN;
out vec3 TANGENT;

uniform mat4 model;
uniform mat4 viewProjection;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = viewProjection * vec4(FragPos, 1.0);
    TexCoords = aTexCoords;
    
    mat3 normalMatrix = mat3(model);
    vec3 N = normalize(normalMatrix * aNormal);
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 B = cross(T, N);

    TBN = transpose(mat3(T, B, N));
}
