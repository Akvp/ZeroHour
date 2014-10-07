#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdlib.h>
#include <cmath>
#include "Params.h"

//returns a random integer between x and y
inline int RandInt(int x, int y) { return rand() % (y - x + 1) + x; }

//returns a random float between zero and 1
inline double RandFloat() { return (rand()) / (RAND_MAX + 1.0); }

//returns a random bool
inline bool RandBool(){	if (RandInt(0, 1)) return true;	else return false;}

//returns a random float in the range -1 < n < 1
inline double RandNormal() { return RandFloat() - RandFloat(); }

//Clamp first argument between 2nd and 3rd arguments
inline void Clamp(double &arg, double min, double max) { arg = arg <= min ? min : (arg >= max ? max : arg); }

inline double ToDeg(double radian) { return radian * 180 / CParams::Pi; }

#endif