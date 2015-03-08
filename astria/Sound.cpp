#include "Sound.h"

CMusic::CMusic()
{
	Music = NULL;
}

bool CMusic::Load(std::string file)
{
	if (Music != NULL) Release();
	Music = Mix_LoadMUS(file.c_str());
	if (Music == NULL)
	{
		Error("Error loading sound file", "Error loading sound file : " + file + "\nError code: " + std::string(Mix_GetError()));
		return false;
	}
	return true;
}

void CMusic::Play(int repetition)
{
	if (Mix_PlayingMusic() == 0)
	{
		Mix_PlayMusic(Music, repetition);
	}
}

void CMusic::PlayFadeIn(int fadetime, int repetition)
{
	if (Mix_PlayingMusic() == 0)
		Mix_FadeInMusic(Music, repetition, fadetime);
}

void CMusic::Rewind()
{
	Mix_RewindMusic();
}

void CMusic::FadeOut(int fadetime)
{
	Mix_FadeOutMusic(fadetime);
}

void CMusic::Stop()
{
	Mix_HaltMusic();
}

void CMusic::Release()
{
	Mix_FreeMusic(Music);
	Music = NULL;
}

void CMusic::SetVolume(int vol)
{
	Mix_VolumeMusic(vol);
}

int CMusic::GetVolume()
{
	return Mix_VolumeMusic(-1);
}

void CMusic::Pause()
{
	if (Mix_PlayingMusic())
	{
		if (!Mix_PausedMusic())
			Mix_PauseMusic();
	}
}

void CMusic::Resume()
{
	if (Mix_PlayingMusic())
	{
		if (Mix_PausedMusic())
			Mix_ResumeMusic();
	}
}

bool CMusic::IsPlaying()
{
	return Mix_PlayingMusic();
}

bool CMusic::IsPaused()
{
	return Mix_PausedMusic();
}

//==================================================================================

CSoundEffect::CSoundEffect()
{

}

bool CSoundEffect::Load(std::string file, int channel)
{
	if (!Sound) Release();
	Sound = Mix_LoadWAV(file.c_str());
	if (Sound == NULL)
	{
		Error("Error loading sound file", "Error loading sound file : " + file + "\nError code: " + std::string(Mix_GetError()));
		return false;
	}
	Channel = channel;
	return true;
}

void CSoundEffect::Play(int repetition)
{
	Mix_PlayChannel(Channel, Sound, repetition);
}

void CSoundEffect::Release()
{
	Mix_FreeChunk(Sound);
	Sound = NULL;
}

void CSoundEffect::SetVolume(int vol)
{
	Mix_VolumeChunk(Sound, vol);
}

void CSoundEffect::SetDistanceEffect(int dist)
{
	Mix_SetDistance(Channel, dist);
}

void CSoundEffect::SetPositionEffect(int angle, int dist)
{
	Mix_SetPosition(Channel, angle, dist);
}

void CSoundEffect::Pause()
{
	Mix_Pause(Channel);
}

void CSoundEffect::Resume()
{
	Mix_Resume(Channel);
}

bool CSoundEffect::IsPlaying()
{
	return Mix_Playing(Channel);
}

bool CSoundEffect::IsPaused()
{
	return Mix_Paused(Channel);
}