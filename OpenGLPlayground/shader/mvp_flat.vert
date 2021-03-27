#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out vec2 TexCoord;
out vec3 ObjectColor;
flat out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 color;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    TexCoord = texCoord;
    TexCoord.y = 1 - TexCoord.y;
    ObjectColor = color;
    Normal = mat3(transpose(inverse(model))) * normal;
    FragPos = vec3(model * vec4(position, 1.0f));
}
