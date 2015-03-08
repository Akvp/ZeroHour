#version 330 

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoords;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec3 inTangent;
layout (location = 4) in vec3 inBitangent;
layout (location = 5) in mat4 inModelMatrix;

struct DirectionalLight
{
	vec3 vColor;
	vec3 vDirection;
	float fAmbient;
	float fBrightness;
};

uniform struct Matrices
{
	mat4 mProjection;
	mat4 mView;                                                                           
} matrices;

smooth out vec3 vNormal;
smooth out vec2 vTexCoord;
smooth out vec3 vWorldPos;

out vec3 vLightDir_tangentspace;

uniform DirectionalLight sunLight;
uniform bool bEnableNormalMap;

void main()
{
	mat4 mMVP = matrices.mProjection * matrices.mView * inModelMatrix;
	vTexCoord = inCoords;

	gl_Position = mMVP*vec4(inPosition, 1.0);

	vNormal = (inModelMatrix*vec4(inNormal, 1.0)).xyz;
	vWorldPos = (matrices.mView * inModelMatrix*vec4(inPosition, 1.0)).xyz;
}