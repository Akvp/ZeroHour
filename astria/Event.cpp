//==============================================================================
#include "Event.h"

//==============================================================================
CEvent::CEvent() {
}

//------------------------------------------------------------------------------
CEvent::~CEvent() {
	//Do nothing
}

//==============================================================================
void CEvent::OnEvent(SDL_Event* Event) {
	switch(Event->type) {
		case SDL_KEYDOWN: {
			OnKeyDown(Event->key.keysym.sym,Event->key.keysym.mod,Event->key.keysym.scancode);
			break;
		}
				
		case SDL_KEYUP: {
			OnKeyUp(Event->key.keysym.sym,Event->key.keysym.mod,Event->key.keysym.scancode);
			break;
		}

		case SDL_MOUSEMOTION: {
			OnMouseMove(Event->motion.x,Event->motion.y,Event->motion.xrel,Event->motion.yrel,(Event->motion.state&SDL_BUTTON(SDL_BUTTON_LEFT))!=0,(Event->motion.state&SDL_BUTTON(SDL_BUTTON_RIGHT))!=0,(Event->motion.state&SDL_BUTTON(SDL_BUTTON_MIDDLE))!=0);
			break;
		}

		case SDL_MOUSEBUTTONDOWN: {
			switch(Event->button.button) {
				case SDL_BUTTON_LEFT: {
					OnLButtonDown(Event->button.x,Event->button.y);
					break;
				}
				case SDL_BUTTON_RIGHT: {
					OnRButtonDown(Event->button.x,Event->button.y);
					break;
				}
				case SDL_BUTTON_MIDDLE: {
					OnMButtonDown(Event->button.x,Event->button.y);
					break;
				}
			}
			break;
		}

		case SDL_MOUSEBUTTONUP:	{
			switch(Event->button.button) {
				case SDL_BUTTON_LEFT: {
					OnLButtonUp(Event->button.x,Event->button.y);
					break;
				}
				case SDL_BUTTON_RIGHT: {
					OnRButtonUp(Event->button.x,Event->button.y);
					break;
				}
				case SDL_BUTTON_MIDDLE: {
					OnMButtonUp(Event->button.x,Event->button.y);
					break;
				}
			}
			break;
		}

		case SDL_WINDOWEVENT:
		{
			switch (Event->window.event) 
			{
			case SDL_WINDOWEVENT_MINIMIZED:
				OnMinimize();
				break;
			case SDL_WINDOWEVENT_MAXIMIZED:
				OnMaximize();
				break;
			case SDL_WINDOWEVENT_RESTORED:
				OnRestore();
			case SDL_WINDOWEVENT_RESIZED:
				OnResize(Event->window.data1, Event->window.data2);
				break;
			case SDL_WINDOWEVENT_FOCUS_GAINED:
				OnGainFocus();
				break;
			case SDL_WINDOWEVENT_FOCUS_LOST:
				OnLoseFocus();
				break;
			case SDL_WINDOWEVENT_EXPOSED:
				OnExpose();
				break;
			}
		}

		default: {
			OnUser(Event->user.type,Event->user.code,Event->user.data1,Event->user.data2);
			break;
		}
	}
}

//------------------------------------------------------------------------------
void CEvent::OnInputFocus() {
	//Pure virtual, do nothing
}

//------------------------------------------------------------------------------
void CEvent::OnInputBlur() {
	//Pure virtual, do nothing
}

//------------------------------------------------------------------------------
void CEvent::OnKeyDown(SDL_Keycode sym, Uint16 mod, SDL_Scancode scancode) {
	//Pure virtual, do nothing
}

//------------------------------------------------------------------------------
void CEvent::OnKeyUp(SDL_Keycode sym, Uint16 mod, SDL_Scancode scancode) {
	//Pure virtual, do nothing
}

//------------------------------------------------------------------------------
void CEvent::OnMouseFocus() {
	//Pure virtual, do nothing
}

//------------------------------------------------------------------------------
void CEvent::OnMouseBlur() {
	//Pure virtual, do nothing
}

//------------------------------------------------------------------------------
void CEvent::OnMouseMove(int mX, int mY, int relX, int relY, bool Left,bool Right,bool Middle) {
	//Pure virtual, do nothing
}

//------------------------------------------------------------------------------
void CEvent::OnLButtonDown(int mX, int mY) {
	//Pure virtual, do nothing
}

//------------------------------------------------------------------------------
void CEvent::OnLButtonUp(int mX, int mY) {
	//Pure virtual, do nothing
}

//------------------------------------------------------------------------------
void CEvent::OnRButtonDown(int mX, int mY) {
	//Pure virtual, do nothing
}

//------------------------------------------------------------------------------
void CEvent::OnRButtonUp(int mX, int mY) {
	//Pure virtual, do nothing
}

//------------------------------------------------------------------------------
void CEvent::OnMButtonDown(int mX, int mY) {
	//Pure virtual, do nothing
}

//------------------------------------------------------------------------------
void CEvent::OnMButtonUp(int mX, int mY) {
	//Pure virtual, do nothing
}

//------------------------------------------------------------------------------
void CEvent::OnMinimize() {
	//Pure virtual, do nothing
}

//------------------------------------------------------------------------------
void CEvent::OnMaximize() {
	//Pure virtual, do nothing
}

//------------------------------------------------------------------------------
void CEvent::OnRestore() {
	//Pure virtual, do nothing
}

//------------------------------------------------------------------------------
void CEvent::OnResize(int w,int h) {
	//Pure virtual, do nothing
}

//------------------------------------------------------------------------------
void CEvent::OnExpose() {
	//Pure virtual, do nothing
}

//------------------------------------------------------------------------------
void CEvent::OnGainFocus() {
	//Pure virtual, do nothing
}

//------------------------------------------------------------------------------
void CEvent::OnLoseFocus() {
	//Pure virtual, do nothing
}

//------------------------------------------------------------------------------
void CEvent::OnUser(Uint8 type, int code, void* data1, void* data2) {
	//Pure virtual, do nothing
}

//==============================================================================
