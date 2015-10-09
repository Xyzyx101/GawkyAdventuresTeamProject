//=============================================================================
// Basic.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Basic effect that currently supports transformations, lighting, and texturing.
//=============================================================================

#include "LightHelper.fx"


 
cbuffer cbPerFrame
{
	DirectionalLight gDirLights[3];
	float3 gEyePosW;

	float  gFogStart;
	float  gFogRange;
	float4 gFogColor;
};

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	float4x4 gViewProj;
	float4x4 gWorldViewProj;
	float4x4 gTexTransform;
	Material gMaterial;
}; 

cbuffer cbSkinned 
{
	float4x4 gBoneTransforms[64];
};

// Nonnumeric values cannot be added to a cbuffer.
Texture2D gDiffuseMap;
TextureCube gCubeMap;

SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;

	AddressU = WRAP;
	AddressV = WRAP;
};
 
struct VertexIn
{
	float3 PosL    : POSITION;
	float3 NormalL : NORMAL;
	float2 Tex     : TEXCOORD;
};

struct SkinnedVertexIn 
{
	float3 PosL			: POSITION;
	float3 NormalL		: NORMAL;
	float2 Tex			: TEXCOORD;
	float4 TangentL		: TANGENT;
	float4 Weights		: WEIGHTS;
	uint4  BoneIndices	: BONEINDICES;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
    float3 PosW    : POSITION;
    float3 NormalW : NORMAL;
	float2 Tex     : TEXCOORD;
	float4 Debug   : COLOR1;
};

float gawkyLineThickness = 0.003f;
float objectLineThickness = 0.2f;

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to world space space.
	vout.PosW    = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
	vout.NormalW = mul(vin.NormalL, (float3x3)gWorldInvTranspose);
		
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	
	// Output vertex attributes for interpolation across triangle.
	vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;

	vout.Debug = float4(0.f, 0.f, 0.f, 1.f);

	return vout;
}
 
VertexOut SkinnedVS( SkinnedVertexIn vin ) 
{
	VertexOut vout;

	// Init array or else we get strange warnings about SV_POSITION.
	float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	weights[0] = vin.Weights.x;
	weights[1] = vin.Weights.y;
	weights[2] = vin.Weights.z;
	weights[3] = 1.0f-weights[0]-weights[1]-weights[2];

	vin.NormalL = normalize( vin.NormalL.xyz );

	float3 posL = float3(0.0f, 0.0f, 0.0f);
	float3 normalL = float3(0.0f, 0.0f, 0.0f);
	float3 tangentL = float3(0.0f, 0.0f, 0.0f);
	for( int i = 0; i < 4; ++i ) {
		// Assume no nonuniform scaling when transforming normals, so 
		// that we do not have to use the inverse-transpose.

		posL += weights[i]*mul( float4(vin.PosL, 1.0f), gBoneTransforms[vin.BoneIndices[i]] ).xyz;
		normalL += weights[i]*mul( vin.NormalL, (float3x3)gBoneTransforms[vin.BoneIndices[i]] );
		tangentL += weights[i]*mul( vin.TangentL.xyz, (float3x3)gBoneTransforms[vin.BoneIndices[i]] );
	}

	// Transform to world space space.
	vout.PosW = mul( float4(posL, 1.0f), gWorld ).xyz;
	vout.NormalW = mul( normalL, (float3x3)gWorldInvTranspose );
	
	// Transform to homogeneous clip space.
	vout.PosH = mul( float4(posL, 1.0f), gWorldViewProj );

	// Output vertex attributes for interpolation across triangle.
	vout.Tex = mul( float4(vin.Tex, 0.0f, 1.0f), gTexTransform ).xy;

	vout.Debug = float4(vout.PosW, 0.f);//float4(gBoneTransforms[4]._m30, gBoneTransforms[4]._m31, gBoneTransforms[4]._m32, 1.f);

	return vout;
}

