#version 330

smooth in vec2 vTexCoord;
smooth in vec3 vNormal;
smooth in vec4 vEyeSpacePos;
smooth in vec3 vWorldPos;
out vec4 outputColor;

uniform sampler2D DiffuseSampler;
uniform sampler2D SpecularSampler;
uniform vec4 vColor;

#include "dirLight.frag"

uniform DirectionalLight sunLight;
uniform vec3 vEyePosition;

uniform Material matActive;

void main()
{
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glScalef(1.0f, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);

   vec4 vDiffuseColor = texture2D(DiffuseSampler, vTexCoord);
   //vec4 vAmbientColor = vec3(0.1f, 0.1f, 0.1f) * vDiffuseColor;
   //vec4 vSpecularColor = texture2D(SpecularSampler, vTexCoord) * 0.3;

   vec4 vMixedColor = vDiffuseColor*vColor;
   
   vec3 vNormalized = normalize(vNormal);

   vDiffuseColor += GetDirectionalLightColor(sunLight, vNormalized);
   vec4 vSpecularColor = GetSpecularColor(vWorldPos, vEyePosition, matActive, sunLight, vNormalized);

   outputColor = vMixedColor*(vDiffuseColor+vSpecularColor);
   //outputColor = vec4(vTexCoord, 0.0, 1.0);
}