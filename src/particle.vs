#version 330 core

layout(location = 0) in vec4 vertex;

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 projection;
uniform mat4 view;
uniform vec3 offset;
uniform vec4 color;

void main()
{
    float scale = 1.0f;
    TexCoords = vertex.zw;
    ParticleColor = color;

    // Combine the translation (offset) with the view-projection matrix
    gl_Position = projection * view * vec4((vertex.xyz + offset) * scale, 1.0);
}