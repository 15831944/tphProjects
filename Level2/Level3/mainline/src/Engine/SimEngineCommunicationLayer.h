#pragma once
#include "ServiceObject.h"

/**
* Class to provide simEngine mode communication functionality in ALTO system.
*/

class AirsideSimulation;
namespace Landside_Engine{ class LandsideEngine; };
class OnBoardSimEngine;
class Terminal;

class CSimEngineCommLayer : public ServiceObject < CSimEngineCommLayer >
{
public:
	CSimEngineCommLayer();
	~CSimEngineCommLayer();

	void setSimEngineModes(AirsideSimulation* pAirsideSimEngine
		,Landside_Engine::LandsideEngine* pLandsideSimEngine
		,Terminal* pTermSimEngine
		,OnBoardSimEngine* pOnBoardSimEngine);

private:
	AirsideSimulation	*m_pAirsideSimEngine;
	Landside_Engine::LandsideEngine		*m_pLandsideSimEngine;
	Terminal			*m_pTerminalSimEngine;
	OnBoardSimEngine	*m_pOnBoardSimEngine;

};

CSimEngineCommLayer* simEngineCommLayer();
AFX_INLINE CSimEngineCommLayer* simEngineCommLayer()
{
	return CSimEngineCommLayer::GetInstance();

}


