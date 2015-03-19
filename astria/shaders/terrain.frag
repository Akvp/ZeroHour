#version 330

struct Material
{
	sampler2D diffuse;
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
uniform float fRenderHeight;
uniform float fMaxTextureU;
uniform float fMaxTextureV;
uniform Material LowAlt;
uniform Material MidAlt;
uniform Material HighAlt;
uniform vec4 vColor;
uniform vec3 vEyePosition;
uniform FogParameters fogParams;
uniform sampler2D sShadowMap;

//boolean uniforms
uniform bool bFog;
uniform bool bEnableShadowMap;

smooth in vec2 vTexCoord;
smooth in vec3 vNormal;
smooth in vec3 vWorldPos;
smooth in vec4 vEyeSpacePos;
smooth in vec4 vShadowCoord;

out vec4 outputColor;

//function prototypes
float GetVisibility(sampler2D shadowMap, vec4 vShadowCoord);

void main()
{
	vec3 vNormalized = normalize(vNormal);
	
	vec4 vDifColor = vec4(0.0);
	vec4 vSpeColor = vec4(0.0);
	
	float fScale = vWorldPos.y/fRenderHeight;

	const float fRange1 = 0.15f;
	const float fRange2 = 0.3f;
	const float fRange3 = 0.65f;
	const float fRange4 = 0.85f;

	if(fScale >= 0.0 && fScale <= fRange1)
	{
		vDifColor = texture2D(LowAlt.diffuse, vTexCoord);
		vSpeColor = texture2D(LowAlt.specular, vTexCoord);
	}
	else if(fScale <= fRange2)
	{
		fScale -= fRange1;
		fScale /= (fRange2-fRange1);
		
		float fScale2 = fScale;
		fScale = 1.0-fScale; 
		
		vDifColor += texture2D(LowAlt.diffuse, vTexCoord)*fScale;
		vDifColor += texture2D(MidAlt.diffuse, vTexCoord)*fScale2;
		vSpeColor += texture2D(LowAlt.specular, vTexCoord)*fScale;
		vSpeColor += texture2D(MidAlt.specular, vTexCoord)*fScale2;
	}
	else if(fScale <= fRange3)
	{
		vDifColor = texture2D(MidAlt.diffuse, vTexCoord);
		vSpeColor = texture2D(MidAlt.specular, vTexCoord);
	}
	else if(fScale <= fRange4)
	{
		fScale -= fRange3;
		fScale /= (fRange4-fRange3);
		
		float fScale2 = fScale;
		fScale = 1.0-fScale; 
		
		vDifColor += texture2D(MidAlt.diffuse, vTexCoord)*fScale;
		vDifColor += texture2D(HighAlt.diffuse, vTexCoord)*fScale2;	
		vSpeColor += texture2D(MidAlt.specular, vTexCoord)*fScale;
		vSpeColor += texture2D(HighAlt.specular, vTexCoord)*fScale2;	
	}
	else 
	{
		vDifColor = texture2D(HighAlt.diffuse, vTexCoord);	
		vSpeColor = texture2D(HighAlt.specular, vTexCoord);
	}

	//vec2 vPathCoord = vec2(vTexCoord.x/fMaxTextureU, vTexCoord.y/fMaxTextureV);
	//vec4 vPathIntensity = texture2D(gSampler[4], vPathCoord);
	//fScale = vPathIntensity.x;
  
	//vec4 vPathColor = texture2D(gSampler[3], vTexCoord); // Black color means there is a path
	//vec4 vFinalTexColor = (fScale+0.2)*vDifColor+(1-fScale)*vPathColor;

	//vec4 vMixedColor = vFinalTexColor*vColor;
	//vec4 vDirLightColor = GetDirectionalLightColor(sunLight, vNormal);

	float fDiffuseIntensity = clamp(dot(vNormal, -sunLight.vDirection), 0.0f, 1.5f);
	vec4 vDiffuseColor = pow(sunLight.fBrightness,2) * 0.8 * vec4(sunLight.vColor, 1.0f) * fDiffuseIntensity * vDifColor;

	vec4 vAmbientColor = vec4(sunLight.vColor, 1.0f) * sunLight.fAmbient * vDifColor;

	vec3 vReflected = normalize(reflect(sunLight.vDirection, vNormalized));
	vec3 vView = normalize(vEyePosition - vWorldPos);
	float fSpecularIntensity = clamp(dot(vReflected, vView), 0, 1);
	vec4 vSpecularColor = pow(sunLight.fBrightness, 2) * 0.6 * vec4(sunLight.vColor, 1.0f) * fSpecularIntensity * vSpeColor;

	outputColor = vDiffuseColor + vAmbientColor + vSpecularColor;
	float visibility = GetVisibility(sShadowMap, vShadowCoord);

	if(bFog)
	{	
		float fFogCoord = distance(vEyePosition, vWorldPos);
		outputColor = mix(outputColor, fogParams.vFogColor, getFogFactor(fogParams, fFogCoord));
	}
}              

vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

float GetVisibility(sampler2D shadowMap, vec4 vShadowCoord)
{
    if(!bEnableShadowMap)return 1.0;
    float visibility = 1.0;
    float bias = 0.005;
 
    vec4 shadowCoordinateWdivide = vec4(vShadowCoord.x/vShadowCoord.w, vShadowCoord.y/vShadowCoord.w, (vShadowCoord.z-bias)/vShadowCoord.w, 1.0) ;

    for (int i=0;i<4;i++)
    {
      int index = i;
		  vec4 vShadowSmooth = vec4(vShadowCoord.x + poissonDisk[index].x/700.0, vShadowCoord.y + poissonDisk[index].y/700.0, (vShadowCoord.z-bias)/vShadowCoord.w, 1.0);
		  float fSub = texture(shadowMap, vShadowSmooth.st).r; 
		  visibility -= 0.1*(1.0-fSub);
    }
    return visibility;
}
        