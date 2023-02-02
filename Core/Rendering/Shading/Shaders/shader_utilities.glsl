#version 450

#define DIFFUSE_TEXTURE_OFFSET 0
#define SPECULAR_TEXTURE_OFFSET 1
#define HEIGHT_MAP_TEXTURE_OFFSET 2
#define TOTAL_TEXTURE_TYPES_COUNT 3

#define MAX_MESHES 8192
#define MAX_TEXTURES MAX_MESHES * TOTAL_TEXTURE_TYPES_COUNT
#define MAX_POINT_LIGHTS 64
#define MAX_DIRECTIONAL_LIGHTS 16

/* --- STRUCTS --- */

struct Material
{
    vec3 diffuse;
    float shininess;

    vec3 specular;
    float vertexExaggeration;

    vec3 ambient;
};

struct ObjectData
{
    mat4 model;
};

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

/* --- FIELDS --- */

layout(set = 0, binding = 0) uniform UniformBuffer
{
    mat4 view;
    mat4 projection;

    uint directionalLightCount;
    uint pointLightCount;
} uniformBuffer;

layout(std140, set = 0, binding = 1) readonly buffer StorageBuffer
{
    ObjectData objectDatas[MAX_MESHES];
    DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
    PointLight pointLights[MAX_POINT_LIGHTS];
} storageBuffer;

/* --- METHODS --- */

bool IsBitSet(uint binaryValue, uint bitIndex)
{
    return (binaryValue & (1 << bitIndex)) > 0;
}

vec3 CalculateDirectionalLight(DirectionalLight directionalLight, vec3 fragmentPosition, vec3 normal, vec3 viewDirection, vec3 textureColor, float specularColor, Material material)
{
    // Calculate required directions
    const vec3 halfwayDirection = normalize(directionalLight.direction + normalize(viewDirection - fragmentPosition));

    // Calculate diffuse and base specular values
    const float diffuseStrength = max(dot(normal, directionalLight.direction), 0.0);
    const float specularStrength = pow(max(dot(normal, halfwayDirection), 0.0), max(material.shininess * 512.f, 1.0f));

    // Calculate light components
    vec3 ambient = material.ambient * textureColor;
    vec3 diffuse = diffuseStrength * directionalLight.intensity * directionalLight.color * material.diffuse * textureColor;
    vec3 specular = specularStrength * directionalLight.color * material.specular * specularColor;

    return ambient + diffuse + specular;
}

vec3 CalculatePointLight(PointLight pointLight, vec3 fragmentPosition, vec3 normal, vec3 viewDirection, vec3 textureColor, float specularColor, Material material)
{
    // Calculate required directions
    const vec3 lightDirection = normalize(pointLight.position - fragmentPosition);
    const vec3 halfwayDirection = normalize(lightDirection + normalize(viewDirection - fragmentPosition));

    // Calculate diffuse and base specular values
    const float diffuseStrength = max(dot(normal, lightDirection), 0.0);
    const float specularStrength = pow(max(dot(normal, halfwayDirection), 0.0), max(material.shininess * 512.f, 1.0f));

    // Calculate light components
    vec3 ambient = material.ambient * textureColor;
    vec3 diffuse = diffuseStrength * pointLight.intensity * pointLight.color * material.diffuse * textureColor;
    vec3 specular =specularStrength * pointLight.color * material.specular * specularColor;

    // Calculate attenuation
    const float distance = length(pointLight.position - fragmentPosition);
    const float attenuation = 1.0 / (1.0f + pointLight.linear * distance + pointLight.quadratic * (distance * distance));

    // Apply attenuation
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}