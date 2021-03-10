#version 330 core
out vec4 color;
  
flat in vec3 Normal;
in vec3 FragPos;
in vec3 ObjectColor;

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
    vec3 normal = normalize(Normal);
    vec3 dirLightDir = normalize(-directionLightDir);
    vec3 pointLightDir = normalize(pointLightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    // Direction Light
    vec3 result = phong_lighting_model(dirLightDir, normal, viewDir, directionAmbientColor, directionDiffuseColor, directionSpecularColor, directionAmbientStrength);
    // Point Light
    result += phong_lighting_model(pointLightDir, normal, viewDir, pointAmbientColor, pointDiffuseColor, pointSpecularColor, pointAmbientStrength);
    color = vec4(result * ObjectColor, 1.0f);
}
