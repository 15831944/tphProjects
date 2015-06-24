#include "StdAfx.h"
#include ".\automaticsavetimer.h"
#include "../AirsideInput/GlobalDatabase.h"

void DoSave(CTermPlanApp* _termPlanApp,PROJECTINFO* proinfo,CString _path)
{
	CMasterDatabase *pMasterDatabase = _termPlanApp->GetMasterDatabase();
	CProjectDatabase projDatabase(PROJMANAGER->GetAppPath());
	CUndoManager* p_undoManger= CUndoManager::GetInStance(_path) ;
	if(p_undoManger == NULL)
		return;

	projDatabase.CloseProject(pMasterDatabase,proinfo->name);
	if(p_undoManger  != NULL)
		p_undoManger->AddNewUndoProject();
	projDatabase.OpenProject(pMasterDatabase,proinfo->name,proinfo->path);
}
CRITICAL_SECTION G_CS ;

void BeginAutoSaveFunc(void* arglist)
{
	CoInitialize(NULL);
	std::vector<void*>* m_argList = (std::vector<void*>*)arglist ;
	HANDLE timerHandle = ((*m_argList)[0]) ;
	CString* Path = (CString*)((*m_argList)[1]) ;
	CTermPlanApp* _termPlanApp = (CTermPlanApp*)(*m_argList)[2] ;
	PROJECTINFO* proinfo = (PROJECTINFO*)(*m_argList)[3] ;
	HANDLE m_event =(HANDLE)( (*m_argList)[4]) ;
    InitializeCriticalSection(&G_CS) ;
	EnterCriticalSection(&G_CS) ;
	while(WaitForSingleObject(m_event,500) != WAIT_TIMEOUT)
	{
		SetEvent(m_event) ;
		WaitForSingleObject(timerHandle,INFINITE) ;
		if(WaitForSingleObject(m_event,500) == WAIT_TIMEOUT)
			break;
		DoSave(_termPlanApp,proinfo,*Path) ;	
		SetEvent(m_event) ;
	}
	LeaveCriticalSection(&G_CS) ;


	CoUninitialize();
}
//_strpath : the path of save path 
//timeintervial : the period of save operation . as the minute , 
CAutomaticSaveTimer::CAutomaticSaveTimer(CString _strpath , int timeInterval,CTermPlanApp* _termPlanApp,PROJECTINFO* _proInfo):m_timeInterval(timeInterval),m_timer(NULL),m_TermPlanApp(_termPlanApp),m_ProInfo(_proInfo)
{

	m_PathName = _strpath ;
	m_timer = CreateWaitableTimer(NULL,FALSE,NULL) ;

	m_Event = CreateEvent(NULL,FALSE,TRUE,NULL) ; //create a event object , 
	m_ArgList.push_back(m_timer) ;
	m_ArgList.push_back(&m_PathName) ;
	m_ArgList.push_back(m_TermPlanApp) ;
	m_ArgList.push_back(_proInfo) ;
	m_ArgList.push_back(m_Event) ;

}
CAutomaticSaveTimer::~CAutomaticSaveTimer(void)
{

	if(m_timer != NULL)
		CloseHandle(m_timer) ;
	CloseHandle(m_Event) ;

}
BOOL CAutomaticSaveTimer::BeginTimer()
{
	LARGE_INTEGER li ;
	li.QuadPart = - (m_timeInterval * 60 * 10000000) ;
	SetWaitableTimer(m_timer,&li,m_timeInterval * 60 * 1000 ,NULL,NULL,FALSE) ;
	 return _beginthread(BeginAutoSaveFunc,0,(void*)&m_ArgList) ;

}
BOOL CAutomaticSaveTimer::EndTimer()
{
	ResetEvent(m_Event) ;
	LARGE_INTEGER li ;
	li.QuadPart = - (1 * 1000000) ;
	return SetWaitableTimer(m_timer,&li,0,NULL,NULL,FALSE) ;
}
//
BOOL CAutomaticSaveTimer::ReSetTimer(int _timerInterval)
{
	LARGE_INTEGER li ;
	li.QuadPart = - (_timerInterval * 60 * 10000000) ;
	return SetWaitableTimer(m_timer,&li,_timerInterval * 60 * 1000 ,NULL,NULL,FALSE) ;
}
