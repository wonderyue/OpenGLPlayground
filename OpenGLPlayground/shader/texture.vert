#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoord;
out vec3 ourColor;
out vec2 TexCoord;
uniform float time;
void main()
{
    gl_Position = vec4(position.x, position.y, position.z, 1.0);
    ourColor = color;
    TexCoord = texCoord;
    TexCoord.y = 1-TexCoord.y;
//    TexCoord.x += sin(time);
//    TexCoord.y += sin(time);
    //TexCoord.x = fract(TexCoord.x);
}