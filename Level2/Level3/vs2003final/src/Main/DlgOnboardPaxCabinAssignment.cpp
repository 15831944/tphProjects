// DlgOnboardPaxCabinAssignment.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgOnboardPaxCabinAssignment.h"
#include "DlgOnboardFlightList.h"
#include "..\InputOnboard\FlightPaxCabinAssign.h"
#include "..\InputOnboard\AircaftLayOut.h"
#include ".\dlgonboardpaxcabinassignment.h"

// DlgOnboardPaxCabinAssignment dialog

IMPLEMENT_DYNCREATE(DlgOnboardPaxCabinAssignment, CXTResizeDialog)

DlgOnboardPaxCabinAssignment::DlgOnboardPaxCabinAssignment(InputOnboard* pInputOnboard,int nProjID,Terminal* _pTerm,ProjectCommon *pPrjCommon,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(DlgOnboardPaxCabinAssignment::IDD, pParent)

{
	m_pTerminal=_pTerm;
	m_pPrjCommon=pPrjCommon;
	m_onboardPaxCabinAssign.setPrjCommon(pPrjCommon);
	m_nPrjID=nProjID;
	m_listPaxCabin.setTerminal(m_pTerminal);
	m_pInputOnboard=pInputOnboard;
	m_listPaxCabin.setInputOnboard(pInputOnboard);
	m_onboardPaxCabinAssign.setPrjID(m_nPrjID);
}
DlgOnboardPaxCabinAssignment::DlgOnboardPaxCabinAssignment()
	:CXTResizeDialog(DlgOnboardPaxCabinAssignment::IDD, NULL)
{
	m_pTerminal=NULL;
	m_pPrjCommon=NULL;
	m_nPrjID=-1;
}
DlgOnboardPaxCabinAssignment::~DlgOnboardPaxCabinAssignment()
{
}
BEGIN_MESSAGE_MAP(DlgOnboardPaxCabinAssignment, CXTResizeDialog)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_FLIGHT_ADD,OnNewFlight)
	ON_COMMAND(ID_FLIGHT_DEL,OnDelFlight)
	ON_COMMAND(ID_PAXCABIN_ADD,OnNewPaxCabin)
	ON_COMMAND(ID_PAXCABIN_DEL,OnDelPaxCabin)
	ON_BN_CLICKED(IDC_BUTTON_OK,OnButtonOK)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL,OnButtonCancel)
	ON_LBN_SELCHANGE(IDC_LIST_FLIGHT,OnSelChangeFlightList)
 	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PAXCABIN, OnLvnItemchangedListPaxcabin)



END_MESSAGE_MAP()

void DlgOnboardPaxCabinAssignment::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_LIST_FLIGHT,m_listFlight);
	DDX_Control(pDX,IDC_LIST_PAXCABIN,m_listPaxCabin);
}
afx_msg int DlgOnboardPaxCabinAssignment::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	if (!m_toolBarFlight.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_toolBarFlight.LoadToolBar(IDR_ADDDEL_FLIGHT))
	{
		return -1;
	}
	if (!m_toolBarPaxCabin.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_toolBarPaxCabin.LoadToolBar(IDR_ADDDEL_PAXCABIN))
	{
		return -1;
	}
	//m_toolBarPaxCabin.GetToolBarCtrl().SetCmdID(0,ID_TOOLBARBUTTONADD);
	return 0;

}

BOOL DlgOnboardPaxCabinAssignment::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();
	m_onboardPaxCabinAssign.ReadData(-1);
	InitToolBar();
	InitFlightList();
	InitPaxCabinList();

	SetResize(IDC_STATIC_CURBSIDEFLIGHT,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_LIST_FLIGHT,SZ_TOP_LEFT,SZ_BOTTOM_CENTER);
	SetResize(IDC_LIST_PAXCABIN,SZ_TOP_CENTER,SZ_BOTTOM_RIGHT);

	SetResize(IDC_BUTTON_CANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_OK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}
