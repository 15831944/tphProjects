// RecordTimerManager.cpp: implementation of the CRecordTimerManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "TermPlanDoc.h"
#include "RecordTimerManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CRecordTimerManager* CRecordTimerManager::m_pInstance = NULL;
CTermPlanDoc* CRecordTimerManager::m_pTPDoc = NULL;
UINT CRecordTimerManager::m_nFPS = 20;
UINT CRecordTimerManager::m_nTimerID = 0;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRecordTimerManager::CRecordTimerManager()
{

}

CRecordTimerManager::~CRecordTimerManager()
{

}

CRecordTimerManager* CRecordTimerManager::GetInstance()
{
	if(m_pInstance == NULL)
		m_pInstance = new CRecordTimerManager();
	return m_pInstance;
}

void CRecordTimerManager::DeleteInstance()
{
	if(m_pInstance != NULL) {
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

void CALLBACK EXPORT CRecordTimerManager::TimerCallbackFunc(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime)
{
	if(m_pTPDoc != NULL) {
		m_pTPDoc->RecordTimerCallback(dwTime);
	}
}

void CRecordTimerManager::EnrollDocument(CTermPlanDoc* pDoc)
{
	m_pTPDoc = pDoc;
}

void CRecordTimerManager::SetFrameRate(UINT fps)
{
	ASSERT(fps > 0 && fps < 30.0);
	m_nFPS = fps;
}

void CRecordTimerManager::StartTimer()
{
	UINT t = static_cast<UINT>((1.0/m_nFPS)*1000);
	m_nTimerID = SetTimer(NULL, 0, t, CRecordTimerManager::TimerCallbackFunc);
}

void CRecordTimerManager::EndTimer()
{
	if(m_nTimerID != 0)
		KillTimer(NULL, m_nTimerID);
}