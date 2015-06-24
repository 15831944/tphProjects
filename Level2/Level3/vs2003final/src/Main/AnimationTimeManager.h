// AnimationTimeManager.h: interface for the CAnimationTimeManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANIMATIONTIMEMANAGER_H__0AB49CF6_6568_4365_BD1F_ABE56FB15E41__INCLUDED_)
#define AFX_ANIMATIONTIMEMANAGER_H__0AB49CF6_6568_4365_BD1F_ABE56FB15E41__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTermPlanDoc;

#define ANIMTIMEMGR			CAnimationTimeManager::GetInstance()
#define DESTROY_ANIMTIMEMGR	CAnimationTimeManager::DeleteInstance();

class CAnimationTimeManager  
{
protected:
	CAnimationTimeManager();
	virtual ~CAnimationTimeManager();

	static void CALLBACK EXPORT TimerCallbackFunc(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime);


public:
	static CAnimationTimeManager* GetInstance();
	static void DeleteInstance();

	static void EnrollDocument(CTermPlanDoc* pDoc);
	static void Enroll3DView(CView* pView);
	static void EnrollDAView(CView* pView);
	static void SetFrameRate(double fps);
	static double GetFrameRate() { return m_dFPS; }
	static void StartTimer();
	static void EndTimer();

private:
	static CAnimationTimeManager* m_pInstance;
	static CTermPlanDoc* m_pTPDoc;
	static CView* m_p3DView;
	static CView* m_pDAView;
	static double m_dFPS;
	static UINT m_nTimerID;

};

#endif // !defined(AFX_ANIMATIONTIMEMANAGER_H__0AB49CF6_6568_4365_BD1F_ABE56FB15E41__INCLUDED_)
