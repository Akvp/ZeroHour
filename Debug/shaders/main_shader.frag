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
   vec4 vTexColor = texture2D(DiffuseSampler, vTexCoord);
   vec4 vMixedColor = vTexColor*vColor;

   vec4 vSpecColor = texture2D(SpecularSampler, vTexCoord);
   
   vec3 vNormalized = normalize(vNormal);

   vec4 vDiffuseColor = GetDirectionalLightColor(sunLight, vNormalized);
   vec4 vSpecularColor = GetSpecularColor(vWorldPos, vEyePosition, matActive, sunLight, vNormalized);
   
   outputColor = vMixedColor*(vDiffuseColor+vSpecularColor);
}