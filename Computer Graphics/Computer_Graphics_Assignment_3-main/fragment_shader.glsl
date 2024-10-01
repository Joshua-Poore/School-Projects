#version 430 core

out vec4 fColor;

uniform vec4 u_color;

void main()
{
    fColor = u_color;
}
