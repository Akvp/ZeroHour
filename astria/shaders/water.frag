#version 330

smooth in vec2 vTexCoord;
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

//Constants
vec4 tangent = vec4(1.0, 0.0, 0.0, 0.0);
vec4 lightNormal = vec4(0.0, 1.0, 0.0, 0.0);
vec4 biTangent = vec4(0.0, 0.0, 1.0, 0.0);

void main()
{
	vec4 vLightDirection_world = normalize(vec4(sunLight.vDirection, 1.0));
	vec4 vLightDirection_tangent = normalize(vec4(dot(vLightDirection_world, tangent), dot(vLightDirection_world, biTangent), dot(vLightDirection_world, lightNormal), 1.0));
	vec4 vNormal = texture2D(normal, vTexCoord);
	float fDiffuseIntensity = clamp(dot(vNormal, vLightDirection_tangent), 0.0f, 1.0f);
	vec4 vDiffuseColor = vec4(sunLight.vColor, 1.0f) * fDiffuseIntensity * vec4(0.1, 0.1, 0.4, 0.8);

	vec3 vReflected = normalize(reflect(vec3(vLightDirection_tangent), vec3(vNormal)));
	vec3 vView = normalize(vEyePosition - vWorldPos);
	float fSpecularIntensity = clamp(dot(vReflected, vView), 0, 1);
	vec4 vSpecularColor = pow(sunLight.fBrightness, 2) * 0.6 * vec4(sunLight.vColor, 1.0f) * fSpecularIntensity;

	outputColor = vDiffuseColor + vSpecularColor;
}