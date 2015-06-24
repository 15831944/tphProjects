#pragma once
#include "ServiceObject.h"
#include "../Common/SelectedSimulationData.h"

class CSimParameter;
class CSimEngineConfig : public ServiceObject < CSimEngineConfig >, public SelectedSimulationData
{
public:
	CSimEngineConfig(void);
	CSimEngineConfig(const SelectedSimulationData& other);
	~CSimEngineConfig(void);

	//--------------------------------------------------------------------
	// Global config
public:
	//Mode selection
	bool isSimALTOModes()const;
	bool isSimAirsideModeAlone()const;
	bool isSimLandsideModeAlone()const;
	bool isSimTerminalModeAlone()const;
	bool isSimOnBoardAlone()const;

	bool		isSimAirsideMode()const{ return IsAirsideSel(); }
	void		simAirsideMode(){ SelAirsideModel(); }
	bool		isSimLandsideMode()const{ return IsLandsideSel(); }
	void		simLandsideMode(){ SelLandsideModel();  }
	bool		isSimTerminalMode()const{ return IsTerminalSel(); } 
	void		simTerminalMode(){	SelTerminalModel(); }
	bool		isSimOnBoardMode()const{ return IsOnBoardSel(); } 
	void		simOnBoardMode(){ SelOnBoardModel(); }


private:


public:
	//
	void setSimParameter(CSimParameter* pSimParam);
	long getMobElementCount();

private:
	CSimParameter* m_pSimParam;

//(A)--------------------------------------------------------------------
// Airside config
public:
private:

//(L)--------------------------------------------------------------------
// Landside config
public:	
private:


//(T)--------------------------------------------------------------------
// Terminal config
public:
private:

//(O)--------------------------------------------------------------------
// OnBoard config
public:
	int		getOnBoardModeTileWidth(){ return m_nTileWidth; }
	int		getOnBoardModeTileHeight(){ return m_nTileHeight; }
	void	setOnBoardModeTileWidth(int nTileWidth){ m_nTileWidth = nTileWidth; }
	void	setOnBoardModeTileHeight(int nTileHeight){ m_nTileHeight = nTileHeight; }

	int		getTravelsalDirectlyCost(){ return m_nTravelsalDirectlyCost; }
	int		getTravelsalDiagonalCost(){ return m_nTravelsalDiagonalCost; }
	void	setTravelsalDirectlyCost(int nCost){ m_nTravelsalDirectlyCost = nCost; }
	void	setTravelsalDiagonalCost(int nCost){ m_nTravelsalDiagonalCost = nCost; }

	int		getUseExtendBarrier(){ return m_nUseExtendBarrier; }
	void	setUseExtendBarrier(int nVar){ m_nUseExtendBarrier = nVar; }

	int		getUseLockInTile(){ return m_nUseLockinTile; }
	void	setUseLockInTile(int nVar){ m_nUseLockinTile = nVar; }



private:
	int m_nTileWidth;//cm
	int m_nTileHeight;//cm
	int m_nTravelsalDirectlyCost;
	int m_nTravelsalDiagonalCost;

	int m_nUseExtendBarrier;
	int m_nUseLockinTile;

};


CSimEngineConfig* simEngineConfig();
AFX_INLINE CSimEngineConfig* simEngineConfig()
{
	return CSimEngineConfig::GetInstance();

}










