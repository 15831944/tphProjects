#include "StdAfx.h"
#include "SimEngineCommunicationLayer.h"
#include "Airside/AirsideSimulation.h"
#include "Landside/LandsideEngine.h"
#include "terminal.h"
#include "OnBoard/OnBoardSimEngine.h"


CSimEngineCommLayer::CSimEngineCommLayer(void)
{
}

CSimEngineCommLayer::~CSimEngineCommLayer(void)
{
	TRACE("Singleton object: simEngineCommLayer() deleted!\n");
}

void CSimEngineCommLayer::setSimEngineModes(AirsideSimulation* pAirsideSimEngine
					   ,Landside_Engine::LandsideEngine* pLandsideSimEngine
					   ,Terminal* pTermSimEngine
					   ,OnBoardSimEngine* pOnBoardSimEngine)
{
	m_pAirsideSimEngine = pAirsideSimEngine;
	m_pLandsideSimEngine = pLandsideSimEngine;
	m_pTerminalSimEngine = pTermSimEngine;
	m_pOnBoardSimEngine = pOnBoardSimEngine;
}
