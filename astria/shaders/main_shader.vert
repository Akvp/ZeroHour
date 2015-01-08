#version 330

uniform struct Matrices
{
	mat4 mProjection;
	mat4 mModel;
	mat4 mView;                                                                           
	mat4 mNormal;
} matrices;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;

smooth out vec3 vNormal;
smooth out vec2 vTexCoord;
smooth out vec3 vWorldPos;

smooth out vec4 vEyeSpacePos;

void main()
{
	mat4 mMV = matrices.mView*matrices.mModel;						//Matrix multiplication is from left to right
	mat4 mMVP = matrices.mProjection*matrices.mView*matrices.mModel;
  
	vTexCoord = inCoord;

	vEyeSpacePos = mMV*vec4(inPosition, 1.0);
	gl_Position = mMVP*vec4(inPosition, 1.0);

	vNormal = (matrices.mNormal*vec4(inNormal, 1.0)).xyz;
	vWorldPos = (matrices.mModel*vec4(inPosition, 1.0)).xyz;
}