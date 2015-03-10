#version 330

struct Material
{
	sampler2D diffuse;
	sampler2D normal;
	sampler2D specular;
};

struct DirectionalLight
{
	vec3 vColor;
	vec3 vDirection;
	float fAmbient;
	float fBrightness;
};

struct FogParameters
{
	vec4 vFogColor; // Fog color
	float fStart; // This is only for linear fog
	float fEnd; // This is only for linear fog
	float fDensity; // For exp and exp2 equation
	
	int iEquation; // 0 = linear, 1 = exp, 2 = exp2
};

float getFogFactor(FogParameters params, float fFogCoord)
{
	float fResult = 0.0;
	if(params.iEquation == 0)
		fResult = (params.fEnd-fFogCoord)/(params.fEnd-params.fStart);
	else if(params.iEquation == 1)
		fResult = exp(-params.fDensity*fFogCoord);
	else if(params.iEquation == 2)
		fResult = exp(-pow(params.fDensity*fFogCoord, 2.0));
		
	fResult = 1.0-clamp(fResult, 0.0, 1.0);
	
	return fResult;
}

uniform DirectionalLight sunLight;
uniform vec3 vEyePosition;
uniform Material mat;
uniform FogParameters fogParams;

uniform int bSkybox;
uniform bool bFog;
uniform bool bEnableNormalMap;

smooth in vec2 vTexCoord;
smooth in vec3 vNormal;
smooth in vec4 vEyeSpacePos;
smooth in vec3 vWorldPos;
in vec3 vLightDir_tangentspace;
in vec3 vEyeDir_tangentspace;

out vec4 outputColor;

void main()
{
	if (bSkybox == 1)
	{
		vec3 vTexColor = sunLight.vColor * vec3(texture2D(mat.diffuse, vTexCoord));
		outputColor = vec4(vTexColor * sunLight.fBrightness, 1.0f);
		if(bFog)
		{
			float fFogCoord = 500 *250*fogParams.fDensity;
			outputColor = mix(outputColor, fogParams.vFogColor, getFogFactor(fogParams, fFogCoord));
		}
		return;
	}

	vec3 vNormal_extended = normalize(vNormal);
	vec3 vLightDir = sunLight.vDirection;
	vec3 vEyeDirection = vEyePosition - vWorldPos;

	if(bEnableNormalMap)
	{
		vNormal_extended = normalize(texture2D(mat.normal, vTexCoord).rgb*2.0-1.0);
		vLightDir = normalize(vLightDir_tangentspace);
		vEyeDirection = vEyeDir_tangentspace;
	}

	//Diffuse light
	float fDiffuseIntensity = clamp(dot(vNormal_extended, -vLightDir), 0.0, 1.0);
	vec3 vDiffuseColor = sunLight.fBrightness * sunLight.vColor * fDiffuseIntensity * vec3(texture2D(mat.diffuse, vTexCoord));
	
	//Ambient
	vec3 vAmbientColor = sunLight.vColor * sunLight.fAmbient * vec3(texture2D(mat.diffuse, vTexCoord));
	
	//Specular	
	vec3 vReflected = reflect(vLightDir, vNormal_extended);
	vec3 vView = normalize(vEyeDirection);
	float fSpecularIntensity = clamp(dot(vReflected, vView), 0, 1);
	vec3 vSpecularColor = sunLight.fBrightness * 0.6 * sunLight.vColor * fSpecularIntensity * vec3(texture2D(mat.specular, vTexCoord));

	outputColor = vec4(vAmbientColor + vDiffuseColor + vSpecularColor, 1.0f);
	
	if (bFog)
	{
		float fFogCoord = distance(vEyePosition, vWorldPos);
		outputColor = mix(outputColor, fogParams.vFogColor, getFogFactor(fogParams, fFogCoord));
	}
}