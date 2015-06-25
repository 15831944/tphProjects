#pragma once

class SelectedSimulationData
{
public:
	SelectedSimulationData()
		:m_bTerminal(false)
		,m_bAirside(false)
		,m_bLandside(false)
		,m_bOnBoard(false)
	{
	}
	~SelectedSimulationData(void){};

	void SelTerminalModel()		{	m_bTerminal = true;	}
	bool IsTerminalSel() const		{	return m_bTerminal;	}
	void SelAirsideModel()		{	m_bAirside = true;	}
	bool IsAirsideSel() const			{	return m_bAirside;	}
	void SelLandsideModel()		{	m_bLandside = true;	}
	bool IsLandsideSel() const		{	return m_bLandside;	}

	void SelOnBoardModel()		{	m_bOnBoard= true;	}
	bool IsOnBoardSel() const		{	return m_bOnBoard;	}

	void Reset()
	{
		m_bTerminal = false;
		m_bAirside  = false;
		m_bLandside = false;
		m_bOnBoard = false;
	}

protected:
	bool m_bTerminal;
	bool m_bAirside;
	bool m_bLandside;
	bool m_bOnBoard;
};
