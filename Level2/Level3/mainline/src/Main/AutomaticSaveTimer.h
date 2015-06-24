#pragma once
#include "../InputAirside/ProjectDatabase.h"
#include "TermPlan.h"
#include "../Common/ProjectManager.h"
#include "../Common/UndoManager.h"
class CTermPlanApp ;
class CMasterDatabase ;
class CUndoManager;


class CAutomaticSaveTimer
{
public:
	CAutomaticSaveTimer(CString _strpath , int timeInterval,CTermPlanApp* _termPlanApp,PROJECTINFO* _proInfo);
	~CAutomaticSaveTimer(void);
protected:
	int m_timeInterval ; //minute
	CString m_PathName ;
	HANDLE m_timer ;
	HANDLE m_Event ;
    CTermPlanApp* m_TermPlanApp ;
	PROJECTINFO*  m_ProInfo ; 
	std::vector<void*> m_ArgList ;
public:
	//reset the period time for save operation 
	BOOL ReSetTimer(int _timerInterval) ;
	BOOL BeginTimer() ;
	BOOL EndTimer() ;
};
