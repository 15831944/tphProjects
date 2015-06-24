#pragma once
#include "../InputAirside/ProjectDatabase.h"
#include "TermPlan.h"
#include "../Common/ProjectManager.h"
#include "../Common/UndoManager.h"
#include "../Common/CAutoSaveHandlerInterface.h"
class CTermPlanApp ;
class CMasterDatabase ;
class CUndoManager;
class CAutoSaveHandler:public CAutoSaveHandlerInterface
{
protected:
	CAutoSaveHandler(void);
public:
	virtual ~CAutoSaveHandler(void);
private:
	CTermPlanApp* m_TermPlanApp ;
	int m_timeInterval ; //minute
	CString m_PathName ;
	CString m_ProjectName ;
	SYSTEMTIME  m_SavedTime ;
public:
	static CAutoSaveHandler* GetInstance() ;
	void InitalAutoSaveHandler(CTermPlanApp* _TermPlanApp , CString& _strpath ,CString _strName) ;
	BOOL IsCanAutoSave() ;
	BOOL DoAutoSave() ;
	void SetTimeInterval(int _intervalTime) { m_timeInterval = _intervalTime ;} ;
};
