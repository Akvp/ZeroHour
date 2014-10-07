#ifndef _PARAM_H_
#define _PARAM_H_

#include <fstream>
#include <Windows.h>

using namespace std;

class CParams
{
public:
	static char* File;

	// ===== General parameters ======
	static double Pi;
	static double HalfPi;
	static double TwoPi;

	static char* WindowName;
	static int WindowWidth;
	static int WindowHeight;

	// ===== General properties =====
	static int FoodCount;
	static int InitIndividualCount;

	// ===== Environmental properties =====
	static int WorldMinX;
	static int WorldMaxX;
	static int WorldMinY;
	static int WorldMaxY;
	static int WorldMinZ;
	static int WorldMaxZ;
	static int GroundLevel;

	// ===== Genetic algorithm =====
	static double CrossoverRate;
	static double MutationRate;
	static double MaxPerturbation;	//Maximum amount the genetic algorithm may mutate each weight by
	static double ActivationResponse;	//For tweaking sigmoid function
	static double Bias;	//bias value

	// ===== Individual properties =====
	static int DecayTimeInterval;	//in millisecond
	static int AgeTimeInterval;		//in millisecond


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