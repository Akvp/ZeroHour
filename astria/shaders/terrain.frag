#version 330

smooth in vec2 vTexCoord;
smooth in vec3 vNormal;
smooth in vec3 vWorldPos;
smooth in vec4 vEyeSpacePos;

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
};
uniform Material LowAlt;
uniform Material MidAlt;
uniform Material HighAlt;

uniform vec4 vColor;

struct DirectionalLight
{
	vec3 vColor;
	vec3 vDirection;
	float fAmbient;
	float fBrightness;
};
uniform DirectionalLight sunLight;
uniform float fRenderHeight;
uniform float fMaxTextureU;
uniform float fMaxTextureV;

out vec4 outputColor;

uniform vec3 vEyePosition;

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
  
}                      