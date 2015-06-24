#pragma once
#include "3dscenewnd.h"
#include "AircraftComponentScene.h"

class CAircraftComponent3DSceneWnd :
	public C3DSceneWnd
{
public:
	CAircraftComponent3DSceneWnd(void);
	~CAircraftComponent3DSceneWnd(void);

	CAircraftComponentScene m_theScene;
};
