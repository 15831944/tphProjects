// MathematicView.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "MathematicView.h"
#include "ProcessFlowView.h"
#include "MainFrm.h"

#include "..\common\WinMsg.h"

#include "FlightScheduleDlg.h"
#include "GateAssignDlg.h"
#include "HubDataDlg.h"
#include "PaxTypeDefDlg.h"
#include "FltPaxDataDlg.h"
#include "PaxDistDlg.h"
#include "LeadlagTimeDlg.h"
#include "MobileCountDlg.h"
#include <deque>
#include "PaxBulkListDlg.h"
#include "TermplanDoc.h"
#include "DlgComments.h"

#include "MathProcessDefine.h"
#include "MathProcess.h"
#include "MathSimulation.h"
#include "MathFlowDefineDlg.h"
#include "..\Inputs\TYPELIST.H"
#include "../Common/ARCTracker.h"

#include "MathProcessDataSet.h"
#include "MathFlowDataSet.h"
#include "DlgMathSimSetting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int ID_MATHVIEW_TREECRL = 101;

/////////////////////////////////////////////////////////////////////////////
// CMathematicView

IMPLEMENT_DYNCREATE(CMathematicView, CView)

CMathematicView::CMathematicView()
{
	//{{AFX_DATA_INIT(CMathematicView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pDlgFlow = NULL;

	m_hSelecting = NULL;

	memset((void*)m_hTreeItem, 0, sizeof(m_hTreeItem));

	m_pSubMenu = NULL;

	m_pFlowVect = NULL;
	
	m_pProcVect = NULL;

	m_strOldText = "";

	m_nIterval = 1800;
}

CMathematicView::~CMathematicView()
{
	if(m_pDlgFlow)
		delete m_pDlgFlow;

	if(m_Font.m_hObject)
	{
		m_Font.DeleteObject();
	}
}

BEGIN_MESSAGE_MAP(CMathematicView, CView)
	//{{AFX_MSG_MAP(CMathematicView)
	ON_WM_CREATE()
	ON_COMMAND(IDOK, OnOk)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_NOTIFY(NM_RCLICK, ID_MATHVIEW_TREECRL, OnRclickTreeModels)
	ON_NOTIFY(NM_CLICK, ID_MATHVIEW_TREECRL, OnClickTreeModels)
	ON_WM_DESTROY()
	ON_COMMAND(ID_MMFLIGHTS_COMMENTS, OnMmflightsComments)
	ON_COMMAND(ID_MMFLIGHTS_HELP, OnMmflightsHelp)
	ON_NOTIFY(NM_DBLCLK, ID_MATHVIEW_TREECRL, OnDblclkTreeModels)
	ON_NOTIFY(TVN_SELCHANGING, ID_MATHVIEW_TREECRL, OnSelchangingTreeModels)
	ON_COMMAND(ID_MMPROC_ADD, OnMmprocAdd)
	ON_COMMAND(ID_MMPROC_COMMENTS, OnMmprocComments)
	ON_COMMAND(ID_MMPROC_HELP, OnMmprocHelp)
	ON_COMMAND(ID_MMFLOW_ADD, OnMmflowAdd)
	ON_COMMAND(ID_MMFLOW_COMMENTS, OnMmflowComments)
	ON_COMMAND(ID_MMFLOW_HELP, OnMmflowHelp)
	ON_COMMAND(ID_MMITEM_EDIT, OnMmitemEdit)
	ON_COMMAND(ID_MMITEM_DELETE, OnMmitemDelete)
	ON_COMMAND(ID_MMITEM_COMMENTS, OnMmitemComments)
	ON_COMMAND(ID_MMITEM_HELP, OnMmitemHelp)
	ON_NOTIFY(TVN_BEGINLABELEDIT, ID_MATHVIEW_TREECRL, OnBeginlabeleditTreeModels)
	ON_NOTIFY(TVN_ENDLABELEDIT, ID_MATHVIEW_TREECRL, OnEndlabeleditTreeModels)
	ON_NOTIFY(TVN_BEGINDRAG, ID_MATHVIEW_TREECRL, OnBeginDragTreeModels)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMathematicView drawing

void CMathematicView::OnDraw(CDC* pDC)
{
	
 	CTermPlanDoc* pDoc = GetDocument();
 	// TODO: add draw code here

}

/////////////////////////////////////////////////////////////////////////////
// CMathematicView diagnostics

#ifdef _DEBUG
void CMathematicView::AssertValid() const
{
	CView::AssertValid();
}

void CMathematicView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMathematicView message handlers

int CMathematicView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here

    m_Font.DeleteObject(); 
	m_Font.CreatePointFont(80, _T ("MS Sans Serif"));

	m_treeModels.Create(WS_CHILD|WS_VISIBLE|TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS|TVS_EDITLABELS,
						CRect(0, 0, 0, 0), this, ID_MATHVIEW_TREECRL);
	CBitmap bmp;
	m_ImageList.Create(16, 16, ILC_COLOR8|ILC_MASK, 0, 1);
	bmp.LoadBitmap(IDB_SMALLNODEIMAGES);
	m_ImageList.Add(&bmp, RGB(255,0,255));
	m_treeModels.SetImageList(&m_ImageList, TVSIL_NORMAL);

	// TODO: add draw code here

	return 0;
}

