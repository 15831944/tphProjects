#pragma once
#include "AirsideObserver.h"
#include "./ARCMobileElement.h"

class AirsideMobElementWriteLogItem;
class ENGINE_TRANSFER CAirsideMobileElement : public ARCMobileElement
{
	DynamicClassStatic(CAirsideMobileElement)
	RegisterInherit(CAirsideMobileElement,ARCMobileElement)
public:
	enum TYPE
	{ 
		AIRSIDE_FLIGHT,
		AIRSIDE_VEHICLE,
	};

	virtual TYPE GetType() = 0 ;



	virtual void WritePureLog(AirsideMobElementWriteLogItem*) = 0;
};
