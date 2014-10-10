#ifndef _PARAM_H_
#define _PARAM_H_

#include <fstream>
#include <Windows.h>

class CParams
{
public:
	static char* File;

	// ===== Constant parameters ======
	static double Pi;
	static double HalfPi;
	static double TwoPi;


	// ===== Graphical options =====
	static char WindowName[64];
	static char VersionNumber[16];
	static int WindowWidth;
	static int WindowHeight;
	static float AspectRatio;

	static char SkyboxFolder[128];
	static char SkyboxTop[64];
	static char SkyboxBot[64];
	static char SkyboxLeft[64];
	static char SkyboxRight[64];
	static char SkyboxFront[64];
	static char SkyboxBack[64];

	// ===== General properties =====
	static float WorldMinX, WorldMaxX;
	static float WorldMinY, WorldMaxY;
	static float WorldMinZ, WorldMaxZ;
	static float GroundLevel;

	// ===== Genetic algorithm =====
	static double CrossoverRate;
	static double MutationRate;
	static double MaxPerturbation;	//Maximum amount the genetic algorithm may mutate each weight by
	static double ActivationResponse;	//For tweaking sigmoid function
	static double Bias;	//bias value

	CParams()
	{
		if (!Load(File))
		{
			char* errormsg;
			sprintf(errormsg, "Cannot find %s", File);
			MessageBox(NULL, errormsg, "Error", MB_ICONERROR);
		}
	}

	static bool Load(char* file);
	static void Store(char* file);
};

#endif