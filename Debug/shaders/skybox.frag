#version 330

smooth in vec2 vTexCoord;
//smooth in vec3 vNormal;
out vec4 outColor;

uniform sampler2D gSampler;
uniform vec4 vColor;

uniform float fLightAmbient;
//uniform vec3 vLightDirection;

void main()
{
	float fConstantAtt = 0.3;
	vec4 vTexColor = texture2D(gSampler, vTexCoord);
	vec4 vMixedColor = vTexColor*vColor;

	//vMixedColor *= normalize(dot(vNormal, vLightDirection))*fConstantAtt;

	outColor = vMixedColor*fLightAmbient;
}