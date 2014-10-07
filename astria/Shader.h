#ifndef _SHADER_H_
#define _SHADER_H_

#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>

using namespace std;

//Shader wrapper class

class CShader
{
public:
	bool load(string file, int type);
	void release();

	bool GetLinesFromFile(string sFile, bool bIncludePart, vector<string>* vResult);

	bool isLoaded();
	GLuint getID();

	string getFile();

	CShader();
	~CShader();

private:
	GLuint shader;	//ID of shader
	string file;
	int type;		//type of shader (e.g. GL_VERTEX_SHADER, GL_FRAGMENT_SHADER)
	bool loaded;	//loaded and compiled
};

class CShaderProgram
{
public:
	bool initiate(int n_arg, ...);
	bool initiate(CShader* vertex, CShader* fragment);

	void create();
	void release();
	
	bool addShader(CShader* shader);
	bool link();

	void use();

	GLuint operator()();

	GLuint getID();

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