#ifndef _SHADER_H_
#define _SHADER_H_

#include <glm/gtc/type_ptr.hpp>

#include "Common.h"

using namespace std;

//Shader wrapper class

class CShader
{
public:
	bool Load(string file, int type);
	void Release();

	bool IsLoaded();
	GLuint GetID();

	string GetFile();

	CShader();
	~CShader();

private:
	GLuint shader;	//ID of shader
	string file;
	int type;		//type of shader (e.g. GL_VERTEX_SHADER, GL_FRAGMENT_SHADER)
	bool loaded;	//loaded and compiled

	bool GetLinesFromFile(string sFile, bool bIncludePart, vector<string>* vResult);
};

class CShaderProgram
{
public:
	bool Initiate(int n_arg, ...);
	bool Initiate(CShader* vertex, CShader* fragment);

	void Create();
	void Release();
	
	bool AddShader(CShader* shader);
	bool Link();

	void Use();
	void Interrupt();

	GLuint GetID();

	// Setting vectors
	void SetUniform(string sName, glm::vec2* vVectors, int iCount = 1);
	void SetUniform(string sName, const glm::vec2 vVector);
	void SetUniform(string sName, glm::vec3* vVectors, int iCount = 1);
	void SetUniform(string sName, const glm::vec3 vVector);
	void SetUniform(string sName, glm::vec4* vVectors, int iCount = 1);
	void SetUniform(string sName, const glm::vec4 vVector);

	// Setting floats
	void SetUniform(string sName, float* fValues, int iCount = 1);
	void SetUniform(string sName, const float fValue);

	// Setting 3x3 matrices
	void SetUniform(string sName, glm::mat3* mMatrices, int iCount = 1);
	void SetUniform(string sName, const glm::mat3 mMatrix);

	// Setting 4x4 matrices
	void SetUniform(string sName, glm::mat4* mMatrices, int iCount = 1);
	void SetUniform(string sName, const glm::mat4 mMatrix);

	// Setting integers
	void SetUniform(string sName, int* iValues, int iCount = 1);
	void SetUniform(string sName, const int iValue);

	// Model and normal matrix setting ispretty common
	void SetModelAndNormalMatrix(string sModelMatrixName, string sNormalMatrixName, glm::mat4 mModelMatrix);
	void SetModelAndNormalMatrix(string sModelMatrixName, string sNormalMatrixName, glm::mat4* mModelMatrix);


	CShaderProgram();

private:
	GLuint program;	//ID of program
	bool linked;	//linked and ready to use
};

#endif