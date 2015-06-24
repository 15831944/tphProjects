#include "StdAfx.h"
#include ".\autosavehandler.h"
#include "../InputAirside/GlobalDatabase.h"
#include "../Main/UndoOptionHander.h"
CAutoSaveHandler::CAutoSaveHandler():m_TermPlanApp(NULL),CAutoSaveHandlerInterface()
{
	CADODatabase::SetAutoSaveHandle(this) ;
}
CAutoSaveHandler::~CAutoSaveHandler()
{

}
CAutoSaveHandler* CAutoSaveHandler::GetInstance() 
{
	static CAutoSaveHandler AutoSaveHandler ;
	return &AutoSaveHandler ;
}
void CAutoSaveHandler::InitalAutoSaveHandler(CTermPlanApp* _TermPlanApp , CString& _strpath  ,CString _strName) 
{
	m_TermPlanApp = _TermPlanApp ;
	m_PathName = _strpath ;
	m_ProjectName = _strName ;
	int length ;
	if(!CUndoOptionHander::ReadData(length,m_timeInterval))
		m_timeInterval = 10 ;
	GetLocalTime(&m_SavedTime) ;
}
BOOL CAutoSaveHandler::IsCanAutoSave()
{
	if(m_timeInterval == 0)
		return FALSE ;
	SYSTEMTIME _currentTime ;
	GetLocalTime(&_currentTime) ;
	int IntervalTime = (_currentTime.wHour*60 + _currentTime.wMinute) - (m_SavedTime.wHour*60+m_SavedTime.wMinute) ;
	if(IntervalTime < m_timeInterval && IntervalTime >= 0 )
		return FALSE ;
	else
	{
		memcpy(&m_SavedTime,&_currentTime,sizeof(SYSTEMTIME)) ;
		return TRUE ;
	}
}
BOOL CAutoSaveHandler::DoAutoSave()
{
	if(IsCanAutoSave())
	{
		if(m_TermPlanApp ==NULL)
			return FALSE ;
		CMasterDatabase *pMasterDatabase = m_TermPlanApp->GetMasterDatabase();
		CProjectDatabase projDatabase(PROJMANAGER->GetAppPath());
		CUndoManager* p_undoManger= CUndoManager::GetInStance(m_PathName) ;
		if(p_undoManger == NULL)
			return FALSE;
		projDatabase.CloseProject(pMasterDatabase,m_ProjectName);
		if(p_undoManger  != NULL)
			p_undoManger->AddNewUndoProject();
		projDatabase.OpenProject(pMasterDatabase,m_ProjectName,m_PathName);
	}
	return TRUE ;
}