#ifndef _SOUND_H_
#define _SOUND_H_

#include "Common.h"
#include "utils.h"

class CMusic
{
public:
	CMusic();
	bool Load(std::string file);
	void Play(int repetition = -1);
	void PlayFadeIn(int fadetime, int repetition = -1);

	void Rewind();

	void FadeOut(int fadetime);
	void Stop();
	void Release();

	//Max volume 128
	void SetVolume(int vol);
	int GetVolume();

	static void Pause();
	static void Resume();
private:
	Mix_Music* Music;
};

class CSoundEffect
{
public:
	CSoundEffect();
	bool Load(std::string file, int channel);
	void Play(int repetition = 0);
	void Release();

	void SetVolume(int vol);

	//Distance effect
	void SetDistanceEffect(int dist);
	//Position effect
	//angle 0 = front, 90 = right
	void SetPositionEffect(int angle, int dist);

	static void PauseAll();
	static void ResumeAll();
private:
	Mix_Chunk* Sound;
	int Channel;
};

#endif