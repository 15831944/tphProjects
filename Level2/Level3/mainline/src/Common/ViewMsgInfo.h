#pragma once

#include "ViewMsg.h"

//VM_UPDATE_FLOOR_ALTITUDE
class CRenderFloor;
class FloorAltChangeInfo
{
public:
	int mode;
	CRenderFloor* pFloor;
};