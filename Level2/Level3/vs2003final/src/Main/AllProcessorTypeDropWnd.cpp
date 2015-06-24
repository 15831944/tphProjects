#include "stdafx.h"
#include "termplan.h"
#include "AllProcessorTypeDropWnd.h"
#include "TermPlanDoc.h"
#include "Inputs\TerminalProcessorTypeSet.h"
#include "Common\ViewMsg.h"
#include "Render3DView.h"
CAllProcessorTypeDropWnd::CAllProcessorTypeDropWnd()
{
}

CAllProcessorTypeDropWnd::~CAllProcessorTypeDropWnd()
{
}

BEGIN_MESSAGE_MAP(CAllProcessorTypeDropWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
	ON_COMMAND(ID_BUT_PIPESHOWALL, OnButPipeshowall)
	ON_COMMAND(ID_BUT_PIPESHOWNO, OnButPipeshowno)
	ON_LBN_SELCHANGE( IDC_PIPESHOWWND_LISTBOX,OnListSelChanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAllProcessorTypeDropWnd message handlers

int CAllProcessorTypeDropWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	if (!m_toolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_toolBar.LoadToolBar(IDR_PIPESHOW_TOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	InitFont(80,"MS Sans Serif");
	if(!m_listBox.Create( LBS_NOTIFY|LBS_HASSTRINGS|WS_CHILD|WS_VISIBLE|WS_BORDER|WS_VSCROLL,
		CRect(0,0,0,0),this,IDC_PIPESHOWWND_LISTBOX) )
	{
		// TRACE("Failed to create checklistbox\n");

		return false;
	}
	m_listBox.SetFont(&m_Font,TRUE) ;
	// TODO: Add your specialized creation code here

	return 0;
}

void CAllProcessorTypeDropWnd::ShowAllProcessorTypeWnd(CPoint point)
{
	InitListBox();
	m_toolBar.MoveWindow(0,-2,PIPEDROPWND_WIDTH,PIPEDROPWND_TOOLBAR_HEIGHT);
	m_listBox.MoveWindow(2,PIPEDROPWND_TOOLBAR_HEIGHT-4,PIPEDROPWND_WIDTH-6,130);
	SetWindowPos(NULL,point.x,point.y,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_SHOWWINDOW );
}

void CAllProcessorTypeDropWnd::OnKillFocus(CWnd* pNewWnd) 
{
	CWnd::OnKillFocus(pNewWnd);

	// TODO: Add your message handler code here
	if(pNewWnd&&pNewWnd->m_hWnd!=m_listBox.m_hWnd)
		ShowWindow(SW_HIDE);

}

void CAllProcessorTypeDropWnd::InitListBox()
{
	m_listBox.ResetContent();

	CMDIChildWnd* pMDIActive = ((CMDIFrameWnd*)AfxGetMainWnd())->MDIGetActive();
	if( pMDIActive== NULL ) return;

	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	if( pDoc== NULL ) return;

	int iFloorCount = pDoc->GetFloorByMode(EnvMode_Terminal).m_vFloors.size();

	std::set<CString> strTypeSet;
	std::map<CString,int>strTypeMap;

	SortProTypeString(strTypeSet,strTypeMap);

	std::set<CString>::const_iterator setIter = strTypeSet.begin();
	for( int i=PointProc; i<= RetailProc; i++ )
	{
		if (setIter != strTypeSet.end())
		{
			char *buff;
			int  nPos;
			CString tempStr(*setIter);
			buff = strstr(tempStr.GetBuffer(),"Processor");
			if (buff)
			{
				nPos = buff - tempStr.GetBuffer();
			}
			else
			{
				nPos = tempStr.GetLength();
			}
			if (strTypeSet.find(tempStr)!= strTypeSet.end())
			{
// 				if( (strTypeMap[tempStr] == StairProc || strTypeMap[tempStr] == EscalatorProc ) && iFloorCount == 1 )
// 				{
// 					++setIter;
// 					continue;
// 				}
				CString strValue = tempStr.Left(nPos);
				strValue.Trim();
				int nIdx = m_listBox.InsertString( -1,strValue );
				m_listBox.SetItemData( nIdx, strTypeMap[tempStr] );
				if (pDoc->GetTerminal().m_pTerminalProcessorTypeSet->m_bProcessorTypeVisiable[strTypeMap[tempStr]])
				{
					m_listBox.SetCheck(nIdx,TRUE);
				}
			}
		}
		++setIter;
	}			
}

void CAllProcessorTypeDropWnd::InitFont(int nPointSize, LPCTSTR lpszFaceName)
{
	m_Font.DeleteObject(); 
	m_Font.CreatePointFont (nPointSize,lpszFaceName);
}

void CAllProcessorTypeDropWnd::OnButPipeshowall() 
{
	// TODO: Add your command handler code here

	CMDIChildWnd* pMDIActive = ((CMDIFrameWnd*)AfxGetMainWnd())->MDIGetActive();
	ASSERT( pMDIActive != NULL );

	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	ASSERT( pDoc!= NULL );
	for (int i = 0; i < 18; i++)
	{
		pDoc->GetTerminal().m_pTerminalProcessorTypeSet->SetProcessorTypeVisiable(i,TRUE);
	}
	if (pDoc->m_systemMode == EnvMode_LandSide)
	{
		pDoc->GetRender3DView()->SendMessage(VM_PROCESSORTYPE_CHECK);
	}
	else if (pDoc->m_systemMode == EnvMode_Terminal || pDoc->m_systemMode == EnvMode_AirSide)
	{
		pDoc->UpdateAllViews( NULL );		
	}
	pDoc->GetTerminal().m_pTerminalProcessorTypeSet->saveDataSet(pDoc->m_ProjInfo.path,false);
	InitListBox();

}

void CAllProcessorTypeDropWnd::OnButPipeshowno() 
{
	// TODO: Add your command handler code here

	CMDIChildWnd* pMDIActive = ((CMDIFrameWnd*)AfxGetMainWnd())->MDIGetActive();
	ASSERT( pMDIActive != NULL );

	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	ASSERT( pDoc!= NULL );
	for (int i = 0; i < 18; i++)
	{
		pDoc->GetTerminal().m_pTerminalProcessorTypeSet->SetProcessorTypeVisiable(i,FALSE);
	}
	if (pDoc->m_systemMode == EnvMode_LandSide)
	{
		pDoc->GetRender3DView()->SendMessage(VM_PROCESSORTYPE_CHECK);
	}
	else if (pDoc->m_systemMode == EnvMode_Terminal || pDoc->m_systemMode == EnvMode_AirSide)
	{
		pDoc->UpdateAllViews( NULL );		
	}
	pDoc->GetTerminal().m_pTerminalProcessorTypeSet->saveDataSet(pDoc->m_ProjInfo.path,false);
	InitListBox();

}

void CAllProcessorTypeDropWnd::OnListSelChanged()
{
	CMDIChildWnd* pMDIActive = ((CMDIFrameWnd*)AfxGetMainWnd())->MDIGetActive();
	ASSERT( pMDIActive != NULL );

	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	ASSERT( pDoc!= NULL );
	int nSel = m_listBox.GetCurSel();
	int ProcessorType = m_listBox.GetItemData(nSel);	
	BOOL bSel = (BOOL)m_listBox.GetCheck( nSel );
	pDoc->GetTerminal().m_pTerminalProcessorTypeSet->SetProcessorTypeVisiable(ProcessorType,bSel);			
	if (pDoc->m_systemMode == EnvMode_LandSide)
	{
		pDoc->GetRender3DView()->SendMessage(VM_PROCESSORTYPE_CHECK);
	}
	else if (pDoc->m_systemMode == EnvMode_Terminal || pDoc->m_systemMode == EnvMode_AirSide)
	{
		pDoc->UpdateAllViews( NULL );		
	}
	pDoc->GetTerminal().m_pTerminalProcessorTypeSet->saveDataSet(pDoc->m_ProjInfo.path,false);
}

void CAllProcessorTypeDropWnd::SortProTypeString( std::set<CString>& proTypeSet,std::map<CString,int>& ProTypeMap )
{
	CMDIChildWnd* pMDIActive = ((CMDIFrameWnd*)AfxGetMainWnd())->MDIGetActive();
	ASSERT( pMDIActive != NULL );

	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	ASSERT( pDoc!= NULL );

	int iFloorCount = pDoc->GetCurModeFloor().m_vFloors.size();
	for (int i = PointProc;i <= RetailProc; i++)
	{
		CString tempStr(PROC_TYPE_NAMES[i]);
		proTypeSet.insert(tempStr);
		ProTypeMap.insert(std::make_pair(tempStr,i));
	}
}
