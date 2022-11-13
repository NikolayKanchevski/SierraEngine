#version 450

#define TEXTURE_TYPE_DIFFUSE_OFFSET 0
#define TEXTURE_TYPE_SPECULAR_OFFSET 1
#define TOTAL_TEXTURE_TYPES_COUNT 2

#define MAX_OBJECTS 256
#define MAX_TEXTURES MAX_OBJECTS * TOTAL_TEXTURE_TYPES_COUNT
#define MAX_POINT_LIGHTS 64
#define MAX_DIRECTIONAL_LIGHTS 16

/* --- STRUCTS --- */

struct Material
{
    vec3 diffuse;
    float shininess;

    vec3 specular;
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
    vec3 position;
    float linear;

    vec3 color;
    float intensity;

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
    ObjectData objectDatas[MAX_OBJECTS];
    DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
    PointLight pointLights[MAX_POINT_LIGHTS];
} storageBuffer;

layout(push_constant) uniform PushConstant
{
    Material material;

    uint meshID;
    uint meshTexturesPresence;
} pushConstant;

/* --- METHODS --- */

bool IsBitSet(uint binaryValue, uint bitIndex)
{
    return (binaryValue & (1 << bitIndex)) > 0;
}

vec3 CalculateDirectionalLight(DirectionalLight directionalLight, vec3 normal, vec3 viewDirection, vec3 textureColor, float specularColor) {
        // Calculate required directions
        const vec3 reflectionDirection = reflect(directionalLight.direction, normal);

        // Calculate diffuse and base specular values
        const float diffuseStrength = max(dot(normal, directionalLight.direction), 0.0);
        const float specularStrength = pow(max(dot(viewDirection, reflectionDirection), 0.0), max(pushConstant.material.shininess * 512, 1.0));

        // Calculate final light components
        const vec3 ambient = pushConstant.material.ambient * textureColor;
        const vec3 diffuse = pushConstant.material.diffuse * textureColor * diffuseStrength * directionalLight.color * directionalLight.intensity;
        const vec3 specular = pushConstant.material.specular * specularColor * specularStrength * directionalLight.color * directionalLight.intensity;

        return ambient + diffuse + specular;
}

vec3 CalculatePointLight(PointLight pointLight, vec3 fragmentPosition, vec3 normal, vec3 viewDirection, vec3 textureColor, float specularColor) {
        // Calculate required directions
        const vec3 lightDirection = normalize(pointLight.position - fragmentPosition);
        const vec3 reflectionDirection = reflect(lightDirection, normal);

        // Calculate diffuse and base specular values
        const float diffuseStrength = max(dot(normal, lightDirection), 0.0);
        const float specularStrength = pow(max(dot(viewDirection, reflectionDirection), 0.0), max(pushConstant.material.shininess * 512, 1.0));

        // Calculate light components
        vec3 ambient = pushConstant.material.ambient * textureColor;
        vec3 diffuse = pushConstant.material.diffuse * textureColor * diffuseStrength * pointLight.color * pointLight.intensity;
        vec3 specular = pushConstant.material.specular * specularColor * specularStrength * pointLight.color * pointLight.intensity;

        // Calculate attenuation
        const float distance = length(pointLight.position - fragmentPosition);
        const float attenuation = 1.0 / (1.0f + pointLight.linear * distance + pointLight.quadratic * (distance * distance));

        // Apply attenuation
        ambient  *= attenuation;
        diffuse  *= attenuation;
        specular *= attenuation;

        return ambient + diffuse + specular;
}