void CMathematicView::OnOk() 
{
	// TODO: Add your command handler code here
	
}

void CMathematicView::OnCancel() 
{
	// TODO: Add your command handler code here
	
}

void CMathematicView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if(m_treeModels.m_hWnd)
	{
		CRect rc, rc2;
		GetClientRect(&rc);
		
		m_treeModels.MoveWindow(0, 0, rc.Width(), rc.Height());
	}
}

void CMathematicView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	// TODO: Add your specialized code here and/or call the base class
	CMainFrame* pFram = (CMainFrame*)AfxGetMainWnd();

	CString strPath = GetDocument()->m_ProjInfo.path + "\\Mathematic";

	CFileFind fd;
	BOOL bFind = fd.FindFile(strPath);
	if(!bFind)//not find the directory, create it
		BOOL b = CreateDirectory(strPath, NULL);

	if(m_pProcVect != NULL)
	{
		if(m_hTreeItem[2] != NULL)
		{
			//delete all items
			while(m_treeModels.ItemHasChildren(m_hTreeItem[2]))
				m_treeModels.DeleteItem(m_treeModels.GetChildItem(m_hTreeItem[2]));

			pFram->m_wndShapesBarPFlow.DeleteAllProcesses();

			//add new items 
			for(int i = 0; i < static_cast<int>(m_pProcVect->size()); i++)
			{
				HTREEITEM hItem = m_treeModels.InsertItem(m_pProcVect->at(i).GetName(), m_hTreeItem[2]);
				if(hItem)
				{
					CString strText = "";
					strText.Format("Number of processors :  %d", m_pProcVect->at(i).GetProcessCount());
					m_treeModels.InsertItem(strText, 0, 0, hItem);
					strText.Format("Server time distribution :  %s", m_pProcVect->at(i).GetProbDistName());
					m_treeModels.InsertItem(strText, 0, 0, hItem);
					strText.Format("Queue discipline(FIFO) :  %s", s_pszQDiscipline[m_pProcVect->at(i).GetQDiscipline()]);
					m_treeModels.InsertItem(strText, 0, 0, hItem);
					strText.Format("Queue configuration :  %s", s_pszQConfig[m_pProcVect->at(i).GetQConfig()]);
					m_treeModels.InsertItem(strText, 0, 0, hItem);

					if(m_pProcVect->at(i).GetCapacity() < 0)				
						strText = "Capacity :  Unlimited";
					else
						strText.Format("Capacity :  %d", m_pProcVect->at(i).GetCapacity());
					m_treeModels.InsertItem(strText, 0, 0, hItem);

					m_treeModels.Expand(hItem, TVE_EXPAND);

					pFram->m_wndShapesBarPFlow.AddNewProcess(m_pProcVect->at(i).GetName());
				}
			}

			m_treeModels.Expand(m_hTreeItem[2], TVE_EXPAND);
		}

		CMathProcessDataSet data;
		data.SetProcesses(m_pProcVect);
		data.saveDataSet(strPath, false);
	}

	if(m_pFlowVect != NULL)
	{
		if(m_hTreeItem[3] != NULL)
		{
			//delete all items
			while(m_treeModels.ItemHasChildren(m_hTreeItem[3]))
				m_treeModels.DeleteItem(m_treeModels.GetChildItem(m_hTreeItem[3]));

			for(int i=0; i < static_cast<int>(m_pFlowVect->size()); i++)
			{
				CString str = (*m_pFlowVect)[i].GetFlowName();
				HTREEITEM hItem = m_treeModels.InsertItem(str, 0, 0, m_hTreeItem[3]);
				
				if(hItem != NULL)
				{
					int iCount = (*m_pFlowVect)[i].GetFlow().size();
					str.Format("Number of process :  %d", iCount);
					m_treeModels.InsertItem(str, 0, 0, hItem);
				
					CString strPaxType(_T("Passenger Type:  "));
					strPaxType += m_pFlowVect->at(i).GetPaxType();
					m_treeModels.InsertItem(strPaxType, 0, 0, hItem);
					

					m_treeModels.Expand(hItem, TVE_EXPAND);
				}				
			}

			m_treeModels.Expand(m_hTreeItem[3], TVE_EXPAND);

		}
			
		CMathFlowDataSet dataFlow;
		dataFlow.SetFlows(m_pFlowVect);
		dataFlow.saveDataSet(strPath, false);
	}
}

void CMathematicView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();

	if(m_treeModels.m_hWnd)
	{
		CRect rc;
		GetClientRect(&rc);
		m_treeModels.MoveWindow(&rc);
	}

	InitTreeModels();

	m_nMenu.LoadMenu(IDR_CTX_MATHEMATIC);
}

