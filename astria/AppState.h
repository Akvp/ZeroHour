#ifndef _APP_STATE_H_
#define _APP_STATE_H_

#include "Event.h"

class CAppState : public CEvent
{
public:
	CAppState() {}
	virtual ~CAppState() {}

	virtual void OnActivate() = 0;
	virtual void OnDeactivate() = 0;

	virtual void OnLoop() = 0;
	virtual void OnRender() = 0;
};

#endif