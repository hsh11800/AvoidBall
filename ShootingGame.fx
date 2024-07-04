//--------------------------------------------------------------------------------------
// File: ShootingGame.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------


struct VS_OUT
{
    float4 pos      : SV_POSITION;
    float4 color    : COLOR0;
};
cbuffer ConstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
}
VS_OUT VS(float4 Pos : POSITION, float4 Color : COLOR)
{
    VS_OUT vsOut = (VS_OUT)0;

    vsOut.pos = mul(Pos, World);
    vsOut.pos = mul(vsOut.pos, View);
    vsOut.pos = mul(vsOut.pos, Projection);
    //vsOut.pos = Pos;
    vsOut.color = Color;
    return vsOut;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUT ps_input) : SV_Target
{
    return ps_input.color;
//return float4(1.0f,1.0f,0.0f,1.0f);// Yellow, with Alpha = 1
}
