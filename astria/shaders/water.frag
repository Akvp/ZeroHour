#version 330

smooth in vec2 vTexCoord;
smooth in vec3 vNormal;
smooth in vec3 vWorldPos;
smooth in vec4 vEyeSpacePos;

uniform sampler2D normal;
uniform sampler2D derivative;

struct DirectionalLight
{
	vec3 vColor;
	vec3 vDirection;
	float fAmbient;
	float fBrightness;
};

uniform DirectionalLight sunLight;
uniform vec3 vEyePosition;

out vec4 outputColor;

void main()
{
	vec4 vNormal = texture2D(normal, vTexCoord);
	float fDiffuseIntensity = clamp(dot(vNormal, vec4(-sunLight.vDirection, 1.0f)), 0.0f, 1.0f);
	vec4 vDiffuseColor = vec4(sunLight.vColor, 1.0f) * fDiffuseIntensity * vec4(0.1, 0.1, 0.5, 0.8);

	outputColor = vNormal;
	//outputColor = glm::vec4(0,0,1,1);
}