#version 330 core
in vec3 ourColor;
in vec2 TexCoord;
out vec4 color;
uniform sampler2D ourTexture;
uniform float time;
void main()
{
    color = texture(ourTexture, vec2(TexCoord.x, TexCoord.y));//+sin(TexCoord.x*8+time)/8));
}