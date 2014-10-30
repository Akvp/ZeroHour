#version 330

smooth in vec2 vTexCoord;
out vec4 outColor;

uniform sampler2D gSampler;
uniform vec4 vColor;

uniform float fLightAmbient;

void main()
{
	vec4 vTexColor = texture2D(gSampler, vTexCoord);
	vec4 vMixedColor = vTexColor*vColor;

	outColor = vMixedColor*fLightAmbient;
}