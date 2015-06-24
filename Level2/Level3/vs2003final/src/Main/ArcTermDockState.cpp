#include "StdAfx.h"
#include ".\arctermdockstate.h"
#include "TermPlan.h"
AFX_STATIC_DATA const TCHAR _afxVisible[] = _T("Visible");
AFX_STATIC_DATA const TCHAR _afxBarSection[] = _T("%s-Bar%d");
AFX_STATIC_DATA const TCHAR _afxSummarySection[] = _T("%s-Summary");
AFX_STATIC_DATA const TCHAR _afxXPos[] = _T("XPos");
AFX_STATIC_DATA const TCHAR _afxYPos[] = _T("YPos");
AFX_STATIC_DATA const TCHAR _afxMRUWidth[] = _T("MRUWidth");
AFX_STATIC_DATA const TCHAR _afxDocking[] = _T("Docking");
AFX_STATIC_DATA const TCHAR _afxMRUDockID[] = _T("MRUDockID");
AFX_STATIC_DATA const TCHAR _afxMRUDockLeftPos[] = _T("MRUDockLeftPos");
AFX_STATIC_DATA const TCHAR _afxMRUDockRightPos[] = _T("MRUDockRightPos");
AFX_STATIC_DATA const TCHAR _afxMRUDockTopPos[] = _T("MRUDockTopPos");
AFX_STATIC_DATA const TCHAR _afxMRUDockBottomPos[] = _T("MRUDockBottomPos");
AFX_STATIC_DATA const TCHAR _afxMRUFloatStyle[] = _T("MRUFloatStyle");
AFX_STATIC_DATA const TCHAR _afxMRUFloatXPos[] = _T("MRUFloatXPos");
AFX_STATIC_DATA const TCHAR _afxMRUFloatYPos[] = _T("MRUFloatYPos");

AFX_STATIC_DATA const TCHAR _afxBarID[] = _T("BarID");
AFX_STATIC_DATA const TCHAR _afxHorz[] = _T("Horz");
AFX_STATIC_DATA const TCHAR _afxFloating[] = _T("Floating");
AFX_STATIC_DATA const TCHAR _afxBars[] = _T("Bars");
AFX_STATIC_DATA const TCHAR _afxScreenCX[] = _T("ScreenCX");
AFX_STATIC_DATA const TCHAR _afxScreenCY[] = _T("ScreenCY");
AFX_STATIC_DATA const TCHAR _afxBar[] = _T("Bar#%d");
CArcTermDockState::CArcTermDockState(void):CDockState()
{

}
CArcTermDockState::~CArcTermDockState(void)
{
	for (int i = 0; i < m_arrBarInfo.GetSize(); i++)
		delete (CArctermControlBarInfo*)m_arrBarInfo[i];
   m_arrBarInfo.RemoveAll();
}
void CArcTermDockState::LoadState(LPCTSTR lpszProfileName)
{
	CTermPlanApp* pApp = (CTermPlanApp*)AfxGetApp();
	TCHAR szSection[256];
	szSection[_countof(szSection)-1] = 0;
	_sntprintf(szSection, _countof(szSection)-1, _afxSummarySection, lpszProfileName);

	int nBars = pApp->GetProfileInt(szSection, _afxBars, 0);

	CSize size;
	size.cx = pApp->GetProfileInt(szSection, _afxScreenCX, 0);
	size.cy = pApp->GetProfileInt(szSection, _afxScreenCY, 0);
	SetScreenSize(size);

	for (int i = 0; i < nBars; i++)
	{
		CArctermControlBarInfo* pInfo = new CArctermControlBarInfo;
		if(pInfo->LoadState(lpszProfileName, i, this))
			m_arrBarInfo.Add(pInfo);
	}
}
void CArcTermDockState::SaveState(LPCTSTR lpszProfileName) 
	{
		int nIndex = 0;
		for (int i = 0;i < m_arrBarInfo.GetSize(); i++)
		{
			CArctermControlBarInfo* pInfo = (CArctermControlBarInfo*)m_arrBarInfo[i];
			ASSERT(pInfo != NULL);
			if (pInfo->SaveState(lpszProfileName, nIndex))
				nIndex++;
		}
		CTermPlanApp* pApp = (CTermPlanApp*)AfxGetApp();
		TCHAR szSection[256];
		szSection[_countof(szSection)-1] = 0;
		_sntprintf(szSection, _countof(szSection)-1, _afxSummarySection, lpszProfileName);
		pApp->WriteProfileInt(szSection, _afxBars, nIndex);

		CSize size = GetScreenSize();
		pApp->WriteProfileInt(szSection, _afxScreenCX, size.cx);
		pApp->WriteProfileInt(szSection, _afxScreenCY, size.cy);
	}