float4 PS(VertexOut pin, 
          uniform int gLightCount, 
		  uniform bool gUseTexure, 
		  uniform bool gAlphaClip, 
		  uniform bool gFogEnabled, 
		  uniform bool gReflectionEnabled) : SV_Target
{
	// Interpolating normal can unnormalize it, so normalize it.
    pin.NormalW = normalize(pin.NormalW);

	// The toEye vector is used in lighting.
	float3 toEye = gEyePosW - pin.PosW;

	// Cache the distance to the eye from this surface point.
	float distToEye = length(toEye);

	// Normalize.
	toEye /= distToEye;
	
    // Default to multiplicative identity.
    float4 texColor = float4(1, 1, 1, 1);
    if(gUseTexure)
	{
		// Sample texture.
		texColor = gDiffuseMap.Sample( samAnisotropic, pin.Tex );

		if(gAlphaClip)
		{
			// Discard pixel if texture alpha < 0.1.  Note that we do this
			// test as soon as possible so that we can potentially exit the shader 
			// early, thereby skipping the rest of the shader code.
			clip(texColor.a - 0.1f);
		}
	}
	 
	//
	// Lighting.
	//

	float4 litColor = texColor;
	if( gLightCount > 0  )
	{  
		// Start with a sum of zero. 
		float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

		// Sum the light contribution from each light source.  
		[unroll]
		for(int i = 0; i < gLightCount; ++i)
		{
			float4 A, D, S;
			ComputeDirectionalLight(gMaterial, gDirLights[i], pin.NormalW, toEye, 
				A, D, S);

			ambient += A;
			diffuse += D;
			spec    += S;
		}

		litColor = texColor*(ambient + diffuse) + spec;

		if( gReflectionEnabled )
		{
			float3 incident = -toEye;
			float3 reflectionVector = reflect(incident, pin.NormalW);
			float4 reflectionColor  = gCubeMap.Sample(samAnisotropic, reflectionVector);

			litColor += gMaterial.Reflect*reflectionColor;
		}
	}
 
	//
	// Fogging
	//

	if( gFogEnabled )
	{
		float fogLerp = saturate( (distToEye - gFogStart) / gFogRange ); 

		// Blend the fog color and the lit color.
		litColor = lerp(litColor, gFogColor, fogLerp);
	}

	// Common to take alpha from diffuse material and texture.
	litColor.a = gMaterial.Diffuse.a * texColor.a;

	//return pin.Debug;
    return litColor;
}

VertexOut OutlineVS( VertexIn vin ) {
	VertexOut vout;

	vin.NormalL = normalize( vin.NormalL.xyz );

	// Transform to world space space.
	float3 original = mul( float4(vin.PosL, 1.0f), gWorld ).xyz;
	vout.NormalW = mul( vin.NormalL, (float3x3)gWorldInvTranspose );
	vout.PosW = original+mul( objectLineThickness, vout.NormalW );

	// Transform to homogeneous clip space.
	vout.PosH = mul( float4(vout.PosW, 1.f), gViewProj );

	// Output vertex attributes for interpolation across triangle.
	vout.Tex = mul( float4(vin.Tex, 0.0f, 1.0f), gTexTransform ).xy;

	vout.Debug = float4(original, 1.f);

	return vout;
}

VertexOut OutlineSkinnedVS( SkinnedVertexIn vin ) {
	VertexOut vout;

	// Init array or else we get strange warnings about SV_POSITION.
	float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	weights[0] = vin.Weights.x;
	weights[1] = vin.Weights.y;
	weights[2] = vin.Weights.z;
	weights[3] = 1.0f-weights[0]-weights[1]-weights[2];

	vin.NormalL = normalize( vin.NormalL.xyz );

	float3 posL = float3(0.0f, 0.0f, 0.0f);
		float3 normalL = float3(0.0f, 0.0f, 0.0f);
		float3 tangentL = float3(0.0f, 0.0f, 0.0f);
		for( int i = 0; i < 4; ++i ) {
			// Assume no nonuniform scaling when transforming normals, so 
			// that we do not have to use the inverse-transpose.

			posL += weights[i]*mul( float4(vin.PosL, 1.0f), gBoneTransforms[vin.BoneIndices[i]] ).xyz;
			normalL += weights[i]*mul( vin.NormalL, (float3x3)gBoneTransforms[vin.BoneIndices[i]] );
			tangentL += weights[i]*mul( vin.TangentL.xyz, (float3x3)gBoneTransforms[vin.BoneIndices[i]] );
		}

	// Transform to world space space.
	float3 original = mul( float4(posL, 1.0f), gWorld ).xyz;
	vout.NormalW = mul( normalL, (float3x3)gWorldInvTranspose );
	vout.PosW = original+mul( gawkyLineThickness, vout.NormalW );

	// Transform to homogeneous clip space.
	vout.PosH = mul( float4(vout.PosW, 1.f), gViewProj );
	
	// Output vertex attributes for interpolation across triangle.
	vout.Tex = mul( float4(vin.Tex, 0.0f, 1.0f), gTexTransform ).xy;

	vout.Debug = float4(vout.PosW, 0.f);//float4(gBoneTransforms[4]._m30, gBoneTransforms[4]._m31, gBoneTransforms[4]._m32, 1.f);

	return vout;
}

