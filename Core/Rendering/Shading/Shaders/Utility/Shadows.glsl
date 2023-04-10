#ifndef SHADOWS_GLSL
#define SHADOWS_GLSL

#include "../Utility/Math.glsl"
#include "../Utility/GaussianBlur.glsl"

const float SHADOW_BIAS = 0.001f;
const float SHADOW_TRANSPARENCY = 0.75f;

float CalculateShadow(sampler2D shadowMap, vec4 shadowCoordinates, vec2 offset)
{
    float shadow = 1.0;

    shadowCoordinates = shadowCoordinates / shadowCoordinates.w;
    shadowCoordinates.xy = shadowCoordinates.xy * 0.5 + 0.5;

    const float dist = texture(shadowMap, shadowCoordinates.xy + offset).r;
    if (shadowCoordinates.w > 0.0 && dist < shadowCoordinates.z)
    {
        shadow = 0.25f;
    }

    return shadow;
}

float CalculateShadow(sampler2DShadow shadowMap, vec4 shadowCoordinates, vec2 offset)
{
    float shadow = 1.0;

    shadowCoordinates = shadowCoordinates / shadowCoordinates.w;
    shadowCoordinates.xy = shadowCoordinates.xy * 0.5 + 0.5;

    const float dist = texture(shadowMap, shadowCoordinates.xyz + vec3(offset, -SHADOW_BIAS)).r;
    if (shadowCoordinates.w > 0.0 && dist < shadowCoordinates.z)
    {
        shadow = 0.25f;
    }

    return shadow;
}

float CalculateShadowPCF(sampler2DShadow shadowMap, vec4 shadowCoordinates)
{
    const ivec2 texDim = textureSize(shadowMap, 0);
    const float scale = 1.5;
    const float dx = scale * 1.0 / float(texDim.x);
    const float dy = scale * 1.0 / float(texDim.y);

    float shadowFactor = 0.0;
    int count = 0;
    const int range = 1;

    for (int x = -range; x <= range; x++)
    {
        for (int y = -range; y <= range; y++)
        {
            shadowFactor += CalculateShadow(shadowMap, shadowCoordinates, vec2(dx * x, dy * y));
            count++;
        }

    }

    return shadowFactor / count;
}

const vec2 poissonDisk[16] = vec2[](
    vec2( -0.942016240, -0.39906216  ),
    vec2(  0.945586090, -0.76890725  ),
    vec2( -0.094184101, -0.92938870  ),
    vec2(  0.344959380,  0.29387760  ),
    vec2( -0.915885810,  0.45771432  ),
    vec2( -0.815442320, -0.87912464  ),
    vec2( -0.382775430,  0.27676845  ),
    vec2(  0.974843980,  0.75648379  ),
    vec2(  0.443233250, -0.97511554  ),
    vec2(  0.537429810, -0.47373420  ),
    vec2( -0.264969110, -0.41893023  ),
    vec2(  0.791975140,  0.19090188  ),
    vec2( -0.241888400,  0.99706507  ),
    vec2( -0.814099550,  0.91437590  ),
    vec2(  0.199841260,  0.78641367  ),
    vec2(  0.143831610, -0.14100790  )
);

const uint POISSON_SAMPLE_COUNT = 4;
const float POISSON_RADIUS = 2.0f;
const float POISSON_SHARPNESS = 0.1f;

float CalculatePoissonShadowPCF(sampler2DShadow shadowMap, vec4 shadowCoordinates, vec3 fragmentPosition)
{
    const float texelSize = 1.0 / textureSize(shadowMap, 0).x;

    shadowCoordinates = shadowCoordinates / shadowCoordinates.w;
    shadowCoordinates.xy = shadowCoordinates.xy * 0.5 + 0.5;

    float shadow = 0.0;
    for (int i = 0; i < POISSON_SAMPLE_COUNT; i++)
    {
        uint index;

        #if defined(SETTINGS_USE_RANDOM_SAMPLE_FOR_POISSON)
            // No banding, but shadow moves with camera, causing a weird visual
            index = int(16.0 * Random(fragmentPosition, i)) % 16;
        #else
            // Gives a fixed pattern, but without noise
            index = i;
        #endif

        vec2 offset;
        #if defined(SETTINGS_USE_GRADIENT_SAMPLING_FOR_POISSON)
            const vec3 magic = vec3(0.06711056, 0.00583715, 52.9829189);
            const float rotationX = cos(2.0 * PI * fract(magic.z * fract(dot(gl_FragCoord.xy, magic.xy))));
            const float rotationY = sin(2.0 * PI * fract(magic.z * fract(dot(gl_FragCoord.xy, magic.xy))));
            const mat2x2 rotationMatrix = mat2x2(rotationX, rotationY, -rotationY, rotationX);
            offset = rotationMatrix * poissonDisk[index];
        #else
            offset = poissonDisk[index] * Random(vec3(poissonDisk[index], 1.0), i) * PI;
        #endif

        shadow += texture(shadowMap, shadowCoordinates.xyz + vec3(texelSize * POISSON_RADIUS * offset, -SHADOW_BIAS)).x;
    }

    return 1.0 - Clipstep(POISSON_SHARPNESS, 1.0, 1.0 - shadow / float(POISSON_SAMPLE_COUNT)) * SHADOW_TRANSPARENCY;
}

