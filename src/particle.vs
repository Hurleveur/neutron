#version 330 core

layout(location = 0) in vec3 vertex;

uniform mat4 viewProjection;
uniform vec3 offset;
uniform float scale;

void main()
{
    gl_Position = viewProjection * vec4((vertex.xyz*scale) + offset, 1.0);
}