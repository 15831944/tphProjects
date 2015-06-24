#pragma once

class Terminal;
class OnBoardCandidateFlights;
class OnBoardCarrierSpaces;
class COnBoardPaxInformation;
class OnBoardSimEngineConfig
{
public:
	OnBoardSimEngineConfig(void);
	~OnBoardSimEngineConfig(void);

	void loadConfig(int nPrjID, Terminal* _pInputTerm, const CString& strProjPath);

public:
	// inputs objects
	OnBoardCandidateFlights		*m_pCandidateFlightsInSim;
	OnBoardCarrierSpaces		*m_pCarrierSpaces;
	COnBoardPaxInformation		*m_pAgentInfo;

};
