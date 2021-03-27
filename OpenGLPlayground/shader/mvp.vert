#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 SurfaceNormal;
    vec3 ObjectColor;
    mat3 TBN;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 color;
uniform vec3 directionLightDir;
uniform vec3 pointLightPos;
uniform vec3 viewPos;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    vs_out.FragPos = vec3(model * vec4(position, 1.0));
    vs_out.TexCoords = texCoord;
    vs_out.TexCoords.y = 1 - vs_out.TexCoords.y;
    
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(vec3(model * vec4(tangent,   0.0)));
    vec3 B = normalize(vec3(model * vec4(bitangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(normal,    0.0)));
    mat3 TBN = mat3(T, B, N);
    
    vs_out.SurfaceNormal = normalMatrix * normal;
    vs_out.ObjectColor = color;
    vs_out.TBN = TBN;
}
