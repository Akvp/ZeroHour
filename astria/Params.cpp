#include "Params.h"

char* CParams::File = "params.ini";

//Set everything before loading the values from .ini file
//In case the file loading fails
double CParams::Pi = 3.14159265358979;
double CParams::HalfPi = Pi / 2;
double CParams::TwoPi = Pi * 2;
char CParams::WindowName[64] = "ASTRIA";
char CParams::VersionNumber[16] = "d0.0.0";
int CParams::WindowWidth = 480;
int CParams::WindowHeight = 360;
float CParams::AspectRatio = 4.0 / 3.0;
char CParams::IconImage[128] = "gfx/img/icon.png";
char CParams::IntroImage[128] = "gfx/img/splash.jpg";
int CParams::IntroDelay = 2000;
char CParams::SkyboxFolder[128] = "/gfx/skybox";
float CParams::WorldMinX = -1000;
float CParams::WorldMaxX =  1000;
float CParams::WorldMinY = -1000;
float CParams::WorldMaxY = 1000;
float CParams::WorldMinZ = -1000;
float CParams::WorldMaxZ = 1000;
float CParams::GroundLevel = 0;
double CParams::ActivationResponse = 0;
double CParams::Bias = 0;
double CParams::CrossoverRate = 0;
double CParams::MutationRate = 0;
double CParams::MaxPerturbation = 0;

//Load the paramets from the file
//Return false if failed to load file
bool CParams::Load(char* file)
{
	std::ifstream inFile(file);

	//Verify file openings
	if (!inFile)
	{
		return false;
	}

	//Load from the file
	char ParamDescription[40];

	inFile >> ParamDescription; inFile.getline(WindowName, 40);
	inFile >> ParamDescription >> VersionNumber;
	inFile >> ParamDescription >> WindowWidth;
	inFile >> ParamDescription >> WindowHeight;
	inFile >> ParamDescription >> IconImage;
	inFile >> ParamDescription >> IntroImage;
	inFile >> ParamDescription >> IntroDelay;
	inFile >> ParamDescription >> SkyboxFolder;
	inFile >> ParamDescription >> WorldMinX >> WorldMaxX;
	inFile >> ParamDescription >> WorldMinY >> WorldMaxY;
	inFile >> ParamDescription >> WorldMinZ >> WorldMaxZ;
	inFile >> ParamDescription >> GroundLevel;
	inFile >> ParamDescription >> ActivationResponse;
	inFile >> ParamDescription >> Bias;
	inFile >> ParamDescription >> CrossoverRate;
	inFile >> ParamDescription >> MutationRate;
	inFile >> ParamDescription >> MaxPerturbation;

	AspectRatio = WindowWidth / WindowHeight;

	return true;
}

void CParams::Store(char* file)
{

}