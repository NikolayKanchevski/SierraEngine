struct _41
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


static float4 gl_Position;
static float2 position;
static _41 Out;
static float4 color;
static float2 UV;

struct SPIRV_Cross_Input
{
    float2 position : TEXCOORD0;
    float2 UV : TEXCOORD1;
    float4 color : TEXCOORD2;
};

struct SPIRV_Cross_Output
{
    _41 Out : TEXCOORD0;
    float4 gl_Position : SV_Position;
};

void main_inner()
{
    gl_Position = float4(mad(pushConstant_scale, position, float2(-1.0f, 1.0f)), 0.0f, 1.0f);
    Out.color = color;
    Out.UV = UV;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    position = stage_input.position;
    color = stage_input.color;
    UV = stage_input.UV;
    main_inner();
    SPIRV_Cross_Output stage_output;
    stage_output.gl_Position = gl_Position;
    stage_output.Out = Out;
    return stage_output;
}
