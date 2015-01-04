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
	Release();
}

bool CShader::Load(string file, int type)
{
	vector<string> sLines;

	if (!GetLinesFromFile(file, false, &sLines))return false;

	const char** sShader = new const char*[sLines.size()];
	for (int i = 0; i < sLines.size(); i++)sShader[i] = sLines[i].c_str();

	shader = glCreateShader(type);

	glShaderSource(shader, sLines.size(), sShader, NULL);
	glCompileShader(shader);

	delete[] sShader;

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


bool CShader::IsLoaded()
{
	return loaded;
}

GLuint CShader::GetID()
{
	return shader;
}

string CShader::GetFile()
{
	return file;
}

void CShader::Release()
{
	if (!IsLoaded())
		return;

	loaded = false;
	glDeleteShader(shader);
}



CShaderProgram::CShaderProgram()
{
	linked = false;
}

GLuint CShaderProgram::GetID()
{
	return program;
}

bool CShaderProgram::Initiate(int n_arg, ...)
{
	Create();
	va_list shaders;
	va_start(shaders, n_arg);
	for (int i = 0; i < n_arg; i++) 
	{
		if (!AddShader(va_arg(shaders, CShader*)))
		{
			return false;
		}
	}
	va_end(shaders);
	if (!Link())
		return false;
	return true;
}

bool CShaderProgram::Initiate(CShader* vertex, CShader* fragment)
{
	Create();
	if (!AddShader(vertex) || !AddShader(fragment))
		return false;
	if (!Link())
		return false;
	return true;
}

void CShaderProgram::Create()
{
	program = glCreateProgram();
}

bool CShaderProgram::AddShader(CShader* shader)
{
	if (!shader->IsLoaded())
	{
		char errorMsg[512];
		sprintf(errorMsg, "Error! Shader file %s wasn't loaded properly\n", shader->GetFile());
		MessageBox(NULL, errorMsg, "Program error", MB_ICONERROR);
		return false;
	}

	glAttachShader(program, shader->GetID());

	return true;
}

bool CShaderProgram::Link()
{
	glLinkProgram(program);
	int status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	linked = status == GL_TRUE;
	if (!linked)
	{
		char sInfoLog[512];
		char sFinalMessage[1024];
		int iLogLength;
		glGetProgramInfoLog(program, 1024, &iLogLength, sInfoLog);
		sprintf(sFinalMessage, "Error! Program cannot be linked!\nThe compiler returned:\n\n%s", sInfoLog);
		MessageBox(NULL, sFinalMessage, "Program error", MB_ICONERROR);
		return false;
		//MessageBox(NULL, "Program cannot be linked", "Program error", MB_ICONERROR);
	}
	return linked;
}

void CShaderProgram::Release()
{
	if (!linked)
		return;
	linked = false;
	glDeleteProgram(program);
}

void CShaderProgram::Use()
{
	if (linked)
		glUseProgram(program);
}

void CShaderProgram::Interrupt()
{
	glUseProgram(0);
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