void DlgOnboardPaxCabinAssignment::InitToolBar()
{
	CRect rect;
	m_listFlight.GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.top=rect.top-26;
	rect.left=rect.left+2;
	rect.bottom=rect.top+22;
	rect.right=rect.right-20;
	m_toolBarFlight.MoveWindow(rect);
	m_toolBarFlight.ShowWindow(SW_SHOW);
	m_toolBarFlight.GetToolBarCtrl().EnableButton(ID_FLIGHT_ADD,true);

	m_listPaxCabin.GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.top=rect.top-26;
	rect.left=rect.left+2;
	rect.bottom=rect.top+22;
	m_toolBarPaxCabin.MoveWindow(rect);
	m_toolBarPaxCabin.ShowWindow(SW_SHOW);
	//m_toolBarPaxCabin.
	m_toolBarPaxCabin.GetToolBarCtrl().EnableButton(ID_PAXCABIN_ADD,false);
	m_toolBarPaxCabin.GetToolBarCtrl().EnableButton(ID_PAXCABIN_DEL,false);
		
}
void DlgOnboardPaxCabinAssignment::InitFlightList()
{
	CDlgOnboardFlightList dlg(m_pTerminal);
	CFlightPaxCabinAssign *curAssign;
	for(int i=0;i<(int)m_onboardPaxCabinAssign.GetItemCount();i++)
	{
		curAssign=(CFlightPaxCabinAssign *)m_onboardPaxCabinAssign.GetItem(i);
		COnboardFlight *onboardFlight=curAssign->getonboardFlight();
		CAircaftLayOut* _layout = CAircarftLayoutManager::GetInstance()->GetAircraftLayOutByFlightID(onboardFlight->getID()) ;
		CString strLayout;
		if(_layout != NULL)
			strLayout = _layout->GetName() ;
		int tmpIndex=m_listFlight.InsertString(m_listFlight.GetCount(),dlg.FormatFlightIdNodeForCandidate(strLayout,onboardFlight));
		m_listFlight.SetItemData(tmpIndex,(DWORD)curAssign);
	}
}
void DlgOnboardPaxCabinAssignment::OnUpdatePaxCabinBtn(CCmdUI* pCmdUI)
{
	int curPaxCabinIndex=m_listPaxCabin.GetCurSel();
	if (curPaxCabinIndex<0)
	{
		pCmdUI->Enable(false);
	}else
	{
		pCmdUI->Enable(true);
	}

}
void DlgOnboardPaxCabinAssignment::OnSize(UINT nType, int cx, int cy)
{
	if (!m_listFlight||!m_listPaxCabin)
	{
		return;
	}
	CXTResizeDialog::OnSize(nType,cx,cy);
	CRect rect;
	m_listFlight.GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.top=rect.top-26;
	rect.left=rect.left+2;
	rect.bottom=rect.top+22;
	rect.right=rect.right-20;
	m_toolBarFlight.MoveWindow(rect);

	m_listPaxCabin.GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.top=rect.top-26;
	rect.left=rect.left+2;
	rect.bottom=rect.top+22;
	m_toolBarPaxCabin.MoveWindow(rect);
}
void DlgOnboardPaxCabinAssignment::InitPaxCabinList()
{
	DWORD dwStyle = m_listPaxCabin.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listPaxCabin.SetExtendedStyle(dwStyle);

	CStringList strList;
	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;
	lvc.pszText = _T("Passenger Type");
	lvc.cx = 190;
 	lvc.fmt = LVCFMT_NOEDIT;
	lvc.csList = &strList;
	m_listPaxCabin.InsertColumn(0, &lvc);

	lvc.pszText = _T("Seat Group");
// 	lvc.fmt = LVCFMT_NUMBER;
	m_listPaxCabin.InsertColumn(1, &lvc);

}
CFlightPaxCabinAssign *DlgOnboardPaxCabinAssignment::getCurFlightPaxCabinAssign()
{
	int curFlightIndex=m_listFlight.GetCurSel();
	if (curFlightIndex<0)
	{
		return NULL;
	}
	return (CFlightPaxCabinAssign *)m_listFlight.GetItemData(curFlightIndex);
}
CPaxSeatGroupAssign *DlgOnboardPaxCabinAssignment::getCurPaxSeatGroupAssign()
{
	int curPaxCabinIndex=m_listPaxCabin.GetCurSel();
	if (curPaxCabinIndex<0)
	{
		return NULL;
	}
	return((CPaxSeatGroupAssign *)m_listPaxCabin.GetItemData(curPaxCabinIndex));
}
void DlgOnboardPaxCabinAssignment::OnSelChangeFlightList()
{
	int curFlightIndex=m_listFlight.GetCurSel();
	if (curFlightIndex<0)
	{
		m_toolBarPaxCabin.GetToolBarCtrl().EnableButton(ID_PAXCABIN_ADD,false);
		return;
	}
	m_toolBarPaxCabin.GetToolBarCtrl().EnableButton(ID_PAXCABIN_ADD,true);
	m_toolBarFlight.GetToolBarCtrl().EnableButton(ID_FLIGHT_DEL,true);
	m_listPaxCabin.resetListContent(getCurFlightPaxCabinAssign());
}
void DlgOnboardPaxCabinAssignment::OnSelChangePaxCabinList()
{
	int curPaxCabinIndex=m_listPaxCabin.GetCurSel();
	if (curPaxCabinIndex<0)
	{
		m_toolBarPaxCabin.GetToolBarCtrl().EnableButton(ID_PAXCABIN_DEL,false);
		return;
	}
	m_toolBarPaxCabin.GetToolBarCtrl().EnableButton(ID_PAXCABIN_DEL,true);
}

