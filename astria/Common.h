#ifndef _COMMON_H_
#define _COMMON_H_

// Include common headers
#include <cassert>
#include <vector>
#include <string>
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <ctime>

#include <gl\glew.h>
#include <SDL/SDL.h>
#include <SDL\SDL_opengl.h>
#include <SDL\SDL_image.h>
#include <SDL\SDL_ttf.h>
#include <SDL\SDL_mixer.h>
#include <gl\GLU.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "Params.h"

#define NDEBUG

extern std::string HelpText;
extern std::string CreditsText;

//=====================
//	HELPER FUNCTIONS  =
//=====================

//returns a random integer between x and y
inline int RandInt(int x, int y) { return rand() % (y - x + 1) + x; }
//returns a random float between zero and 1
inline double RandFloat() { return (rand()) / (RAND_MAX + 1.0); }
inline double RandFloatInterval(float min, float max) { float interval = max - min;	return min + interval*RandFloat(); }
//returns a random bool
inline bool RandBool(){ if (RandInt(0, 1)) return true;	else return false; }
//returns a random float in the range -1 < n < 1
inline double RandNormal() { return RandFloat() - RandFloat(); }
//Clamp first argument between 2nd and 3rd arguments
inline void Clamp(double &arg, double min, double max) { arg = arg <= min ? min : (arg >= max ? max : arg); }
inline void Clamp(float &arg, float min, float max) { arg = arg <= min ? min : (arg >= max ? max : arg); }
inline void Clamp(int &arg, int min, int max) { arg = arg <= min ? min : (arg >= max ? max : arg); }
//Radian to degree
inline double ToDeg(double radian) { return radian * 180 / CParams::Pi; }

//Log functions
inline void Error(const char* title, const char* text) { MessageBox(NULL, text, title, MB_ICONERROR); }
inline void Error(const char* title, std::string text) { MessageBox(NULL, text.c_str(), title, MB_ICONERROR); }
inline void Error(std::string title, std::string text) { MessageBox(NULL, text.c_str(), title.c_str(), MB_ICONERROR); }
inline void Warning(const char* title, const char* text) { MessageBox(NULL, text, title, MB_ICONWARNING); }
inline void Warning(const char* title, std::string text) { MessageBox(NULL, text.c_str(), title, MB_ICONWARNING); }
inline void Warning(std::string title, std::string text) { MessageBox(NULL, text.c_str(), title.c_str(), MB_ICONWARNING); }


#endif