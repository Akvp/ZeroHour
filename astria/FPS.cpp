#include "FPS.h"

CFPS CFPS::FPSControl;

CFPS::CFPS() 
{
	OldTime     = 0;
	LastTime    = 0;

	SpeedFactor	= 0;

	Frames      = 0;
	NumFrames   = 0;
}

void CFPS::Loop() 
{
	//After each second
	if(OldTime + 1000 < SDL_GetTicks()) 
	{
		//Clear timer
		OldTime = SDL_GetTicks();

		//Get FPS
		NumFrames = Frames;

		//Clear frame count
		Frames = 0;
	}

	SpeedFactor = ((SDL_GetTicks() - LastTime) / 1000.0f) * 32.0f;

	LastTime = SDL_GetTicks();

	Frames++;
}

int CFPS::GetFPS() 
{
    return NumFrames;
}

float CFPS::GetSpeedFactor() 
{
    return SpeedFactor;
}

