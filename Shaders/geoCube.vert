#version 330 core
layout (location = 0) in vec3 aPos;
in vec3 inColor;


uniform mat4 model;
uniform mat4 view;
uniform mat4 perspective;

out VS_OUT{
    vec3 color;
}vs_out;

void main(){
    vs_out.color = inColor;
    gl_Position = perspective * view * model * vec4(aPos, 1.0);
}