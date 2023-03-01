#ifndef DEFAULT_LIGHTING
#define DEFAULT_LIGHTING

#include "../Utility/ShaderDefinitions.glsl"

struct DirectionalLight
{
    vec3 direction;
    float intensity;

    vec3 color;
};

struct PointLight
{
    vec3 color;
    float intensity;

    vec3 position;
    float linear;

    float quadratic;
};

vec3 CalculateDirectionalLight(DirectionalLight directionalLight, vec3 fragmentPosition, vec3 diffuseColor, float specularColor, float shininess, vec3 normalColor)
{
    // Calculate required directions
    const vec3 viewDirection = normalize(-fragmentPosition);
    const vec3 halfwayDirection = normalize(directionalLight.direction + normalize(viewDirection - fragmentPosition));

    // Calculate diffuse and base specular values
    const float diffuseStrength = max(dot(normalColor, directionalLight.direction), 0.0);
    const float specularStrength = pow(max(dot(normalColor, halfwayDirection), 0.0), clamp(shininess * 512.0f, 1.0f, 512.0f));

    // Calculate light components
    const vec3 ambient = AMBIENT_STRENGTH * diffuseColor;
    const vec3 diffuse = diffuseStrength * directionalLight.intensity * directionalLight.color * diffuseColor;
    const vec3 specular = specularStrength * directionalLight.color * specularColor;

    return ambient + diffuse + specular;
}

vec3 CalculatePointLight(PointLight pointLight, vec3 fragmentPosition, vec3 diffuseColor, float specularColor, float shininess, vec3 normalColor)
{
    // Calculate required directions
    const vec3 viewDirection = normalize(-fragmentPosition);
    const vec3 lightDirection = normalize(pointLight.position - fragmentPosition);
    const vec3 halfwayDirection = normalize(lightDirection + normalize(viewDirection - fragmentPosition));

    // Calculate diffuse and base specular values
    const float diffuseStrength = max(dot(normalColor, lightDirection), 0.0);
    const float specularStrength = pow(max(dot(normalColor, halfwayDirection), 0.0), clamp(shininess * 512.0f, 1.0f, 512.0f));

    // Calculate attenuation
    const float distance = length(pointLight.position - fragmentPosition);
    const float attenuation = 1.0 / (1.0f + pointLight.linear * distance + pointLight.quadratic * (distance * distance));

    // Calculate light components
    const vec3 ambient = AMBIENT_STRENGTH * diffuseColor * attenuation;
    const vec3 diffuse = diffuseStrength * pointLight.intensity * pointLight.color * diffuseColor * attenuation;
    const vec3 specular = specularStrength * pointLight.color * specularColor                       * attenuation;

    return ambient + diffuse + specular;
}

#endif