const float VARIANCE_PROBABILITY_LIMIT = 0.6f;

float CalculateVarianceShadow(sampler2D shadowMap, vec4 shadowCoordinates, vec3 fragmentPosition)
{
    shadowCoordinates = shadowCoordinates / shadowCoordinates.w;
    shadowCoordinates.xy = shadowCoordinates.xy * 0.5 + 0.5;

    const vec2 moments = texture(shadowMap, shadowCoordinates.xy).xy;

    const float p = step(shadowCoordinates.z, moments.x);
    const float variance = max(moments.y - moments.x * moments.x, 0.00002);

    const float d = shadowCoordinates.z - moments.x;
    const float pMax = variance / (variance + d * d);

    const float ls = Linstep(VARIANCE_PROBABILITY_LIMIT, 1.0, pMax);
    const float shadow = min(max(p, ls), 1.0);

    return shadow;
}

const float MOMENT_SHADOW_BIAS = 0.0022f;

float ComputeMSMHausdorff(vec4 moments, float depth);
float ComputeMSMHamburger(vec4 moments, float depth);

float CalculateMomentShadow(sampler2D shadowMap, vec4 shadowCoordinates, float depth)
{
    shadowCoordinates = shadowCoordinates / shadowCoordinates.w;
    shadowCoordinates.xy = shadowCoordinates.xy * 0.5 + 0.5;

    vec4 optimized = texture(shadowMap, shadowCoordinates.xy);
    optimized[0] -= 0.035955884801f;

    const vec4 moments = mat4x4(
                            0.2227744146f, 0.1549679261f, 0.1451988946f, 0.163127443f,
                            0.0771972861f, 0.1394629426f, 0.2120202157f, 0.2591432266f,
                            0.7926986636f, 0.7963415838f, 0.7258694464f, 0.6539092497f,
                            0.0319417555f,-0.1722823173f,-0.2758014811f,-0.3376131734f
                        ) * optimized;

//    #if defined(SETTINGS_USE_HAMBURGER_FOR_MOMENT_SHADOW_MAPPING)
        return ComputeMSMHamburger(moments, depth);
//    #else
//        return ComputeMSMHausdorff(moments, depth);
//    #endif

    return 1.0f;
}

float ComputeMSMHamburger(vec4 moments, float depth)
{
    // Bias input data to avoid artifacts
    vec4 b = Lerp(moments, vec4(0.5f, 0.5f, 0.5f, 0.5f), MOMENT_SHADOW_BIAS);
    vec3 z;
    z[0] = depth - SHADOW_BIAS;

    // Compute a Cholesky factorization of the Hankel matrix B storing only non-
    // trivial entries or related products
    //    float L32D22 = mad(-b[0], b[1], b[2]);
    float L32D22 = -b[0] * b[1] + b[2];
    //    float D22 = mad(-b[0], b[0], b[1]);
    float D22 = -b[0] * b[0] + b[1];
    //    float squaredDepthVariance = mad(-b[1], b[1], b[3]);
    float squaredDepthVariance = -b[1] * b[1] + b[3];
    float D33D22 = dot(vec2(squaredDepthVariance, -L32D22), vec2(D22, L32D22));
    float InvD22 = 1.0f / D22;
    float L32 = L32D22 * InvD22;

    // Obtain a scaled inverse image of bz = (1,z[0],z[0]*z[0])^T
    vec3 c = vec3(1.0f, z[0], z[0] * z[0]);

    // Forward substitution to solve L*c1=bz
    c[1] -= b.x;
    c[2] -= b.y + L32 * c[1];

    // Scaling to solve D*c2=c1
    c[1] *= InvD22;
    c[2] *= D22 / D33D22;

    // Backward substitution to solve L^T*c3=c2
    c[1] -= L32 * c[2];
    c[0] -= dot(c.yz, b.xy);

    // Solve the quadratic equation c[0]+c[1]*z+c[2]*z^2 to obtain solutions
    // z[1] and z[2]
    float p = c[1] / c[2];
    float q = c[0] / c[2];
    float D = (p * p * 0.25f) - q;
    float r = sqrt(D);
    z[1] =- p * 0.5f - r;
    z[2] =- p * 0.5f + r;

    // Compute the shadow intensity by summing the appropriate weights
    vec4 switchVal = (z[2] < z[0]) ? vec4(z[1], z[0], 1.0f, 1.0f) :
    ((z[1] < z[0]) ? vec4(z[0], z[1], 0.0f, 1.0f) :
    vec4(0.0f,0.0f,0.0f,0.0f));
    float quotient = (switchVal[0] * z[2] - b[0] * (switchVal[0] + z[2]) + b[1])/((z[2] - switchVal[1]) * (z[0] - z[1]));
    float shadowIntensity = switchVal[2] + switchVal[3] * quotient;
    return 1.0f - Normalize(shadowIntensity);
}

