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

	static char IconImage[128];

	static char IntroImage[128];
	static int IntroDelay;

	static char SkyboxFolder[128];

	// ===== World properties =====
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

	static bool Load(char* file);
	static void Store(char* file);
};

#endif