BOOL CMathematicView::InitTreeModels()
{
	//add passengers node to root
	CString s = "";

	//add flights node to root
	s.LoadString(IDS_TVNN_FLIGHTS);
	m_hTreeItem[0] = m_treeModels.InsertItem(s, 0, 0);
		s.LoadString(IDS_TVNN_SCHEDULE);
		m_treeModels.InsertItem(s, 0, 0, m_hTreeItem[0]);
		s.LoadString(IDS_TVNN_DELAYS);
		m_treeModels.InsertItem(s, 0, 0, m_hTreeItem[0]);
		s.LoadString(IDS_TVNN_LOADFACTORS);
		m_treeModels.InsertItem(s, 0, 0, m_hTreeItem[0]);
		s.LoadString(IDS_TVNN_ACCAPACITIES);
		m_treeModels.InsertItem(s, 0, 0, m_hTreeItem[0]);

		s.LoadString(IDS_TVNN_GATE_ASSIGNMENT);
		HTREEITEM hGateAssignment = m_treeModels.InsertItem(s, 0, 0, m_hTreeItem[0]);
		{
			s.LoadString(IDS_TVNN_STAND_GATE_ASSIGNMENT);
			m_treeModels.InsertItem(s, 0, 0, hGateAssignment);
			s.LoadString(IDS_TVNN_ARRIVAL_GATE_ASSIGNMENT);
			m_treeModels.InsertItem(s, 0, 0, hGateAssignment);
			s.LoadString(IDS_TVNN_DEPARTURE_GATE_ASSIGNMENT);
			m_treeModels.InsertItem(s, 0, 0, hGateAssignment);
		}

		s.LoadString(IDS_TVNN_HUBBINGDATA);
		m_treeModels.InsertItem(s, 0, 0, m_hTreeItem[0]);
	m_treeModels.Expand(m_hTreeItem[0], TVE_EXPAND);

	s.LoadString(IDS_TVNN_PASSENGERS);
	m_hTreeItem[1] = m_treeModels.InsertItem(s);
		s.LoadString(IDS_TVNN_PAXNAMES);
		m_treeModels.InsertItem(s, 0, 0, m_hTreeItem[1]);
		s.LoadString(IDS_TVNN_PAXDISTRIBUTIONS);
		m_treeModels.InsertItem(s, 0, 0, m_hTreeItem[1]);
		s.LoadString(IDS_TVNN_MOBILECOUNT);
		m_treeModels.InsertItem(s, 0, 0, m_hTreeItem[1]);
		s.LoadString(IDS_TVNN_PAXSPEED);
		m_treeModels.InsertItem(s, 0, 0, m_hTreeItem[1]);
		
		s.LoadString(IDS_TVNN_PAXLLDIST);
		HTREEITEM h = m_treeModels.InsertItem(s, 0, 0, m_hTreeItem[1]);
			s.LoadString(IDS_TVNN_PAXDISTPROFILE);
			m_treeModels.InsertItem(s, 0, 0, h);
			s.LoadString(IDS_TVNN_PAXBULK);
			m_treeModels.InsertItem(s, 0, 0, h);
		m_treeModels.Expand(h, TVE_EXPAND);
	
		s.LoadString(IDS_TVNN_PAXGROUPSIZE);
		m_treeModels.InsertItem(s, 0, 0, m_hTreeItem[1]);
	m_treeModels.Expand(m_hTreeItem[1], TVE_EXPAND);

	s = "Processes";
	m_hTreeItem[2] = m_treeModels.InsertItem(s, 0, 0);

	s = "Flow";
	m_hTreeItem[3] = m_treeModels.InsertItem(s, 0, 0);

	s = "Simulation";
	HTREEITEM hSimItem = m_treeModels.InsertItem(s, 0, 0);
	s = "Setting";
	m_hTreeItem[4] = m_treeModels.InsertItem(s, 0, 0, hSimItem);
	s = "Run";
	m_hTreeItem[5] = m_treeModels.InsertItem(s, 0, 0, hSimItem);

	return TRUE;
}

void CMathematicView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	// Do not call CFormView::OnPaint() for painting messages
}

InputTerminal* CMathematicView::GetInputTerminal() const
{
    return (InputTerminal*)& ( const_cast<CMathematicView*>( this ) )->GetDocument()->GetTerminal();
}

CTermPlanDoc* CMathematicView::GetDocument()
{
	return (CTermPlanDoc*)m_pDocument;
}

void CMathematicView::OnClickTreeModels(NMHDR* pNMHDR, LRESULT* pResult)
{
}

void CMathematicView::OnRclickTreeModels(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CPoint point, pt;
	GetCursorPos(&point);
	pt = point;
	//CWnd* pWnd = (CWnd*)GetDlgItem(IDC_TREE_MODELS);
	m_treeModels.ScreenToClient(&point);
	
	UINT flags = TVHT_ABOVE;
	HTREEITEM h = m_treeModels.HitTest(point, &flags);
	if(!h)
		return;
	m_treeModels.SelectItem(h);
	CString str = m_treeModels.GetItemText(h);//pTVDispInfo->item.hItem
    // TRACE(str+'\n');

//	m_hSelecting = h;
	
	m_pSubMenu = NULL;
	HTREEITEM hParent = m_treeModels.GetParentItem(h);
	HTREEITEM hPParent = NULL;
	if(hParent != NULL)
		hPParent = m_treeModels.GetParentItem(hParent);
	if(h == m_hTreeItem[0] || h == m_hTreeItem[1]
		|| hParent == m_hTreeItem[0] || hParent == m_hTreeItem[1]
		|| hPParent == m_hTreeItem[0] || hPParent == m_hTreeItem[1])
	{
		m_pSubMenu = DYNAMIC_DOWNCAST(CNewMenu, m_nMenu.GetSubMenu(0));
	}
	else if(h == m_hTreeItem[2])
	{
		m_pSubMenu = DYNAMIC_DOWNCAST(CNewMenu, m_nMenu.GetSubMenu(1));
	}
	else if(h == m_hTreeItem[3])
	{
		m_pSubMenu = DYNAMIC_DOWNCAST(CNewMenu, m_nMenu.GetSubMenu(2));
	}
	else if(hParent == m_hTreeItem[2])
	{
		m_strOldText = m_treeModels.GetItemText(h);
		m_pSubMenu = DYNAMIC_DOWNCAST(CNewMenu, m_nMenu.GetSubMenu(3));		
		m_pSubMenu->EnableMenuItem(0, MF_BYPOSITION | MF_ENABLED);
	}
	else if(hParent == m_hTreeItem[3])
	{
		m_strOldText = m_treeModels.GetItemText(h);
		m_pSubMenu = DYNAMIC_DOWNCAST(CNewMenu, m_nMenu.GetSubMenu(3));		
		m_pSubMenu->EnableMenuItem(0, MF_BYPOSITION | MF_ENABLED);
	}
	
	if(m_pSubMenu)
		m_pSubMenu->TrackPopupMenu(0, pt.x, pt.y, this, NULL);

	*pResult = 0;
}