// DlgOnboardPaxCabinAssignment message handlers
void DlgOnboardPaxCabinAssignment::OnNewFlight()
{
	CDlgOnboardFlightList dlg(m_pTerminal);
	if(dlg.DoModal()==IDOK)
	{
		COnboardFlight *onboardFlight=dlg.getOnboardFlight();

		if (FlightExist(onboardFlight))
		{
			MessageBox("Flight already exist.");
			return;
		}
		CFlightPaxCabinAssign *newAssign=new CFlightPaxCabinAssign();
		newAssign->setonboardFlight(onboardFlight);
		m_onboardPaxCabinAssign.AddItem(newAssign);
        int tmpIndex=m_listFlight.InsertString(m_listFlight.GetCount(),dlg.getFlightString());
		m_listFlight.SetCurSel(tmpIndex);
		m_toolBarFlight.GetToolBarCtrl().EnableButton(ID_FLIGHT_DEL,true);
		m_listFlight.SetItemData(tmpIndex,(DWORD)newAssign);
	}	
}
void DlgOnboardPaxCabinAssignment::OnDelFlight()
{
	int curFlightIndex=m_listFlight.GetCurSel();
	if (curFlightIndex<0)
	{
		return;
	}	
	m_onboardPaxCabinAssign.DeleteItem(getCurFlightPaxCabinAssign());
	m_listFlight.DeleteString(curFlightIndex);	 
	if (curFlightIndex<m_listFlight.GetCount())
	{
		m_listFlight.SetCurSel(curFlightIndex);
		m_listPaxCabin.resetListContent(getCurFlightPaxCabinAssign());
	}else if(m_listFlight.GetCount()>0)
	{
		m_listFlight.SetCurSel(m_listFlight.GetCount()-1);
		m_listPaxCabin.resetListContent(getCurFlightPaxCabinAssign());
	}else
	{
		m_toolBarFlight.GetToolBarCtrl().EnableButton(ID_FLIGHT_DEL,false);
		m_toolBarPaxCabin.GetToolBarCtrl().EnableButton(ID_PAXCABIN_ADD,false);
		m_toolBarPaxCabin.GetToolBarCtrl().EnableButton(ID_PAXCABIN_DEL,false);
		m_listPaxCabin.DeleteAllItems();
	}	
}
void DlgOnboardPaxCabinAssignment::OnNewPaxCabin()
{
	CFlightPaxCabinAssign *curFlightPaxCabinAssign=getCurFlightPaxCabinAssign();
	if (curFlightPaxCabinAssign==NULL)
	{
		return;
	}
	CPassengerType *paxType=new CPassengerType(m_pTerminal->inStrDict);
	CPaxSeatGroupAssign *newAssign=new CPaxSeatGroupAssign(paxType);
	curFlightPaxCabinAssign->AddItem(newAssign);
    m_listPaxCabin.resetListContent(curFlightPaxCabinAssign);
	m_listPaxCabin.SetCurSel(m_listPaxCabin.GetItemCount()-1);
}
void DlgOnboardPaxCabinAssignment::OnDelPaxCabin()
{
	int curPaxCabinIndex=m_listPaxCabin.GetCurSel();
	if (curPaxCabinIndex<0)
	{
		return;
	}
	CFlightPaxCabinAssign *curAssign=getCurFlightPaxCabinAssign();
	if (curAssign!=NULL)
	{
		curAssign->DeleteItem(getCurPaxSeatGroupAssign());
	}
	m_listPaxCabin.DeleteItemEx(curPaxCabinIndex);	
	if (m_listPaxCabin.GetItemCount()==0)
	{
		m_toolBarPaxCabin.GetToolBarCtrl().EnableButton(ID_PAXCABIN_DEL,false);
	}
}
bool DlgOnboardPaxCabinAssignment::FlightExist(COnboardFlight *flight)
{
	COnboardFlight *curFlight;
	for (int i=0;i<m_listFlight.GetCount();i++)
	{
		curFlight=((CFlightPaxCabinAssign *)m_listFlight.GetItemData(i))->getonboardFlight();
		if (curFlight->getACType()==flight->getACType()
			&& curFlight->getFlightID()==flight->getFlightID()
			&& curFlight->getFlightType()==flight->getFlightType()
			&& curFlight->getDay()==flight->getDay())
		{			
			return true;
		}
	}
	return false;
}

