struct _29
{
    float4 color;
    float2 UV;
};

cbuffer PushConstant
{
    uint pushConstant_fontAtlasIndex : packoffset(c0);
    uint pushConstant_fontSamplerIndex : packoffset(c0.y);
    float2 pushConstant_scale : packoffset(c0.z);
};

Texture2D<float4> _sierra_sampled_image_declaration[] : register(t2, space0);
SamplerState _sierra_sampler_declaration[] : register(s6, space0);

static float4 Out;
static _29 In;

struct SPIRV_Cross_Input
{
    _29 In : TEXCOORD0;
};

struct SPIRV_Cross_Output
{
    float4 Out : SV_Target0;
};

void main_inner()
{
    Out = In.color * _sierra_sampled_image_declaration[pushConstant_fontAtlasIndex].Sample(_sierra_sampler_declaration[pushConstant_fontSamplerIndex], In.UV).x;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    In = stage_input.In;
    main_inner();
    SPIRV_Cross_Output stage_output;
    stage_output.Out = Out;
    return stage_output;
}
