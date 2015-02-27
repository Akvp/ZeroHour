#version 330

uniform struct Matrices
{
	mat4 mProjection;
	mat4 mModel;
	mat4 mView;                                                                           
	mat4 mNormal;
} matrices;

struct DirectionalLight
{
	vec3 vColor;
	vec3 vDirection;
	float fAmbient;
	float fBrightness;
};

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec3 inTangent;
layout (location = 4) in vec3 inBitangent;


smooth out vec3 vNormal;
smooth out vec2 vTexCoord;
smooth out vec3 vWorldPos;

smooth out vec4 vEyeSpacePos;

out vec3 vEyeDir_tangentspace;
out vec3 vLightDir_tangentspace;

uniform DirectionalLight sunLight;
uniform bool bEnableNormalMap;

void main()
{
	mat4 mMV = matrices.mView*matrices.mModel;						//Matrix multiplication is from left to right
	mat4 mMVP = matrices.mProjection*matrices.mView*matrices.mModel;
  
	vTexCoord = inCoord;

	vEyeSpacePos = mMV*vec4(inPosition, 1.0);
	gl_Position = mMVP*vec4(inPosition, 1.0);

	vNormal = (matrices.mNormal*vec4(inNormal, 1.0)).xyz;
	vWorldPos = (matrices.mModel*vec4(inPosition, 1.0)).xyz;

	vec3 vEyeDir_eyespace = vec3(0,0,0) - vEyeSpacePos.xyz;

	if(bEnableNormalMap)
	{
		mat3 mMV3x3 = mat3(mMV);

		vec3 vNormal_eyespace = mMV3x3 * normalize(vNormal);
		vec3 vTangent_eyespace = mMV3x3 * inTangent;
		vec3 vBitangent_eyespace = mMV3x3 * inBitangent;

		mat3 mTBN = transpose(mat3(vTangent_eyespace, vBitangent_eyespace, vNormal_eyespace));
		vLightDir_tangentspace = normalize(mTBN * sunLight.vDirection);
		vEyeDir_tangentspace = normalize(mTBN * vEyeDir_eyespace);
	}
	else
		vLightDir_tangentspace = vec3(0,0,0);
}