float4 OutlinePS( VertexOut pin) : SV_Target
{
	float4 texColor = gDiffuseMap.Sample( samAnisotropic, pin.Tex );
	
	float4 litColor = 0.05f * texColor;
		
	// Common to take alpha from diffuse material and texture.
	litColor.a = gMaterial.Diffuse.a * texColor.a;

	//return pin.Debug;
	return litColor;
}

float4 CelShadePS( VertexOut pin) : SV_Target
{
	// Interpolating normal can unnormalize it, so normalize it.
	pin.NormalW = normalize( pin.NormalW );

	// The toEye vector is used in lighting.
	float3 toEye = gEyePosW-pin.PosW;

	// Cache the distance to the eye from this surface point.
	float distToEye = length( toEye );

	// Normalize.
	toEye /= distToEye;

	// Default to multiplicative identity.
	float4 texColor = gDiffuseMap.Sample( samAnisotropic, pin.Tex );

	float4 litColor = texColor;
		
	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
	ComputeDirectionalLight( gMaterial, gDirLights[0], pin.NormalW, toEye,	ambient, diffuse, spec );

	if( diffuse.x>0.3f ) {
		litColor = float4(1.f, 1.f, 1.f, 1.f) * texColor;
	} else if( diffuse.x>0.05f ) {
		litColor = float4(0.8f, 0.8f, 0.8f, 1.f) * texColor;
	} else {
		litColor = float4(0.1f, 0.1f, 0.1f, 1.f) * texColor;
	}

	if( spec.r>0.15f ) {
		litColor = lerp( texColor, float4(1.f, 1.f, 1.f, 1.f), 0.5f ); 
	}

	return litColor;
}

RasterizerState CW {
	FrontCounterClockwise = FALSE;
};
RasterizerState CCW {
	FrontCounterClockwise = TRUE;
};

technique11 Light1
{
	pass P0 {
		SetVertexShader( CompileShader(vs_5_0, OutlineVS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, OutlinePS()) );
		SetRasterizerState( CCW );
	}
	pass P1 {
		SetVertexShader( CompileShader(vs_5_0, VS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, CelShadePS()) );
		SetRasterizerState( CW );
	}
}

technique11 Light2
{
	pass P0 {
		SetVertexShader( CompileShader(vs_5_0, OutlineVS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, OutlinePS()) );
		SetRasterizerState( CCW );
	}
	pass P1 {
		SetVertexShader( CompileShader(vs_5_0, VS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, CelShadePS()) );
		SetRasterizerState( CW );
	}
}

technique11 Light3
{
	pass P0 {
		SetVertexShader( CompileShader(vs_5_0, OutlineVS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, OutlinePS()) );
		SetRasterizerState( CCW );
	}
	pass P1 {
		SetVertexShader( CompileShader(vs_5_0, VS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, CelShadePS()) );
		SetRasterizerState( CW );
	}
}

technique11 Light0Tex
{
	pass P0 {
		SetVertexShader( CompileShader(vs_5_0, OutlineVS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, OutlinePS()) );
		SetRasterizerState( CCW );
	}
	pass P1 {
		SetVertexShader( CompileShader(vs_5_0, VS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, CelShadePS()) );
		SetRasterizerState( CW );
	}
}

technique11 Light1Tex
{
	pass P0 {
		SetVertexShader( CompileShader(vs_5_0, OutlineVS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, OutlinePS()) );
		SetRasterizerState( CCW );
	}
	pass P1 {
		SetVertexShader( CompileShader(vs_5_0, VS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, CelShadePS()) );
		SetRasterizerState( CW );
	}
}

technique11 Light2Tex
{
	pass P0 {
		SetVertexShader( CompileShader(vs_5_0, OutlineVS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, OutlinePS()) );
		SetRasterizerState( CCW );
	}
	pass P1 {
		SetVertexShader( CompileShader(vs_5_0, VS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, CelShadePS()) );
		SetRasterizerState( CW );
	}
}

