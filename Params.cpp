#include "Params.h"

char* CParams::File = "param.ini";

//Set everything to zero before loading the values from .ini file
double CParams::Pi = 3.14159265358979;
double CParams::HalfPi = Pi / 2;
double CParams::TwoPi = Pi * 2;
char* CParams::WindowName = "NeuralNetwork";
int CParams::WindowWidth = 480;
int CParams::WindowHeight = 360;
int CParams::FoodCount = 10;
int CParams::InitIndividualCount = 10;
int CParams::WorldMinX = -1000;
int CParams::WorldMaxX =  1000;
int CParams::WorldMinY = -1000;
int CParams::WorldMaxY =  1000;
int CParams::WorldMinZ = -1000;
int CParams::WorldMaxZ =  1000;
int CParams::GroundLevel = 0;
double CParams::ActivationResponse = 0;
double CParams::Bias = 0;
double CParams::CrossoverRate = 0;
double CParams::MutationRate = 0;
double CParams::MaxPerturbation = 0;
int CParams::DecayTimeInterval = 0;
int CParams::AgeTimeInterval = 0;

//Load the paramets from the file
//Return false if failed to load file
bool CParams::Load(char* file)
{
	ifstream inFile(file);

	//Verify file opening
	if (!inFile)
	{
		return false;
	}

	//Load from the file
	char ParamDescription[40];
	inFile >> ParamDescription >> WindowWidth;
	inFile >> ParamDescription >> WindowHeight;
	inFile >> ParamDescription >> FoodCount;
	inFile >> ParamDescription >> InitIndividualCount;
	inFile >> ParamDescription >> WorldMinX >> WorldMaxX;
	inFile >> ParamDescription >> WorldMinY >> WorldMaxY;
	inFile >> ParamDescription >> WorldMinZ >> WorldMaxZ;
	inFile >> ParamDescription >> GroundLevel;
	inFile >> ParamDescription >> ActivationResponse;
	inFile >> ParamDescription >> Bias;
	inFile >> ParamDescription >> CrossoverRate;
	inFile >> ParamDescription >> MutationRate;
	inFile >> ParamDescription >> MaxPerturbation;
	inFile >> ParamDescription >> DecayTimeInterval;
	inFile >> ParamDescription >> AgeTimeInterval;
	inFile >> ParamDescription >> MatingCooldown;

	return true;
}

void CParams::Store(char* file)
{
	
}