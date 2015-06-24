// AnimationTimeManager.cpp: implementation of the CAnimationTimeManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "AnimationTimeManager.h"
#include "TermPlanDoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CAnimationTimeManager* CAnimationTimeManager::m_pInstance = NULL;
CTermPlanDoc* CAnimationTimeManager::m_pTPDoc = NULL;
CView* CAnimationTimeManager::m_p3DView = NULL;
CView* CAnimationTimeManager::m_pDAView = NULL;
double CAnimationTimeManager::m_dFPS = 20.0;
UINT CAnimationTimeManager::m_nTimerID = 0;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAnimationTimeManager::CAnimationTimeManager()
{
}

CAnimationTimeManager::~CAnimationTimeManager()
{
}

CAnimationTimeManager* CAnimationTimeManager::GetInstance()
{
	if(m_pInstance == NULL)
		m_pInstance = new CAnimationTimeManager();
	return m_pInstance;
}

void CAnimationTimeManager::DeleteInstance()
{
	if(m_pInstance != NULL) {
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

void CALLBACK EXPORT CAnimationTimeManager::TimerCallbackFunc(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime)
{
	if(m_pTPDoc != NULL) {
		m_pTPDoc->AnimTimerCallback();
		//if(m_p3DView != NULL)
		//	m_p3DView->Invalidate(FALSE);
		//if(m_pDAView != NULL)
		//	m_pDAView->Invalidate(FALSE);
	}
}

void CAnimationTimeManager::EnrollDocument(CTermPlanDoc* pDoc)
{
	m_pTPDoc = pDoc;
}

void CAnimationTimeManager::Enroll3DView(CView* pView)
{
	m_p3DView = pView;
}

void CAnimationTimeManager::EnrollDAView(CView* pView)
{
	m_pDAView = pView;
}

void CAnimationTimeManager::SetFrameRate(double fps)
{
	ASSERT(fps > 0.0 && fps < 1000.0);
	m_dFPS = fps;
}

void CAnimationTimeManager::StartTimer()
{
	UINT t = static_cast<UINT>((1.0/m_dFPS)*1000);
	m_nTimerID = SetTimer(NULL, 0, t, CAnimationTimeManager::TimerCallbackFunc);
}

void CAnimationTimeManager::EndTimer()
{
	if(m_nTimerID != 0)
		KillTimer(NULL, m_nTimerID);
}