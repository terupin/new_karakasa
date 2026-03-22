cbuffer Transform : register(b0)
{
    float4x4 world;
    float4x4 viewProj;
};

cbuffer Light : register(b1)
{
    float3 lightDir;
    float pad0;

    float4 lightColor;
    float4 ambient;

    float3 cameraPos;
    float pad1;
};

cbuffer Material : register(b2)
{
    float4 baseColor;
    float specStrength;
    float shininess;
    float2 pad2;
};

struct VSInput
{
    float3 pos : POSITION;
    float3 nrm : NORMAL;
};

struct VSOutput
{
    float4 posH : SV_POSITION;
    float3 normalW : NORMAL;
    float3 worldPos : TEXCOORD0;
};

VSOutput VSMain(VSInput input)
{
    VSOutput o;

    float4 wpos = mul(float4(input.pos, 1.0f), world);
    o.posH = mul(wpos, viewProj);

    o.worldPos = wpos.xyz;
    o.normalW = mul(input.nrm, (float3x3) world);

    return o;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    float3 N = normalize(input.normalW);
    float3 L = normalize(-lightDir);
    float3 V = normalize(cameraPos - input.worldPos);

    float ndotl = saturate(dot(N, L));
    float3 diffuse = baseColor.rgb * lightColor.rgb * ndotl;

    float3 H = normalize(L + V);
    float spec = pow(saturate(dot(N, H)), shininess);
    float3 specular = lightColor.rgb * spec * specStrength;

    float3 ambientTerm = baseColor.rgb * ambient.rgb;
    float3 finalColor = ambientTerm + diffuse + specular;

    return float4(finalColor, baseColor.a);
}