float ComputeMSMHausdorff(vec4 moments, float depth)
{
    // Bias input data to avoid artifacts
    vec4 b = Lerp(moments, vec4(0.5f, 0.5f, 0.5f, 0.5f), MOMENT_SHADOW_BIAS);
    vec3 z;
    z[0] = depth - SHADOW_BIAS;

    // Compute a Cholesky factorization of the Hankel matrix B storing only non-
    // trivial entries or related products
    //    float L32D22 = mad(-b[0], b[1], b[2]);
    float L32D22 = -b[0] * b[1] + b[2];
    //    float D22 = mad(-b[0], b[0], b[1]);
    //    float D22 = mad(-b[0], b[0], b[1]);
    float D22 = -b[0] * b[0] + b[1];
    //    float squaredDepthVariance = mad(-b[1], b[1], b[3]);
    float squaredDepthVariance = -b[1] * b[1] + b[3];
    float D33D22 = dot(vec2(squaredDepthVariance, -L32D22), vec2(D22, L32D22));
    float InvD22 = 1.0f / D22;
    float L32 = L32D22 * InvD22;

    // Obtain a scaled inverse image of bz=(1,z[0],z[0]*z[0])^T
    vec3 c = vec3(1.0f, z[0], z[0] * z[0]);

    // Forward substitution to solve L*c1=bz
    c[1] -= b.x;
    c[2] -= b.y + L32 * c[1];

    // Scaling to solve D*c2=c1
    c[1] *= InvD22;
    c[2] *= D22 / D33D22;

    // Backward substitution to solve L^T*c3=c2
    c[1] -= L32 * c[2];
    c[0] -= dot(c.yz, b.xy);

    // Solve the quadratic equation c[0]+c[1]*z+c[2]*z^2 to obtain solutions z[1]
    // and z[2]
    float p = c[1] / c[2];
    float q = c[0] / c[2];
    float D = ((p * p) / 4.0f) - q;
    float r = sqrt(D);
    z[1] =- (p / 2.0f) - r;
    z[2] =- (p / 2.0f) + r;

    float shadow = 1.0f;

    // Use a solution made of four deltas if the solution with three deltas is invalid
    if(z[1] < 0.0f || z[2] > 1.0f)
    {
        float zFree = ((b[2] - b[1]) * z[0] + b[2] - b[3]) / ((b[1] - b[0]) * z[0] + b[1] - b[2]);
        float w1Factor = (z[0] > zFree) ? 1.0f : 0.0f;
        shadow = (b[1] - b[0] + (b[2] - b[0] - (zFree + 1.0f) * (b[1] - b[0])) * (zFree - w1Factor - z[0]) / (z[0] * (z[0] - zFree))) / (zFree - w1Factor) + 1.0f - b[0];
    }
    // Use the solution with three deltas
    else
    {
        vec4 switchVal = (z[2] < z[0]) ? vec4(z[1], z[0], 1.0f, 1.0f) : ((z[1] < z[0]) ? vec4(z[0], z[1], 0.0f, 1.0f) : vec4(0.0f, 0.0f, 0.0f, 0.0f));
        float quotient = (switchVal[0] * z[2] - b[0] * (switchVal[0] + z[2]) + b[1]) / ((z[2] - switchVal[1]) * (z[0] - z[1]));
        shadow = switchVal[2] + switchVal[3] * quotient;
    }

    return 1.0f - Normalize(shadow);
}

#endif