void CMathematicView::OnDestroy() 
{
	CView::OnDestroy();
	
	// TODO: Add your message handler code here
	if(m_nMenu.m_hMenu)
	{
		m_nMenu.DestroyMenu();

		m_pSubMenu = NULL;
	}
}

void CMathematicView::OnMmflightsComments() 
{
	// TODO: Add your command handler code here
	TCHAR buffer[4097];

	CString sCommentsPath = PROJMANAGER->GetPath( GetDocument()->m_ProjInfo.path, CommentsDir );
	FileManager fileMan;
	if(!fileMan.IsDirectory(sCommentsPath)) {
		if(!fileMan.MakePath(sCommentsPath)) {
			AfxMessageBox("Error: could not create comments directory");
			return;
		}
	}

	//filename = node name (if processor, whole processor name) (if proc group, group name)
	CString str = m_treeModels.GetItemText(m_hSelecting);
    // TRACE(str+'\n');

	CString sFileName;
	sFileName = sCommentsPath + "\\" + str + ".txt";

	FILE* f = NULL;
	f = fopen(sFileName, "r");
	if(f != NULL) { //file exists
		//fill buffer with file contents
		int c = fread(buffer, sizeof(TCHAR), 4096, f);
		buffer[c] = '\0';
		fclose(f);
	}
	else {
		buffer[0] = '\0';
	}

	CDlgComments dlg(buffer);
	dlg.DoModal();

	if(buffer[0] == '\0') {
		//no comments, delete file if it exists
		CFileStatus rStatus;
		if(CFile::GetStatus(sFileName, rStatus))
			CFile::Remove(sFileName);
	}
	else {
		//create or overwrite file
		f = fopen(sFileName, "w");
		fwrite(buffer, sizeof(TCHAR), _tcslen(buffer), f);
		fclose(f);
	}

	/*
	CString sAppPath = AfxGetApp()->GetProfileString("Paths", "AppPath", "C:\\ARCTerm");
	CString sHelpFile = sAppPath + "\\aawhelp.chm::/Preferences.htm";
	HtmlHelp(this->GetSafeHwnd(), sHelpFile, HH_DISPLAY_TOPIC, NULL);
	*/
}

void CMathematicView::OnMmflightsHelp() 
{
	// TODO: Add your command handler code here
	
}

