#include "Shader.h"
#include <fstream>
#include <string>
#include <sstream>
#include <cstdarg>
#include <Windows.h>

using namespace std;

CShader::CShader()
{
	loaded = false;
}

CShader::~CShader()
{
	release();
}

bool CShader::load(string file, int type)
{
	vector<string> sLines;

	if (!GetLinesFromFile(file, false, &sLines))return false;

	const char** sProgram = new const char*[sLines.size()];
	for (int i = 0; i < sLines.size(); i++)sProgram[i] = sLines[i].c_str();

	shader = glCreateShader(type);

	glShaderSource(shader, sLines.size(), sProgram, NULL);
	glCompileShader(shader);

	delete[] sProgram;

	int iCompilationStatus;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &iCompilationStatus);

	if (iCompilationStatus == GL_FALSE)
	{
		char sInfoLog[512];
		char sFinalMessage[1024];
		int iLogLength;
		glGetShaderInfoLog(shader, 1024, &iLogLength, sInfoLog);
		sprintf(sFinalMessage, "Error! Shader file %s wasn't compiled! The compiler returned:\n\n%s", file.c_str(), sInfoLog);
		MessageBox(NULL, sFinalMessage, "Error", MB_ICONERROR);
		return false;
	}
	this->file = file;
	this->type = type;
	this->loaded = true;

	return true;
}

bool CShader::GetLinesFromFile(string sFile, bool bIncludePart, vector<string>* vResult)
{
	FILE* fp = fopen(sFile.c_str(), "rt");
	if (!fp)return false;

	string sDirectory;
	int slashIndex = -1;
	for (int i = sFile.size() - 1; i >= 0; i--)
	{
		if (sFile[i] == '\\' || sFile[i] == '/')
		{
			slashIndex = i;
			break;
		}
	}

	sDirectory = sFile.substr(0, slashIndex + 1);

	// Get all lines from a file

	char sLine[255];

	bool bInIncludePart = false;

	while (fgets(sLine, 255, fp))
	{
		stringstream ss(sLine);
		string sFirst;
		ss >> sFirst;
		if (sFirst == "#include")
		{
			string sFileName;
			ss >> sFileName;
			if (sFileName.size() > 0 && sFileName[0] == '\"' && sFileName[sFileName.size() - 1] == '\"')
			{
				sFileName = sFileName.substr(1, sFileName.size() - 2);
				GetLinesFromFile(sDirectory + sFileName, true, vResult);
			}
		}
		else if (sFirst == "#include_part")
			bInIncludePart = true;
		else if (sFirst == "#definition_part")
			bInIncludePart = false;
		else if (!bIncludePart || (bIncludePart && bInIncludePart))
			vResult->push_back(sLine);
	}
	fclose(fp);

	return true;
}


bool CShader::isLoaded()
{
	return loaded;
}

GLuint CShader::getID()
{
	return shader;
}

string CShader::getFile()
{
	return file;
}

void CShader::release()
{
	if (!isLoaded())
		return;

	loaded = false;
	glDeleteShader(shader);
}



CShaderProgram::CShaderProgram()
{
	linked = false;
}

bool CShaderProgram::initiate(int n_arg, ...)
{
	create();
	va_list shaders;
	va_start(shaders, n_arg);
	for (int i = 0; i < n_arg; i++) 
	{
		if (!addShader(va_arg(shaders, CShader*)))
		{
			return false;
		}
	}
	va_end(shaders);
	if (!link())
		return false;
	return true;
}

bool CShaderProgram::initiate(CShader* vertex, CShader* fragment)
{
	create();
	if (!addShader(vertex) || !addShader(fragment))
		return false;
	if (!link())
		return false;
	return true;
}

void CShaderProgram::create()
{
	program = glCreateProgram();
}

bool CShaderProgram::addShader(CShader* shader)
{
	if (!shader->isLoaded())
	{
		char errorMsg[512];
		sprintf(errorMsg, "Error! Shader file %s wasn't loaded properly\n", shader->getFile());
		MessageBox(NULL, errorMsg, "Program error", MB_ICONERROR);
		return false;
	}

	glAttachShader(program, shader->getID());

	return true;
}

bool CShaderProgram::link()
{
	glLinkProgram(program);
	int status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	linked = status == GL_TRUE;
	if (!linked)
	{
		MessageBox(NULL, "Program cannot be linked", "Program error", MB_ICONERROR);
	}
	return linked;
}

