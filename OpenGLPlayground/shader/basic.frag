#version 330 core

in vec3 OutColor;

out vec4 color;

void main(){
    color = vec4(OutColor, 1.0f);
}