void CMathematicView::OnDblclkTreeModels(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here

 	CPoint point, pt;
 	GetCursorPos(&point);
 	pt = point;
 	//CWnd* pWnd = (CWnd*)GetDlgItem(IDC_TREE_MODELS);
 	m_treeModels.ScreenToClient(&point);
 
 	m_treeModels.SetFocus();
 
 	UINT flags = TVHT_ABOVE;
 	HTREEITEM hItem = m_treeModels.HitTest(point, &flags);
 	if(!hItem)
 		return;
 	m_treeModels.SelectItem(hItem);
 	CString str  = m_treeModels.GetItemText(hItem);
 
	if(m_treeModels.GetParentItem(hItem) == m_hTreeItem[2])
	{
		SetTimer(0, 200, NULL);
	}
	else if(m_treeModels.GetParentItem(hItem) == m_hTreeItem[3])
	{
		//if processflowview not exists, create and show it
		CView* pView = NULL;
		if(ShowProcessFlowView(&pView))
		{
			((CProcessFlowView*)pView)->AddFlowChartPane(str);
		}
		//-----end create and show processflowview
	}
	else if (hItem == m_hTreeItem[4])
	{
		CDlgMathSimSetting dlg;
		dlg.set_Interval(m_nIterval);
		if (dlg.DoModal() == IDOK)
		{
			m_nIterval = dlg.get_Interval();
		}
	}
	else if(hItem == m_hTreeItem[5])
	{
		RunSimulation();
	}
 	//////////////////////////////////////////////////////////////////////////
 	else if((hItem != NULL))
 	{
 		if(str != "")
 		{
 			//:xb
 			CString s;
 
 			//Flight Schedule
 			s.LoadString( IDS_TVNN_SCHEDULE );
 			if(s.Compare(str)==0)
 			{
 				CFlightScheduleDlg dlg( this );
 				dlg.DoModal();
 			}
 			
 			//Flight Delays
 			s.LoadString(IDS_TVNN_DELAYS);
 			if(s.Compare(str)==0)
 			{
 				CFltPaxDataDlg dlg( FLIGHT_DELAYS, this );
 				dlg.DoModal();
 			}
 
 			//Passenger Loads
 			s.LoadString(IDS_TVNN_LOADFACTORS);
 			if(s.Compare(str)==0)
 			{
 				CFltPaxDataDlg dlg( FLIGHT_LOAD_FACTORS, this);
 				dlg.DoModal();
 			}
 
 			//Aircraft Capacity
 			s.LoadString(IDS_TVNN_ACCAPACITIES);
 			if(s.Compare(str)==0)
 			{
 				CFltPaxDataDlg dlg(FLIGHT_AC_CAPACITIES, this);
 				dlg.DoModal();
 			}
 
 			//Gate Assignment
 			//s.LoadString(IDS_TVNN_GATEASSIGNMENTS);
 			//if(s.Compare(str)==0)
 			//{
 			//	////CGateAssignDlg dlg( this );
 			//	////dlg.DoModal();
 			//}//end if compare	
 
 			s.LoadString(IDS_TVNN_HUBBINGDATA);
 			if(s.Compare(str)==0)
 			{
 				CHubDataDlg dlg( this );
 				dlg.DoModal();
 			}//end if compare	
 
 	//---------------------------------------
 			
 			//passenger / name
 			s.LoadString(IDS_TVNN_PAXNAMES);
 			if(s.Compare(str)==0)
 			{
 				CPaxTypeDefDlg dlg( this );
 				dlg.DoModal();
 			}
 
 			//passenger / dist
 			s.LoadString(IDS_TVNN_PAXDISTRIBUTIONS);
 			if(s.Compare(str)==0)
 			{
				CPaxDistDlg dlg( this );
 				dlg.DoModal();
 			}
 
 			//Mobile Element
 			s.LoadString(IDS_TVNN_MOBILECOUNT);
 			if(s.Compare(str)==0)
 			{
 				CMobileCountDlg dlg(this);
 				dlg.DoModal();
 			}
 
 			//Boarding Calls
 			s.LoadString(IDS_TVNN_PAXSPEED);
 			if(s.Compare(str)==0)
 			{
 				CFltPaxDataDlg dlg(PAX_SPEED, this );
 				dlg.DoModal();
 			}
 		
 			//Boarding Calls
 			s.LoadString(IDS_TVNN_PAXDISTPROFILE);
 			if(s.Compare(str)==0)
 			{
 				CLeadLagTimeDlg dlg(this);
 				dlg.DoModal();
 			}
 
 			//Boarding Calls
 			s.LoadString(IDS_TVNN_PAXBULK);
 			if(s.Compare(str)==0)
 			{
 				CPaxBulkListDlg dlg(this);
 				dlg.DoModal();
 			}	
 			
 			//Boarding Calls
 			s.LoadString(IDS_TVNN_PAXGROUPSIZE);
 			if(s.Compare(str)==0)
 			{
 				CFltPaxDataDlg dlg(PAX_GROUP_SIZE, this );
 				dlg.DoModal();
 			}
 		}
 	}
 	
 	*pResult = 0;
}

void CMathematicView::OnSelchangingTreeModels(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	TVITEM tvItem = (TVITEM)pNMTreeView->itemNew;
	m_hSelecting = tvItem.hItem;

	*pResult = 0;
}

BOOL CMathematicView::RunSimulation()
{
	PLACE_METHOD_TRACK_STRING();
	CMainFrame* pFram = (CMainFrame*)AfxGetMainWnd();
	CCompRepLogBar* pWnd = (CCompRepLogBar*)(&(pFram->m_wndCompRepLogBar));
	pWnd->m_pProj = NULL;
	pWnd->SetParentIndex(2);

	pFram->ShowControlBar((CToolBar*) pWnd, TRUE, FALSE);//!bIsShown

	CRect rc1, rc2;
	pFram->m_wndCompRepLogBar.GetWindowRect(&rc1);
	pFram->m_wndCompRepLogBar.GetClientRect(&rc2);
	pFram->ChangeSize(rc1, rc2, 2);
	
	pWnd->DeleteLogText();
//	((CCompareReportDoc*)GetDocument())->Run(this->GetSafeHwnd(), (CCompRepLogBar*)pWnd);
	pWnd->SetProgressPos(0);

	CTermPlanDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	for (vector<CMathFlow>::iterator iter = pDoc->m_vFlow.begin();
		iter != pDoc->m_vFlow.end(); iter++)
	{
		CMathSimulation mSim(&(*iter));
		mSim.SetTermPlanDoc(pDoc);
		mSim.SetLogWnd(pWnd);
		mSim.SetInterval(m_nIterval);
		mSim.Run();
	}

	return TRUE;
}

