struct VSInput
{
    float3 pos : POSITION;
    float4 col : COLOR;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
};

VSOutput VSMain(VSInput input)
{
    VSOutput o;
    o.pos = float4(input.pos, 1.0f);
    o.col = input.col;
    return o;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    return input.col;
}