#version 330 core
layout (location = 0) in vec3 model_position;
layout (location = 1) in vec3 model_normal;
layout (location = 2) in vec3 model_tangent;
layout (location = 3) in vec2 model_texcoord;

out vec3 FragPos;
out vec2 TexCoords;
out mat3 TBN;

uniform mat4 model_to_world_matrix;
uniform mat4 view_projection_matrix;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    FragPos = vec3(model_to_world_matrix * vec4(model_position, 1.0));

    gl_Position = view_projection_matrix * vec4(FragPos, 1.0);
    TexCoords = model_texcoord;
    
    mat3 model_to_world_matrix = mat3(model_to_world_matrix);
    vec3 N = normalize(model_to_world_matrix * model_normal);
    vec3 T = normalize(model_to_world_matrix * model_tangent);
    vec3 B = cross(N, T);
    TBN = transpose(mat3(T, B, N));
}