void CMathematicView::OnMmprocAdd() 
{
	// TODO: Add your command handler code here
	Terminal* pInTerm = (Terminal*)&GetDocument()->GetTerminal();
	CMathProcessDefine dlg;
	dlg.SetInTerminal(pInTerm);
	dlg.SetProcessVectorPtr(m_pProcVect);
	if (dlg.DoModal() == IDOK)
	{
		CMathProcess proc = dlg.GetMathProcess();
		CString str = proc.GetName();
		str.MakeUpper();
		proc.SetName(str);
		m_pProcVect->push_back(proc);

		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		pFrame->ShowControlBar(&pFrame->m_wndShapesBarPFlow, TRUE, FALSE);
		pFrame->ShapesBarPFlowMsg(TRUE);
		
		//GetDocument()->SetModifiedFlag(TRUE);
		GetDocument()->SaveMathematicData();
		OnUpdate(NULL, 0, NULL);
	}
}

void CMathematicView::OnMmprocComments() 
{
	// TODO: Add your command handler code here
	OnMmflightsComments();
}

void CMathematicView::OnMmprocHelp() 
{
	// TODO: Add your command handler code here
	
}

void CMathematicView::OnMmflowAdd() 
{
	// TODO: Add your command handler code here
/*	int nIndex = 0;
	CString strName = "";
	strName.Format("NEWFLOW%d", nIndex);
	while(HasChild(m_hTreeItem[3], strName))
	{
		strName.Format("NEWFLOW%d", ++nIndex);
	}
*/
	CTermPlanDoc* pDoc = GetDocument();
	ASSERT(pDoc != NULL);
	CMathFlow flow;
	CMathFlowDefineDlg dlg(&flow);
	if (dlg.DoModal() == IDOK)
	{
		m_pFlowVect->push_back(flow);

		//if processflowview not exists, create and show it
		CView* pView = NULL;
		if(ShowProcessFlowView(&pView))
		{
			((CProcessFlowView*)pView)->AddFlowChartPane(flow.GetFlowName());
		}
		//-----end create and show processflowview

		//GetDocument()->SetModifiedFlag(TRUE);
		GetDocument()->SaveMathematicData();
		OnUpdate(NULL, 0, NULL);

/*		HTREEITEM hItem = HasChild(m_hTreeItem[3], flow.GetFlowName());
		if(hItem != NULL)
		{
			m_hSelecting = hItem;
			SetTimer(0, 200, NULL);
		}
*/
		m_treeModels.SetFocus();
	}

}

void CMathematicView::OnMmflowComments() 
{
	// TODO: Add your command handler code here
	OnMmflightsComments();
}

void CMathematicView::OnMmflowHelp() 
{
	// TODO: Add your command handler code here
	
}

void CMathematicView::OnMmitemEdit() 
{
	// TODO: Add your command handler code here
	HTREEITEM hParent = m_treeModels.GetParentItem(m_hSelecting);
	if(hParent == m_hTreeItem[2])//processes item menu
	{
		Terminal* pInTerm = (Terminal*)&GetDocument()->GetTerminal();
		CMathProcessDefine dlg;
		dlg.SetInTerminal(pInTerm);
		dlg.SetProcessVectorPtr(m_pProcVect);
		CString strName = m_treeModels.GetItemText(m_hSelecting);
		int i = 0;
		for( ; i < static_cast<int>(m_pProcVect->size()); i++)
		{
			if(_strnicmp((LPCTSTR)strName, (LPCTSTR)m_pProcVect->at(i).GetName(), strName.GetLength()) == 0)
			{
				dlg.SetMathProcess(m_pProcVect->at(i));
				break;
			}
		}
				
		if (dlg.DoModal() == IDOK)
		{
			CMathProcess proc = dlg.GetMathProcess();

			m_pProcVect->at(i) = proc;

			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			pFrame->ShowControlBar(&pFrame->m_wndShapesBarPFlow, TRUE, FALSE);
			pFrame->ShapesBarPFlowMsg(TRUE);
			
		    //GetDocument()->SetModifiedFlag(TRUE);
	    	GetDocument()->SaveMathematicData();
			OnUpdate(NULL, 0, NULL);
		}
	}
	else if(hParent == m_hTreeItem[3])//flow item
	{
		//CEdit* pEdit = (CEdit*)m_treeModels.EditLabel(m_hSelecting);
		CString strName = m_treeModels.GetItemText(m_hSelecting);
		for( int i = 0; i < static_cast<int>(m_pFlowVect->size()); i++)
		{
			if(_strnicmp((LPCTSTR)strName, (LPCTSTR)m_pFlowVect->at(i).GetFlowName(), strName.GetLength()) == 0)
			{
				CMathFlowDefineDlg dlg(&(m_pFlowVect->at(i)));
				if (dlg.DoModal() == IDOK)
				{
					GetDocument()->SaveMathematicData();
					OnUpdate(NULL, 0, NULL);
				}
				break;
			}
		}
	}
}

void CMathematicView::DeleteItem(const CString& strName)
{
	OnMmitemDelete();
}

