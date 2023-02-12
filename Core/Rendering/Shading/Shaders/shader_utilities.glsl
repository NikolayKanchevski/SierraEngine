#version 450

const uint DIFFUSE_TEXTURE_OFFSET = 0;
const uint SPECULAR_TEXTURE_OFFSET = 1;
const uint NORMAL_MAP_TEXTURE_OFFSET = 3;
const uint HEIGHT_MAP_TEXTURE_OFFSET = 3;
const uint TOTAL_TEXTURE_TYPES_COUNT = 4;

const uint UNIFORM_BUFFER_BINDING = 0;
const uint STORAGE_BUFFER_BINDING = 1;
const uint DIFFUSE_TEXTURE_BINDING = 2;
const uint SPECULAR_TEXTURE_BINDING = 3;
const uint NORMAL_MAP_TEXTURE_BINDING = 4;
const uint HEIGHT_MAP_TEXTURE_BINDING = 5;

const uint MAX_MESHES = 8192;
const uint MAX_TEXTURES = MAX_MESHES * TOTAL_TEXTURE_TYPES_COUNT;
const uint MAX_POINT_LIGHTS = 64;
const uint MAX_DIRECTIONAL_LIGHTS = 16;

const float AMBIENT_STRENGTH = 0.05f;

/* --- STRUCTS --- */

struct Material
{
    vec3 diffuse;
    float specular;

    float shininess;
    float ambient;

    float vertexExaggeration;
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
} uniformBuffer;

layout(std140, set = 0, binding = 1) readonly buffer StorageBuffer
{
    ObjectData objectDatas[MAX_MESHES];

    DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
    PointLight pointLights[MAX_POINT_LIGHTS];

    uint directionalLightCount;
    uint pointLightCount;
} storageBuffer;

/* --- METHODS --- */

bool IsBitSet(uint binaryValue, uint bitIndex)
{
    return (binaryValue & (1 << bitIndex)) > 0;
}

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