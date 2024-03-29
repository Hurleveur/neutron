#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 view_projection_matrix;

void main()
{
    TexCoords = aPos;
    vec4 pos = view_projection_matrix * vec4(aPos, 0.0);
    gl_Position = pos.xyww;
}
