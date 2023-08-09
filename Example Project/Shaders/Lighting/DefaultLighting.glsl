#ifndef DEFAULT_LIGHTING_GLSL
#define DEFAULT_LIGHTING_GLSL

#include "../Utility/ShaderDefinitions.glsl"

struct DirectionalLight
{
    mat4x4 projectionView;

    vec3 direction;
    float intensity;

    vec3 color;
};

struct PointLight
{
    mat4x4 projectionView;

    vec3 color;
    float intensity;

    vec3 position;
    float linear;

    float quadratic;
};

const float AMBIENT_STRENGTH = 0.05f;

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 fragmentPosition, vec3 diffuse, float specular, float shininess, vec3 normal)
{
    const vec3 lightDirection = normalize(-light.direction);
    const vec3 finalAmbient = diffuse * AMBIENT_STRENGTH;

    #if defined(SETTINGS_SHADING_MODEL_PHONG)
        const vec3 viewDirection = normalize(-fragmentPosition);

        const vec3 finalDiffuse = max(dot(normal, lightDirection), 0.0) * diffuse;
        const float finalSpecular = pow(max(dot(viewDirection, reflect(-lightDirection, normal)), 0.0), shininess) * specular;
        return light.intensity * (finalDiffuse + vec3(finalSpecular) + finalAmbient) * light.color;
    #endif

    #if defined(SETTINGS_SHADING_MODEL_BLINN_PHONG)
        const vec3 viewDirection = normalize(-fragmentPosition);
        const vec3 halfwayDirection = normalize(lightDirection + viewDirection);

        const vec3 finalDiffuse = max(dot(normal, lightDirection), 0.0) * diffuse;
        const float finalSpecular = clamp(pow(max(dot(normal, halfwayDirection), 0.0), shininess) * specular, 0.0, 1.0);
        return light.intensity * (finalDiffuse + vec3(finalSpecular) + finalAmbient) * light.color;
    #endif

    #if defined(SETTINGS_SHADING_MODEL_LAMBERTIAN)
        vec3 finalDiffuse = light.intensity * max(dot(normal, lightDirection), 0.0) * diffuse * light.color;
        return finalDiffuse;
    #endif

    #if defined(SETTINGS_SHADING_MODEL_GAUSSIAN)
        const vec3 viewDirection = normalize(-fragmentPosition);
        const vec3 halfwayDirection = normalize(lightDirection + viewDirection);

        const float finalDiffuse = max(dot(normal, lightDirection), 0.0);
        const float normalHalfway = acos(dot(halfwayDirection, normal));
        float exponent = normalHalfway / 1.0;
        exponent = -(exponent * exponent);
        return light.intensity * light.color * (finalDiffuse + exp(exponent)) * diffuse;
    #endif

    return vec3(0.0);
}

vec3 CalculatePointLight(PointLight light, vec3 fragmentPosition, vec3 diffuse, float specular, float shininess, vec3 normal)
{
    vec3 lightDirection = normalize(light.position - fragmentPosition);
    float distance = length(light.position - fragmentPosition);
    float attenuation = 1.0f / (distance * distance);

    vec3 finalAmbient = diffuse * AMBIENT_STRENGTH;

    #if defined(SETTINGS_SHADING_MODEL_PHONG)
        const vec3 viewDirection = normalize(-fragmentPosition);

        const vec3 finalDiffuse = max(dot(normal, lightDirection), 0.0) * diffuse;
        const float finalSpecular = pow(max(dot(viewDirection, reflect(-lightDirection, normal)), 0.0), shininess) * specular;
        return light.intensity * attenuation * (finalDiffuse + vec3(finalSpecular) + finalAmbient) * light.color;
    #endif

    #if defined(SETTINGS_SHADING_MODEL_BLINN_PHONG)
        const vec3 viewDirection = normalize(-fragmentPosition);
        const vec3 halfwayDirection = normalize(lightDirection + viewDirection);

        const vec3 finalDiffuse = max(dot(normal, lightDirection), 0.0) * diffuse;
        const float finalSpecular = clamp(pow(max(dot(normal, halfwayDirection), 0.0), shininess) * specular, 0.0, 1.0);
        return light.intensity * attenuation * (finalDiffuse + vec3(finalSpecular) + finalAmbient) * light.color;
    #endif

    #if defined(SETTINGS_SHADING_MODEL_LAMBERTIAN)
        const vec3 finalDiffuse = light.intensity * attenuation * max(dot(normal, lightDirection), 0.0) * diffuse * light.color;
        return finalDiffuse;
    #endif

    #if defined(SETTINGS_SHADING_MODEL_GAUSSIAN)
        const vec3 viewDirection = normalize(-fragmentPosition);
        const vec3 halfwayDirection = normalize(lightDirection + viewDirection);

        const float finalDiffuse = max(dot(normal, lightDirection), 0.0);
        const float normalHalfway = acos(dot(halfwayDirection, normal));
        float exponent = normalHalfway / 1.0;
        exponent = -(exponent * exponent);
        return light.intensity * attenuation * (finalDiffuse + exp(exponent)) * diffuse * light.color;
    #endif

    return vec3(0.0);
}

#endif