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

smooth out vec2 vTexCoord;
smooth out vec3 vNormal;
smooth out vec3 vWorldPos;
smooth out vec4 vEyeSpacePos;

void main()
{
	mat4 mMVP = matrices.mProjection*matrices.mView*matrices.mModel;
   
	gl_Position = mMVP * vec4(inPosition, 1.0f);
	vEyeSpacePos = matrices.mView*matrices.mModel*vec4(inPosition, 1.0);
  
	vTexCoord = inCoord;
   
	vec4 vWorldPosLocal = matrices.mModel*vec4(inPosition, 1.0f);
	vWorldPos = vWorldPosLocal.xyz;
}