technique11 Light3Tex
{
	pass P0 {
		SetVertexShader( CompileShader(vs_5_0, OutlineVS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, OutlinePS()) );
		SetRasterizerState( CCW );
	}
	pass P1 {
		SetVertexShader( CompileShader(vs_5_0, VS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, CelShadePS()) );
		SetRasterizerState( CW );
	}
}

technique11 Light1Reflect
{
	pass P0 {
		SetVertexShader( CompileShader(vs_5_0, OutlineVS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, OutlinePS()) );
		SetRasterizerState( CCW );
	}
	pass P1 {
		SetVertexShader( CompileShader(vs_5_0, VS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, CelShadePS()) );
		SetRasterizerState( CW );
	}
}

technique11 Light2Reflect
{
	pass P0 {
		SetVertexShader( CompileShader(vs_5_0, OutlineVS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, OutlinePS()) );
		SetRasterizerState( CCW );
	}
	pass P1 {
		SetVertexShader( CompileShader(vs_5_0, VS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, CelShadePS()) );
		SetRasterizerState( CW );
	}
}

technique11 Light3Reflect
{
	pass P0 {
		SetVertexShader( CompileShader(vs_5_0, OutlineVS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, OutlinePS()) );
		SetRasterizerState( CCW );
	}
	pass P1 {
		SetVertexShader( CompileShader(vs_5_0, VS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, CelShadePS()) );
		SetRasterizerState( CW );
	}
}

technique11 Light0TexReflect
{
	pass P0 {
		SetVertexShader( CompileShader(vs_5_0, OutlineVS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, OutlinePS()) );
		SetRasterizerState( CCW );
	}
	pass P1 {
		SetVertexShader( CompileShader(vs_5_0, VS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, CelShadePS()) );
		SetRasterizerState( CW );
	}
}

technique11 Light1TexReflect
{
	pass P0 {
		SetVertexShader( CompileShader(vs_5_0, OutlineVS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, OutlinePS()) );
		SetRasterizerState( CCW );
	}
	pass P1 {
		SetVertexShader( CompileShader(vs_5_0, VS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, CelShadePS()) );
		SetRasterizerState( CW );
	}
}

technique11 Light2TexReflect
{
	pass P0 {
		SetVertexShader( CompileShader(vs_5_0, OutlineVS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, OutlinePS()) );
		SetRasterizerState( CCW );
	}
	pass P1 {
		SetVertexShader( CompileShader(vs_5_0, VS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, CelShadePS()) );
		SetRasterizerState( CW );
	}
}

technique11 Light3TexReflect
{
	pass P0 {
		SetVertexShader( CompileShader(vs_5_0, OutlineVS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, OutlinePS()) );
		SetRasterizerState( CCW );
	}
	pass P1 {
		SetVertexShader( CompileShader(vs_5_0, VS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, CelShadePS()) );
		SetRasterizerState( CW );
	}
}

technique11 Light0TexSkinned {
	pass P0 {
		SetVertexShader( CompileShader(vs_5_0, OutlineSkinnedVS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, OutlinePS()) );
		SetRasterizerState( CCW );
	}
	pass P1 {
		SetVertexShader( CompileShader(vs_5_0, SkinnedVS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, CelShadePS()) );
		SetRasterizerState( CW );
	}
}

technique11 Light1TexSkinned {
	pass P0 {
		SetVertexShader( CompileShader(vs_5_0, OutlineSkinnedVS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, OutlinePS()) );
		SetRasterizerState( CCW );
	}
	pass P1 {
		SetVertexShader( CompileShader(vs_5_0, SkinnedVS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, CelShadePS()) );
		SetRasterizerState( CW );
	}
}

technique11 Light2TexSkinned {
	pass P0 {
		SetVertexShader( CompileShader(vs_5_0, OutlineSkinnedVS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, OutlinePS()) );
		SetRasterizerState( CCW );
	}
	pass P1 {
		SetVertexShader( CompileShader(vs_5_0, SkinnedVS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, CelShadePS()) );
		SetRasterizerState( CW );
	}
}

technique11 Light3TexSkinned {
	pass P0 {
		SetVertexShader( CompileShader(vs_5_0, OutlineSkinnedVS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, OutlinePS()) );
		SetRasterizerState( CCW );
	}
	pass P1 {
		SetVertexShader( CompileShader(vs_5_0, SkinnedVS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, CelShadePS()) );
		SetRasterizerState( CW );
	}
}