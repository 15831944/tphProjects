#pragma once
#include "airsideevent.h"

class CAirsideMobileElement;
class CAirsideMobileElementEvent :
	public AirsideEvent
{
public:
	CAirsideMobileElementEvent(CAirsideMobileElement* mobileElement);
	virtual ~CAirsideMobileElementEvent(void);

	bool IsAirsideMobileEllementEvent(){ return true;}

	CAirsideMobileElement* m_pMobileElement;


	ElapsedTime removeMoveEvent(CAirsideMobileElement *anElement);
	void RemoveAllMobileElementEvent( CAirsideMobileElement * _pElement );

	virtual void addEvent ();
};