void DlgOnboardPaxCabinAssignment::OnButtonOK()
{
	m_onboardPaxCabinAssign.SaveData(-1);
	OnOK();
}

void DlgOnboardPaxCabinAssignment::OnButtonCancel()
{
	OnCancel();
}
/*
CString DlgOnboardPaxCabinAssignment::FormatFlightStrForPrint(const CString& _configName,COnboardFlight* pFlight)
{
	CString strFlight ;
	if (pFlight->getFlightType() == ArrFlight)
	{
		strFlight.Format(_T("Flight %s - ACTYPE %s Arr Day%d"),pFlight->getFlightID(),pFlight->getACType(), pFlight->getDay());
	}
	else
	{
		strFlight.Format(_T("Flight %s - ACTYPE %s Dep Day%d"),pFlight->getFlightID(),pFlight->getACType(), pFlight->getDay());
	}    
	if(!_configName.IsEmpty())
	{
		CString config;
		config.Format("(%s)",_configName) ;
		strFlight.Append(config) ;
	}
	return strFlight ;
}*/

void DlgOnboardPaxCabinAssignment::OnLvnItemchangedListPaxcabin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	int curPaxCabinIndex=m_listPaxCabin.GetCurSel();
	if (curPaxCabinIndex<0||m_listPaxCabin.GetItemCount()==0)
	{
		m_toolBarPaxCabin.GetToolBarCtrl().EnableButton(ID_PAXCABIN_DEL,false);
	}else
	{
		m_toolBarPaxCabin.GetToolBarCtrl().EnableButton(ID_PAXCABIN_DEL,true);
	}
	*pResult = 0;
}
