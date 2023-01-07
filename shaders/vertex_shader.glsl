#version 330 core
layout (location = 0) in vec3 a_position;

out vec4 v_color;

uniform vec4 tri_color;
uniform mat4 mvMatrix;
uniform mat4 projMatrix;

void main( )
{
    v_color = vec4(tri_color);
    gl_Position = projMatrix * mvMatrix * vec4(a_position, 1.0);
}


