#version 330 core

in vec2 texCoords;
out vec4 FragColor;

uniform sampler2D sprite;
//uniform vec4 color;

void main()
{
    FragColor = texture(sprite, texCoords); // + color
}