void CShaderProgram::release()
{
	if (!linked)
		return;
	linked = false;
	glDeleteProgram(program);
}

void CShaderProgram::use()
{
	if (linked)
		glUseProgram(program);
}

GLuint CShaderProgram::operator()()
{
	return program;
}

void CShaderProgram::SetUniform(string sName, float* fValues, int iCount)
{
	int iLoc = glGetUniformLocation(program, sName.c_str());
	glUniform1fv(iLoc, iCount, fValues);
}

void CShaderProgram::SetUniform(string sName, const float fValue)
{
	int iLoc = glGetUniformLocation(program, sName.c_str());
	glUniform1fv(iLoc, 1, &fValue);
}

// Setting vectors

void CShaderProgram::SetUniform(string sName, glm::vec2* vVectors, int iCount)
{
	int iLoc = glGetUniformLocation(program, sName.c_str());
	glUniform2fv(iLoc, iCount, (GLfloat*)vVectors);
}

void CShaderProgram::SetUniform(string sName, const glm::vec2 vVector)
{
	int iLoc = glGetUniformLocation(program, sName.c_str());
	glUniform2fv(iLoc, 1, (GLfloat*)&vVector);
}

void CShaderProgram::SetUniform(string sName, glm::vec3* vVectors, int iCount)
{
	int iLoc = glGetUniformLocation(program, sName.c_str());
	glUniform3fv(iLoc, iCount, (GLfloat*)vVectors);
}

void CShaderProgram::SetUniform(string sName, const glm::vec3 vVector)
{
	int iLoc = glGetUniformLocation(program, sName.c_str());
	glUniform3fv(iLoc, 1, (GLfloat*)&vVector);
}

void CShaderProgram::SetUniform(string sName, glm::vec4* vVectors, int iCount)
{
	int iLoc = glGetUniformLocation(program, sName.c_str());
	glUniform4fv(iLoc, iCount, (GLfloat*)vVectors);
}

void CShaderProgram::SetUniform(string sName, const glm::vec4 vVector)
{
	int iLoc = glGetUniformLocation(program, sName.c_str());
	glUniform4fv(iLoc, 1, (GLfloat*)&vVector);
}

// Setting 3x3 matrices

void CShaderProgram::SetUniform(string sName, glm::mat3* mMatrices, int iCount)
{
	int iLoc = glGetUniformLocation(program, sName.c_str());
	glUniformMatrix3fv(iLoc, iCount, FALSE, (GLfloat*)mMatrices);
}

void CShaderProgram::SetUniform(string sName, const glm::mat3 mMatrix)
{
	int iLoc = glGetUniformLocation(program, sName.c_str());
	glUniformMatrix3fv(iLoc, 1, FALSE, (GLfloat*)&mMatrix);
}

// Setting 4x4 matrices

void CShaderProgram::SetUniform(string sName, glm::mat4* mMatrices, int iCount)
{
	int iLoc = glGetUniformLocation(program, sName.c_str());
	glUniformMatrix4fv(iLoc, iCount, FALSE, (GLfloat*)mMatrices);
}

void CShaderProgram::SetUniform(string sName, const glm::mat4 mMatrix)
{
	int iLoc = glGetUniformLocation(program, sName.c_str());
	glUniformMatrix4fv(iLoc, 1, FALSE, (GLfloat*)&mMatrix);
}

// Setting integers

void CShaderProgram::SetUniform(string sName, int* iValues, int iCount)
{
	int iLoc = glGetUniformLocation(program, sName.c_str());
	glUniform1iv(iLoc, iCount, iValues);
}

void CShaderProgram::SetUniform(string sName, const int iValue)
{
	int iLoc = glGetUniformLocation(program, sName.c_str());
	glUniform1i(iLoc, iValue);
}

void CShaderProgram::SetModelAndNormalMatrix(string sModelMatrixName, string sNormalMatrixName, glm::mat4 mModelMatrix)
{
	SetUniform(sModelMatrixName, mModelMatrix);
	SetUniform(sNormalMatrixName, glm::transpose(glm::inverse(mModelMatrix)));
}

void CShaderProgram::SetModelAndNormalMatrix(string sModelMatrixName, string sNormalMatrixName, glm::mat4* mModelMatrix)
{
	SetUniform(sModelMatrixName, mModelMatrix);
	SetUniform(sNormalMatrixName, glm::transpose(glm::inverse(*mModelMatrix)));
}