void CMathematicView::OnMmitemDelete() 
{
	// TODO: Add your command handler code here
	HTREEITEM hParent = m_treeModels.GetParentItem(m_hSelecting);
	if(hParent == m_hTreeItem[2])//processes item menu
	{
		CString str = m_treeModels.GetItemText(m_hSelecting);
	
		std::vector<CMathProcess>::iterator iter;

		for (iter = m_pProcVect->begin(); iter != m_pProcVect->end(); iter++)
		{
			CString strTemp = iter->GetName();
			strTemp.MakeUpper();
			str.MakeUpper();
			char* str1 = strTemp.GetBuffer(strTemp.GetLength());
			strTemp.ReleaseBuffer();
			char* str2 = str.GetBuffer(str.GetLength());
			str.ReleaseBuffer();
			if(_strnicmp(str1, str2, strlen(str1)) == 0)
			{
				m_pProcVect->erase(iter);
				break;
			}
		}

		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		pFrame->ShowControlBar(&pFrame->m_wndShapesBarPFlow, TRUE, FALSE);
		pFrame->ShapesBarPFlowMsg(TRUE);

		CView* pView = NULL;
		ShowProcessFlowView(&pView);
		((CProcessFlowView*)pView)->DeleteProcess(str);

		//GetDocument()->SetModifiedFlag(TRUE);
		GetDocument()->SaveMathematicData();
		OnUpdate(NULL, 0, NULL);
	}
	else if(hParent == m_hTreeItem[3])//flow item menu
	{
		CString str = m_treeModels.GetItemText(m_hSelecting);

		std::vector<CMathFlow>::iterator iter;
		
		for(iter = m_pFlowVect->begin(); iter != m_pFlowVect->end(); iter++)
		{
			CString strTemp = iter->GetFlowName();
			strTemp.MakeUpper();
			str.MakeUpper();
			char* str1 = strTemp.GetBuffer(strTemp.GetLength());
			strTemp.ReleaseBuffer();
			char* str2 = str.GetBuffer(str.GetLength());
			str.ReleaseBuffer();
			if(_strnicmp(str1, str2, strlen(str1)) == 0)
			{
				//add code to delete Process flow view
				CView* pView = NULL;
				if(ShowProcessFlowView(&pView))
				{
					((CProcessFlowView*)pView)->AddFlowChartPane(str);
				}
				((CProcessFlowView*)pView)->DeleteFlowChartPane(str);
				//-----end create and show processflowview				
				m_pFlowVect->erase(iter);
				break;
			}
		}

		//GetDocument()->SetModifiedFlag(TRUE);
		GetDocument()->SaveMathematicData();
		OnUpdate(NULL, 0, NULL);
	}
}

void CMathematicView::OnMmitemComments() 
{
	// TODO: Add your command handler code here
	OnMmflightsComments();
}

void CMathematicView::OnMmitemHelp() 
{
	// TODO: Add your command handler code here
}

void CMathematicView::OnBeginlabeleditTreeModels(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	TVITEM tvItem = pTVDispInfo->item;
	m_strOldText = tvItem.pszText;//
	// TRACE(m_strOldText);
	HTREEITEM pParent = m_treeModels.GetParentItem(m_hSelecting);
	if (pParent == m_hTreeItem[3] || pParent == m_hTreeItem[2])
	{
		*pResult = FALSE;
		return ;
	}

	*pResult = TRUE;
}

void CMathematicView::OnBeginDragTreeModels(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;
	//////////////////////////////////////////////////////////////////
	CPoint point, pt;
	GetCursorPos(&point);
	pt = point;
	
	m_treeModels.ScreenToClient(&point);
	UINT flags = TVHT_ABOVE;
	HTREEITEM h = m_treeModels.HitTest(point, &flags);
	if(!h || m_treeModels.GetParentItem(h) != m_hTreeItem[2])
		return;

	m_treeModels.SelectItem(h);
	m_hSelecting = h; 
    
	CString strName = m_treeModels.GetItemText(m_hSelecting);
	if(strName == "")
		return ;
	
	CShape tmpShape;
	((CMainFrame*)AfxGetMainWnd())->SendMessage(WM_SLB_SELCHANGED, (WPARAM)(char*)&tmpShape, (LPARAM)(LPCTSTR)strName);
}

