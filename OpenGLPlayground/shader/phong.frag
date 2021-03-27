#version 330 core
out vec4 color;
  
in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 SurfaceNormal;
    vec3 ObjectColor;
    mat3 TBN;
} fs_in;

uniform vec3 directionLightDir;
uniform vec3 directionAmbientColor;
uniform vec3 directionDiffuseColor;
uniform vec3 directionSpecularColor;
uniform float directionAmbientStrength;
uniform vec3 pointLightPos;
uniform vec3 pointAmbientColor;
uniform vec3 pointDiffuseColor;
uniform vec3 pointSpecularColor;
uniform float pointAmbientStrength;
uniform int shininess;
uniform vec3 viewPos;
uniform sampler2D textureMap;
uniform sampler2D normalMap;
uniform bool useTextureMapping;
uniform bool useNormalMapping;

vec3 phong_lighting_model(vec3 lightDir, vec3 normal, vec3 viewDir, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor, float ambientStrength)
{
    // Ambient
    vec3 ambient = ambientStrength * ambientColor;
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * diffuseColor;
    // Specular shading
    vec3 halfway = normalize(lightDir + viewDir);
    vec3 spec = pow(dot(normal, halfway), shininess) * specularColor;
    return ambient + diffuse + spec;
}

void main()
{
    vec3 normal;
    if (useNormalMapping) //in tangent space
    {
        normal = texture(normalMap, fs_in.TexCoords).rgb;
        // Transform normal vector to range [-1,1]
        normal = normalize(normal * 2.0 - 1.0);
        normal = normalize(fs_in.TBN * normal);
    }
    else
    {
        normal = normalize(fs_in.SurfaceNormal);
    }
    vec3 dirLightDir = normalize(-directionLightDir);
    vec3 pointLightDir = normalize(pointLightPos - fs_in.FragPos);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    // Direction Light
    vec3 result = phong_lighting_model(dirLightDir, normal, viewDir, directionAmbientColor, directionDiffuseColor, directionSpecularColor, directionAmbientStrength);
    // Point Light
    result += phong_lighting_model(pointLightDir, normal, viewDir, pointAmbientColor, pointDiffuseColor, pointSpecularColor, pointAmbientStrength);

    if (useTextureMapping)
    {
        vec4 texColor = texture(textureMap, fs_in.TexCoords);
        color = vec4(result * fs_in.ObjectColor * texColor.xyz, texColor.w);
    }
    else
    {
        color = vec4(result * fs_in.ObjectColor, 1.0f);
    }
}
