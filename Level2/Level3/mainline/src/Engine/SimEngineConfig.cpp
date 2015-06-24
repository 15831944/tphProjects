#include "StdAfx.h"
#include "SimEngineConfig.h"
#include "Inputs/SimParameter.h"
#include "OnBoard/OnBoardDefs.h"

CSimEngineConfig::CSimEngineConfig(void)
:m_pSimParam(NULL)

//OnBoard Mode
,m_nTileWidth(Deck_tile_width)
,m_nTileHeight(Deck_tile_height)
,m_nTravelsalDirectlyCost(Traversal_directly_cost)
,m_nTravelsalDiagonalCost(Traversal_diagonal_cost)
,m_nUseExtendBarrier(1)
,m_nUseLockinTile(0)
{
}

CSimEngineConfig::CSimEngineConfig( const SelectedSimulationData& other )
:SelectedSimulationData(other)
,m_pSimParam(NULL)

//OnBoard Mode
,m_nTileWidth(Deck_tile_width)
,m_nTileHeight(Deck_tile_height)
,m_nTravelsalDirectlyCost(Traversal_directly_cost)
,m_nTravelsalDiagonalCost(Traversal_diagonal_cost)
,m_nUseExtendBarrier(1)
,m_nUseLockinTile(0)
{

}

CSimEngineConfig::~CSimEngineConfig(void)
{
	TRACE("Singleton object: simEngineConfig() deleted!\n");
}

void CSimEngineConfig::setSimParameter(CSimParameter* pSimParam )
{
	ASSERT( pSimParam != NULL );
	m_pSimParam = pSimParam;
}

long CSimEngineConfig::getMobElementCount()
{
	ASSERT( m_pSimParam != NULL );
	return m_pSimParam->GetPaxCount();
}

bool CSimEngineConfig::isSimALTOModes()const
{
	return isSimAirsideMode() || isSimLandsideMode()|| isSimTerminalMode() || isSimOnBoardMode();
}

bool CSimEngineConfig::isSimAirsideModeAlone()const
{
	return isSimAirsideMode() && !isSimLandsideMode()&& !isSimTerminalMode() && !isSimOnBoardMode();
}

bool CSimEngineConfig::isSimLandsideModeAlone()const
{
	return isSimLandsideMode() && !isSimAirsideMode()&& !isSimTerminalMode() && !isSimOnBoardMode();
}

bool CSimEngineConfig::isSimTerminalModeAlone()const
{
	return isSimTerminalMode() && !isSimAirsideMode()&& !isSimLandsideMode() && !isSimOnBoardMode();
}

bool CSimEngineConfig::isSimOnBoardAlone()const
{
	return isSimOnBoardMode() && !isSimAirsideMode()&& !isSimLandsideMode()&& !isSimTerminalMode();
}