void CMathematicView::OnEndlabeleditTreeModels(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	TVITEM tvItem = pTVDispInfo->item;
	if(tvItem.mask != TVIF_TEXT)
		return ;
	
	CString strText = tvItem.pszText;

	strText.MakeUpper();
	m_strOldText.MakeUpper();
	char* str1 = strText.GetBuffer(strText.GetLength());
	strText.ReleaseBuffer();
	char* str2 = m_strOldText.GetBuffer(m_strOldText.GetLength());
	m_strOldText.ReleaseBuffer();
	if(_strnicmp(str1, str2, strlen(str1)) == 0)
		return ;

	HTREEITEM pParent = m_treeModels.GetParentItem(tvItem.hItem);

	if(strText.IsEmpty())
	{
		if(pParent == m_hTreeItem[2])
			MessageBox("Process name can not be empty!", "Warning");
		else if(pParent == m_hTreeItem[3])
			MessageBox("Flow name can not be empty!", "Warning");
		return ;
	}

	if(pParent == m_hTreeItem[2])
	{
		std::vector<CMathProcess>::iterator iter, iterEdit;

		for (iter = m_pProcVect->begin(); iter != m_pProcVect->end(); iter++)
		{
			CString strTemp = iter->GetName();
			strTemp.MakeUpper();
			char* str1 = strTemp.GetBuffer(strTemp.GetLength());
			strTemp.ReleaseBuffer();
			char* str2 = strText.GetBuffer(strText.GetLength());
			strText.ReleaseBuffer();
			char* str3 = m_strOldText.GetBuffer(m_strOldText.GetLength());
			m_strOldText.ReleaseBuffer();
		
			if(_strnicmp(str1, str2, strlen(str1)) == 0)
			{
				MessageBox("A process with the specified name already exists!", "Warning");
				return ;
			}
			if(_strnicmp(str1, str3, strlen(str1)) == 0)
			{
				iterEdit = iter;
			}
		}
		
		if(&(*iterEdit))
		{
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			pFrame->m_wndShapesBarPFlow.UpdateProcess(m_strOldText, strText);

			pFrame->ShowControlBar(&pFrame->m_wndShapesBarPFlow, TRUE, FALSE);
			pFrame->ShapesBarPFlowMsg(TRUE);

			CView* pView = NULL;
			ShowProcessFlowView(&pView);
			((CProcessFlowView*)pView)->UpdateProcess(m_strOldText, strText);
		
			iterEdit->SetName(strText);
		}
	}
	else if(pParent == m_hTreeItem[3])
	{
		std::vector<CMathFlow>::iterator iter, iterEdit;
		
		for (iter = m_pFlowVect->begin(); iter != m_pFlowVect->end(); iter++)
		{
			CString strTemp = iter->GetFlowName();
			strTemp.MakeUpper();
			char* str1 = strTemp.GetBuffer(strTemp.GetLength());
			strTemp.ReleaseBuffer();
			char* str2 = strText.GetBuffer(strText.GetLength());
			strText.ReleaseBuffer();
			char* str3 = m_strOldText.GetBuffer(m_strOldText.GetLength());
			m_strOldText.ReleaseBuffer();

			if(_strnicmp(str1, str2, strlen(str1)) == 0)
			{
				MessageBox("A flow with the specified name already exists!", "Warning");
				return ;
			}
			if(_strnicmp(str1, str3, strlen(str1)) == 0)
			{
				iterEdit = iter;
			}
		}
		
		if(&(*iterEdit))
		{
			//if processflowview not exists, create and show it
			CView* pView = NULL;
			if(ShowProcessFlowView(&pView))
			{
				((CProcessFlowView*)pView)->AddFlowChartPane(m_strOldText);
			}
			((CProcessFlowView*)pView)->ModifyFlowChartPane(m_strOldText, strText);
			//-----end create and show processflowview
			iterEdit->SetFlowName(strText);
		}
	}

	//GetDocument()->SetModifiedFlag(TRUE);
	GetDocument()->SaveMathematicData();
	OnUpdate(NULL, 0, NULL);

	*pResult = FALSE;
}

HTREEITEM CMathematicView::HasChild(HTREEITEM hItem, const CString& strText)
{
	BOOL b = m_treeModels.ItemHasChildren(hItem);
	if(!b)
		return NULL;

	HTREEITEM h = m_treeModels.GetChildItem(hItem);
	while(h)
	{
		CString s = m_treeModels.GetItemText(h);
		if(s == strText)
			return h;
		h = m_treeModels.GetNextSiblingItem(h);
	}

	return NULL;
}

void CMathematicView::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent == 0)
	{
		KillTimer(nIDEvent);
		CEdit* pEdit = (CEdit*)m_treeModels.EditLabel(m_hSelecting);
	}
	CView::OnTimer(nIDEvent);
}

BOOL CMathematicView::ShowProcessFlowView(CView** pView)
{
	//if processflowview not exists, create and show it
	CView* view = NULL;
	if(pView == NULL)
		pView = &view;
		
	CTermPlanDoc* pDoc = GetDocument();
	POSITION pos  = pDoc->GetFirstViewPosition();
	while(pos)
	{
		*pView = pDoc->GetNextView(pos);
		if((*pView)->IsKindOf(RUNTIME_CLASS(CProcessFlowView)))
		{
			return TRUE;
		}
	}

	CMDIChildWnd* pNewFrame	= (CMDIChildWnd*)(theApp.m_pProcessFlowTemplate->CreateNewFrame(pDoc, NULL));
	theApp.m_pProcessFlowTemplate->InitialUpdateFrame(pNewFrame, pDoc);

	pos  = pDoc->GetFirstViewPosition();
	while(pos)
	{
		*pView = pDoc->GetNextView(pos);
		if((*pView)->IsKindOf(RUNTIME_CLASS(CProcessFlowView)))
		{
			((CProcessFlowView*)*pView)->SetMathFlowPtr(m_pFlowVect);
			((CProcessFlowView*)*pView)->InitTabWnd();
			break;
		}
	}

	return FALSE;
}

BOOL CMathematicView::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message == WM_SYSKEYDOWN || pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_DELETE && pMsg->hwnd == m_treeModels.m_hWnd)
		{
			OnMmitemDelete();
		}
	}

	return CView::PreTranslateMessage(pMsg);
}

void CMathematicView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	// TODO: Add your specialized code here and/or call the base class
/*	CDocument* pDoc = GetDocument();
	
	CString strTitle = pDoc->GetTitle();
	
	strTitle += "         Modeling Sequence View";

	GetParentFrame()->SetWindowText(strTitle);

	
*/
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void CMathematicView::SetSelectItem(const CString& strName)
{
	if(strName == "")
		return ;

	HTREEITEM h = HasChild(m_hTreeItem[2], strName);
	if(h != NULL)
	{
		m_treeModels.SelectItem(h);
		
		m_hSelecting = h;
	}
}

