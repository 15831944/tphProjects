#pragma once
#include "Inputs/MobileElemConstraint.h"

class EventList;
class Terminal;
class OnBoardSimEngineConfig;
class CARCportEngine;

class OnBoardSimEngine
{
public:
	OnBoardSimEngine(int nPrjID, CARCportEngine* _pEngine, const CString& strProjPath);
	~OnBoardSimEngine(void);

	friend class Terminal;

public:
	void Initialize();
	int eventListInitialize( EventList* _eventlist);

	long generateAgents();

private:

	int					m_nProjID;
	CString				m_strProjPath;
	//Terminal			*m_pInputTerm;
	CARCportEngine		*m_pEngine;
	OnBoardSimEngineConfig* m_pSimEngineConfig;

};



















