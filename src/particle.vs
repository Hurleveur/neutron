#version 330 core

layout(location = 0) in vec4 vertex;

out vec2 TexCoords;
uniform mat4 projection;
uniform mat4 view;
uniform vec3 offset;
uniform float scale;

void main()
{
    TexCoords = vertex.xy;
    gl_Position = projection * view * vec4((vertex.xyz*scale) + offset, 1.0);
}