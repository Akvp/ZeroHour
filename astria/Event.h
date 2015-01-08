#ifndef _CEVENT_H_
#define _CEVENT_H_

#include <SDL\SDL.h>

class CEvent {
public:
	CEvent();

	virtual ~CEvent();

	void OnEvent(SDL_Event* Event);

	virtual void OnInputFocus();
	virtual void OnInputBlur(); 

	//Keyboard event
	virtual void OnKeyDown(SDL_Keycode sym, Uint16 mod, SDL_Scancode scancode);
	virtual void OnKeyUp(SDL_Keycode sym,  Uint16 mod, SDL_Scancode scancode);

	//Mouse event
	virtual void OnMouseFocus();
	virtual void OnMouseBlur();
	virtual void OnMouseMove(int mX, int mY, int relX, int relY, bool Left,bool Right,bool Middle);
		
	//Mouse button events
	virtual void OnLButtonDown(int mX, int mY);
	virtual void OnLButtonUp(int mX, int mY);
	virtual void OnRButtonDown(int mX, int mY);
	virtual void OnRButtonUp(int mX, int mY);
	virtual void OnMButtonDown(int mX, int mY);
	virtual void OnMButtonUp(int mX, int mY);

	//Window event
	virtual void OnMaximize();
	virtual void OnMinimize();
	virtual void OnRestore();
	virtual void OnResize(int w,int h);
	virtual void OnExpose();
	virtual void OnGainFocus();
	virtual void OnLoseFocus();

	virtual void OnUser(Uint8 type, int code, void* data1, void* data2);
};

#endif