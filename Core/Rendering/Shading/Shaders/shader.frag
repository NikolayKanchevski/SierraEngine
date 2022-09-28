#version 450

layout(location = 0) in vec3 fromVert_Position;
layout(location = 1) in vec3 fromVert_Normal;
layout(location = 2) in vec2 fromVert_TextureCoordinates;

const uint MAX_POINT_LIGHTS = 64;
const uint MAX_DIRECTIONAL_LIGHTS = 16; 
const uint MAX_SPOTLIGHTS = 16; 

struct DirectionalLight {
        vec3 direction;
        float intensity;
        
        vec3 color;
        float _align1_;
};

struct PointLight {
        vec3 position;
        float linear;
        
        vec3 color;
        float intensity;
        
        vec3 _align_1;
        float quadratic;
};

struct Spotlight {
        vec3 position;
        float radius;
        
        vec3 direction;
        float intensity;
        
        vec3 color;
        float linear;
        
        float _align1_;
        float _align2_;
        
        float quadratic;
        float spreadRadius;
};

layout(set = 0, binding = 0) uniform UniformBuffer {
        /* VERTEX DATA */
        mat4 view;
        mat4 projection;

        /* FRAGMENT DATA */
        DirectionalLight[MAX_DIRECTIONAL_LIGHTS] directionalLights;
        PointLight[MAX_POINT_LIGHTS] pointLights;
        Spotlight[MAX_SPOTLIGHTS] spotlights;

        int directionalLightsCount;
        int pointLightsCount;
        int spotlightsCount;
} ub;

struct Material {
        vec3 diffuse;
        float shininess;

        vec3 specular;
        float _align1_;

        vec3 ambient;
        float _align2_;
};

layout(push_constant) uniform PushConstant {
        /* VERTEX DATA */
        mat4 model;

        /* FRAGMENT DATA */
        Material material;
} pushConstant;

layout(set = 1, binding = 1) uniform sampler2D diffuseSampler;
layout(set = 2, binding = 2) uniform sampler2D specularSampler;

layout(location = 0) out vec4 outColor;

vec3 textureColor;
float specularColor;

vec3 viewDirection;

//vec3 normal;

vec3 CalculateDirectionalLight(DirectionalLight directionalLight);
vec3 CalculatePointLight(PointLight pointLight);
vec3 Calculatespotlight(Spotlight spotlight);

void main() {
        // Get the texture color
        textureColor = texture(diffuseSampler, fromVert_TextureCoordinates).rgb;
        specularColor = texture(specularSampler, fromVert_TextureCoordinates).r;
        
        // Set view direction
        viewDirection = normalize(-fromVert_Position);
        
        // Define end fragment color
        vec3 calculatedColor = vec3(0, 0, 0);
        
        // Directional light data
        for (int i = 0; i < ub.directionalLightsCount; i++) {
                if (ub.directionalLights[i].intensity <= 0.0001) continue;
                calculatedColor += CalculateDirectionalLight(ub.directionalLights[i]);
        }

        // For each point light calculate its color
        for (int i = 0; i < ub.pointLightsCount; i++) {
                if (ub.pointLights[i].intensity <= 0.0001) continue;
                calculatedColor += CalculatePointLight(ub.pointLights[i]);
        }
        
        // For each spot light calculate its color
        for (int i = 0; i < ub.spotlightsCount; i++) {
                if (ub.spotlights[i].intensity <= 0.0001) continue;
                calculatedColor += Calculatespotlight(ub.spotlights[i]);
        }
        
        outColor = vec4(calculatedColor, 1.0);
}

vec3 CalculateDirectionalLight(DirectionalLight directionalLight) {
        // Calculate required directions
        const vec3 reflectionDirection = reflect(directionalLight.direction, fromVert_Normal);

        // Calculate diffuse and base specular values
        const float diffuseStrength = max(dot(fromVert_Normal, directionalLight.direction), 0.0);
        const float specularStrength = pow(max(dot(viewDirection, reflectionDirection), 0.0), max(pushConstant.material.shininess * 512, 1.0));

        // Calculate final light components
        const vec3 ambient = pushConstant.material.ambient * textureColor;
        const vec3 diffuse = pushConstant.material.diffuse * textureColor * diffuseStrength * directionalLight.color * directionalLight.intensity;
        const vec3 specular = pushConstant.material.specular * specularColor * specularStrength * directionalLight.color * directionalLight.intensity;
        
        return (ambient + diffuse + specular);
} 

vec3 CalculatePointLight(PointLight pointLight) {
        // Calculate required directions
        const vec3 lightDirection = normalize(pointLight.position - fromVert_Position);
        const vec3 reflectionDirection = reflect(lightDirection, fromVert_Normal);

        // Calculate diffuse and base specular values
        const float diffuseStrength = max(dot(fromVert_Normal, lightDirection), 0.0);
        const float specularStrength = pow(max(dot(viewDirection, reflectionDirection), 0.0), max(pushConstant.material.shininess * 512, 1.0));

        // Calculate light components
        vec3 ambient = pushConstant.material.ambient * textureColor;
        vec3 diffuse = pushConstant.material.diffuse * textureColor * diffuseStrength * pointLight.color * pointLight.intensity;
        vec3 specular = pushConstant.material.specular * specularColor * specularStrength * pointLight.color * pointLight.intensity;
        
        // Calculate attenuation
        const float distance = length(pointLight.position - fromVert_Position);
        const float attenuation = 1.0 / (1.0f + pointLight.linear * distance + pointLight.quadratic * (distance * distance));

        // Apply attenuation
        ambient  *= attenuation;
        diffuse  *= attenuation;
        specular *= attenuation;
        
        return (ambient + diffuse + specular);
}

vec3 Calculatespotlight(Spotlight spotlight) {
        if (spotlight.radius < spotlight.spreadRadius) {
                spotlight.spreadRadius = spotlight.radius;
        }

        // Calculate required directions
        const vec3 lightDirection = normalize(spotlight.position - fromVert_Position);
        const vec3 reflectionDirection = reflect(-lightDirection, fromVert_Normal);

        const float theta = dot(lightDirection, normalize(-spotlight.direction));
        const float epsilon = (spotlight.radius - spotlight.spreadRadius);
        const float calculatedIntensity = clamp((theta - spotlight.spreadRadius) / epsilon, 0.0, 1.0) * spotlight.intensity;

        // Calculate diffuse and base specular values
        const float diffuseStrength = max(dot(fromVert_Normal, -lightDirection), 0.0);
        const float specularStrength = pow(max(dot(viewDirection, reflectionDirection), 0.0), max(pushConstant.material.shininess * 512, 1.0));

        // Calculate final light components
        vec3 ambient = pushConstant.material.ambient * textureColor;
        vec3 diffuse = pushConstant.material.diffuse * textureColor * diffuseStrength * spotlight.color * calculatedIntensity;
        vec3 specular = pushConstant.material.specular * specularColor * specularStrength * spotlight.color * calculatedIntensity;

        // Calculate attenuation
        const float distance = length(spotlight.position - fromVert_Position);
        const float attenuation = 1.0 / (1.0f + spotlight.linear * distance + spotlight.quadratic * (distance * distance));

        // Apply attenuation
        ambient  *= attenuation;
        diffuse  *= attenuation;
        specular *= attenuation;

        return (ambient + diffuse + specular);
}