CArctermControlBarInfo::CArctermControlBarInfo(void):CControlBarInfo()
	{

	}
CArctermControlBarInfo::~CArctermControlBarInfo(void)
	{

	}
BOOL CArctermControlBarInfo::LoadState(LPCTSTR lpszProfileName, int nIndex, CDockState* pDockState)
	{
		ASSERT(pDockState != NULL);

		CTermPlanApp* pApp = (CTermPlanApp*)AfxGetApp();
		TCHAR szSection[256];
		szSection[_countof(szSection)-1] = 0;
		_sntprintf(szSection, _countof(szSection)-1, _afxBarSection, lpszProfileName, nIndex);

		m_nBarID = pApp->GetProfileInt(szSection, _afxBarID, 0);
		m_bVisible = (BOOL) pApp->GetProfileInt(szSection, _afxVisible, TRUE);
		m_bHorz = (BOOL) pApp->GetProfileInt(szSection, _afxHorz, TRUE);
		m_bFloating = (BOOL) pApp->GetProfileInt(szSection, _afxFloating, FALSE);
		m_pointPos = CPoint(
			pApp->GetProfileInt(szSection, _afxXPos, -1),
			pApp->GetProfileInt(szSection, _afxYPos, -1));
		pDockState->ScalePoint(m_pointPos);
		if (m_bFloating)
		{
			// multi-monitor support only available under Win98/ME/Win2000 or greater
			OSVERSIONINFO vi;
			ZeroMemory(&vi, sizeof(OSVERSIONINFO));
			vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			::GetVersionEx(&vi);
			if ((vi.dwMajorVersion > 4) || (vi.dwMajorVersion == 4 && vi.dwMinorVersion != 0))
			{
				if (m_pointPos.x - GetSystemMetrics(SM_CXFRAME) < GetSystemMetrics(SM_XVIRTUALSCREEN))
					m_pointPos.x = GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_XVIRTUALSCREEN);
				if (m_pointPos.x + GetSystemMetrics(SM_CXFRAME) > GetSystemMetrics(SM_CXVIRTUALSCREEN))
					m_pointPos.x = - GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXVIRTUALSCREEN);
				if (m_pointPos.y - GetSystemMetrics(SM_CYFRAME) - GetSystemMetrics(SM_CYSMCAPTION) < GetSystemMetrics(SM_YVIRTUALSCREEN))
					m_pointPos.y = GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYSMCAPTION) + GetSystemMetrics(SM_YVIRTUALSCREEN);
				if (m_pointPos.y + GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYSMCAPTION) > GetSystemMetrics(SM_CYVIRTUALSCREEN))
					m_pointPos.y = - GetSystemMetrics(SM_CYFRAME) - GetSystemMetrics(SM_CYSMCAPTION) + GetSystemMetrics(SM_CYVIRTUALSCREEN);

				HMODULE hUser = ::GetModuleHandle(_T("user32.dll"));
				ASSERT(hUser);
				if (hUser != NULL)
				{
					// in case of multiple monitors check if point is in one of monitors
					HMONITOR (WINAPI* pfnMonitorFromPoint)(POINT, DWORD) = NULL;
					(FARPROC&)pfnMonitorFromPoint = ::GetProcAddress(hUser, "MonitorFromPoint");
					if (pfnMonitorFromPoint && !(pfnMonitorFromPoint)(m_pointPos, MONITOR_DEFAULTTONULL))
					{
						m_pointPos.x = GetSystemMetrics(SM_CXFRAME);
						m_pointPos.y = GetSystemMetrics(SM_CYSMCAPTION) + GetSystemMetrics(SM_CYFRAME);
					}
				}
			}
			else
			{
				if (m_pointPos.x - GetSystemMetrics(SM_CXFRAME) < 0)
					m_pointPos.x = GetSystemMetrics(SM_CXFRAME) + 0;
				if (m_pointPos.y - GetSystemMetrics(SM_CYFRAME) - GetSystemMetrics(SM_CYSMCAPTION) < 0)
					m_pointPos.y = GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYSMCAPTION) + 0;
			}
		}

		m_nMRUWidth = pApp->GetProfileInt(szSection, _afxMRUWidth, 32767);
		m_bDocking = pApp->GetProfileInt(szSection, _afxDocking, 0);
		if (m_bDocking)
		{
			m_uMRUDockID = pApp->GetProfileInt(szSection, _afxMRUDockID, 0);

			m_rectMRUDockPos = CRect(
				pApp->GetProfileInt(szSection, _afxMRUDockLeftPos, 0),
				pApp->GetProfileInt(szSection, _afxMRUDockTopPos, 0),
				pApp->GetProfileInt(szSection, _afxMRUDockRightPos, 0),
				pApp->GetProfileInt(szSection, _afxMRUDockBottomPos, 0));
			pDockState->ScaleRectPos(m_rectMRUDockPos);

			m_dwMRUFloatStyle = pApp->GetProfileInt(szSection, _afxMRUFloatStyle, 0);

			m_ptMRUFloatPos = CPoint(
				pApp->GetProfileInt(szSection, _afxMRUFloatXPos, 0),
				pApp->GetProfileInt(szSection, _afxMRUFloatYPos, 0));
			pDockState->ScalePoint(m_ptMRUFloatPos);
		}

		int nBars = pApp->GetProfileInt(szSection, _afxBars, 0);
		for (int i=0; i < nBars; i++)
		{
			TCHAR buf[16];
			wsprintf(buf, _afxBar, i);
			m_arrBarID.Add((void*)(UINT_PTR)pApp->GetProfileInt(szSection, buf, 0));
		}

		return m_nBarID != 0;
	}
	BOOL CArctermControlBarInfo::SaveState(LPCTSTR lpszProfileName, int nIndex)
	{
		TCHAR szSection[256];
		szSection[_countof(szSection)-1] = 0;
		_sntprintf(szSection, _countof(szSection)-1, _afxBarSection, lpszProfileName, nIndex);

		// delete the section
		CTermPlanApp* pApp = (CTermPlanApp*)AfxGetApp();
		pApp->WriteProfileString(szSection, NULL, NULL);

		if (m_bDockBar && m_bVisible && !m_bFloating && m_pointPos.x == -1 &&
			m_pointPos.y == -1 && m_arrBarID.GetSize() <= 1)
		{
			return FALSE;
		}

		pApp->WriteProfileInt(szSection, _afxBarID, m_nBarID);
		if (!m_bVisible)
			pApp->WriteProfileInt(szSection, _afxVisible, m_bVisible);
		if (m_bFloating)
		{
			pApp->WriteProfileInt(szSection, _afxHorz, m_bHorz);
			pApp->WriteProfileInt(szSection, _afxFloating, m_bFloating);
		}
		if (m_pointPos.x != -1)
			pApp->WriteProfileInt(szSection, _afxXPos, m_pointPos.x);
		if (m_pointPos.y != -1)
			pApp->WriteProfileInt(szSection, _afxYPos, m_pointPos.y);
		if (m_nMRUWidth != 32767)
			pApp->WriteProfileInt(szSection, _afxMRUWidth, m_nMRUWidth);
		if (m_bDocking)
		{
			pApp->WriteProfileInt(szSection, _afxDocking, m_bDocking);
			pApp->WriteProfileInt(szSection, _afxMRUDockID, m_uMRUDockID);
			pApp->WriteProfileInt(szSection, _afxMRUDockLeftPos, m_rectMRUDockPos.left);
			pApp->WriteProfileInt(szSection, _afxMRUDockTopPos, m_rectMRUDockPos.top);
			pApp->WriteProfileInt(szSection, _afxMRUDockRightPos, m_rectMRUDockPos.right);
			pApp->WriteProfileInt(szSection, _afxMRUDockBottomPos, m_rectMRUDockPos.bottom);
			pApp->WriteProfileInt(szSection, _afxMRUFloatStyle, m_dwMRUFloatStyle);
			pApp->WriteProfileInt(szSection, _afxMRUFloatXPos, m_ptMRUFloatPos.x);
			pApp->WriteProfileInt(szSection, _afxMRUFloatYPos, m_ptMRUFloatPos.y);
		}

		if (m_arrBarID.GetSize() > 1) //if ==1 then still empty
		{
			pApp->WriteProfileInt(szSection, _afxBars, (int)m_arrBarID.GetSize());
			for (int i = 0; i < m_arrBarID.GetSize(); i++)
			{
				TCHAR buf[16];
				wsprintf(buf, _afxBar, i);
				pApp->WriteProfileInt(szSection, buf, (UINT_PTR)(m_arrBarID[i]));
				
			}
		}
		return TRUE;
	}