#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float scale;

void main()
{
    gl_Position = projection * view * model * vec4(aPos*scale, 1.0);
    TexCoords = aPos;  // Use position as texture coordinates (modify this based on your requirements)
}