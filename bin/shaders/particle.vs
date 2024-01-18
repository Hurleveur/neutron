#version 330 core

layout(location = 0) in vec3 vertex;

out vec2 texCoords;

uniform mat4 view_projection_matrix;
uniform vec3 offset;
uniform float scale;

void main()
{
    texCoords = vertex.xy;
    gl_Position = view_projection_matrix * vec4((vertex.xyz*scale) + offset, 1.0);
}
