#version 330 

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inCoords;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in mat4 inModelMatrix;

uniform struct Matrices
{
	mat4 mProjection;
	mat4 mView;                                                                           
} matrices;

uniform mat4 mModel[50];

smooth out vec3 vNormal;
smooth out vec2 vTexCoord;
smooth out vec3 vWorldPos;

void main()
{
	mat4 mMVP = matrices.mProjection * matrices.mView * inModelMatrix;
	vTexCoord = inCoords;

	gl_Position = mMVP*vec4(inPosition, 1.0);

	vNormal = (inModelMatrix*vec4(inNormal, 1.0)).xyz;
	vWorldPos = (inModelMatrix*vec4(inPosition, 1.0)).xyz;
}