// RecordTimerManager.h: interface for the CRecordTimerManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECORDTIMERMANAGER_H__6696A50E_A5DC_4B23_98EE_B5C9270068CB__INCLUDED_)
#define AFX_RECORDTIMERMANAGER_H__6696A50E_A5DC_4B23_98EE_B5C9270068CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define RECORDMGR			CRecordTimerManager::GetInstance()
#define DESTROY_RECORDMGR	CRecordTimerManager::DeleteInstance();

class CTermPlanDoc;

class CRecordTimerManager  
{
protected:
	CRecordTimerManager();
	virtual ~CRecordTimerManager();

	static void CALLBACK EXPORT TimerCallbackFunc(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime);

public:
	static CRecordTimerManager* GetInstance();
	static void DeleteInstance();

	static void EnrollDocument(CTermPlanDoc* pDoc);
	static void SetFrameRate(UINT nFPS);
	static UINT GetFrameRate() { return m_nFPS; }
	static void StartTimer();
	static void EndTimer();

private:
	static CRecordTimerManager* m_pInstance;
	static CTermPlanDoc* m_pTPDoc;
	static UINT m_nFPS;
	static UINT m_nTimerID;


};

#endif // !defined(AFX_RECORDTIMERMANAGER_H__6696A50E_A5DC_4B23_98EE_B5C9270068CB__INCLUDED_)
