#version 330

smooth in vec2 vTexCoord;
smooth in vec3 vNormal;
smooth in vec4 vEyeSpacePos;
smooth in vec3 vWorldPos;
out vec4 outputColor;

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


uniform DirectionalLight sunLight;
uniform vec3 vEyePosition;
uniform Material mat;

uniform int bSkybox;

void main()
{
	if (bSkybox == 1)
	{
		vec3 vTexColor = sunLight.vColor * vec3(texture2D(mat.diffuse, vTexCoord));
		outputColor = vec4(vTexColor * sunLight.fBrightness, 1.0f);
		return;
	}

	vec3 vNormalized = normalize(vNormal);

	//Diffuse light
	float fDiffuseIntensity = max(0.0, dot(vNormal, -sunLight.vDirection));
	vec3 vDiffuseColor = sunLight.vColor * fDiffuseIntensity * vec3(texture2D(mat.diffuse, vTexCoord));
	
	//Ambient
	vec3 vAmbientColor = sunLight.vColor * sunLight.fAmbient * vec3(texture2D(mat.diffuse, vTexCoord));
	
	//Specular
	vec3 vReflected = normalize(reflect(sunLight.vDirection, vNormalized));
	vec3 vView = normalize(vEyePosition - vWorldPos);
	float fSpecularIntensity = dot(vReflected, vView);
	vec3 vSpecularColor = 0.6 * sunLight.vColor * fSpecularIntensity * vec3(texture2D(mat.specular, vTexCoord));
	vSpecularColor = clamp(vSpecularColor, 0.0f, 1.0f);

	outputColor = vec4(vAmbientColor + vDiffuseColor + vSpecularColor, 1.0f);
}