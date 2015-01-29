#ifndef _SOUND_H_
#define _SOUND_H_

#include "Common.h"

class CMusic
{
public:
	CMusic();
	bool Load(std::string file);
	void Play();
	void Stop();
	void Release();
private:
	Mix_Music* Music;

};

#endif