cbuffer Transform : register(b0)
{
    float4x4 world;
    float4x4 viewProj;
}

cbuffer Light : register(b1)
{
    float3 lightDir;
    float _pad0;
    float4 lightColor;
    float4 ambient;
}

struct VSInput
{
    float3 pos : POSITION;
    float3 nrm : NORMAL;
    float4 col : COLOR;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float3 nrmW : NORMAL;
    float4 col : COLOR;
};

VSOutput VSMain(VSInput input)
{
    VSOutput o;
    
    float4 wpos = mul(float4(input.pos, 1.0f), world);
    o.pos = mul(wpos, viewProj);
    
    o.nrmW = mul(input.nrm, (float3x3) world);
    o.col = input.col;
    return o;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    float3 N = normalize(input.nrmW);
    
    float3 L = normalize(-lightDir); //物体からの光源方向はマイナスになる
    
    float ndotl = saturate(dot(N, L));
    float4 diffuse = ndotl * lightColor;
    
    return input.col * (ambient + diffuse);
}