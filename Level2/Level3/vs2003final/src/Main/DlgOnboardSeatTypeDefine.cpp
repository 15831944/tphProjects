

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgOnboardSeatTypeDefine.h"
#include "DlgOnboardFlightList.h"
#include "..\InputOnboard\AircaftLayOut.h"
#include ".\DlgOnboardSeatTypeDefine.h"


IMPLEMENT_DYNCREATE(DlgOnboardSeatTypeDefine, CXTResizeDialog)


DlgOnboardSeatTypeDefine::DlgOnboardSeatTypeDefine(InputOnboard* pInputOnboard,/*int nProjID,*/Terminal* _pTerm,CWnd* pParent )
	: CXTResizeDialog(DlgOnboardSeatTypeDefine::IDD, pParent)

{
	m_pTerminal=_pTerm;
 	m_pInputOnboard=pInputOnboard;
	m_pSeatTypeDefine=new COnboardSeatTypeDefine;
	m_listSeatType.setInputOnboard(pInputOnboard);
}

DlgOnboardSeatTypeDefine::DlgOnboardSeatTypeDefine()
	:CXTResizeDialog(DlgOnboardSeatTypeDefine::IDD, NULL)
{

	m_pTerminal=NULL;
	m_pInputOnboard=NULL;
	m_pSeatTypeDefine=new COnboardSeatTypeDefine;

}
DlgOnboardSeatTypeDefine::~DlgOnboardSeatTypeDefine()
{
}
BEGIN_MESSAGE_MAP(DlgOnboardSeatTypeDefine, CXTResizeDialog)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_FLIGHT_ADD,OnNewFlight)
	ON_COMMAND(ID_FLIGHT_DEL,OnDelFlight)
	ON_COMMAND(ID_SEATTYPE_ADD,OnNewSeatTypeDef)
	ON_COMMAND(ID_SEATTYPE_DEL,OnDelSeatTypeDef)
	ON_BN_CLICKED(IDC_BUTTON_OK,OnButtonOK)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL,OnButtonCancel)
	ON_LBN_SELCHANGE(IDC_LIST_FLIGHT,OnSelChangeFlightList)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonCurbsave)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SEATTYPE, OnLvnItemchangedListSeattype)
END_MESSAGE_MAP()

void DlgOnboardSeatTypeDefine::DoDataExchange(CDataExchange* pDX)
{

	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_LIST_FLIGHT,m_listFlight);
	DDX_Control(pDX,IDC_LIST_SEATTYPE,m_listSeatType);

}
afx_msg int DlgOnboardSeatTypeDefine::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	if (!m_toolBarFlight.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_toolBarFlight.LoadToolBar(IDR_ADDDEL_FLIGHT))
	{
		return -1;
	}
	if (!m_toolBarSeatType.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_toolBarSeatType.LoadToolBar(IDR_ADDDEL_SEATTYPE))
	{
		return -1;
	}
	return 0;

}

BOOL DlgOnboardSeatTypeDefine::OnInitDialog()
{

	CXTResizeDialog::OnInitDialog();
	m_pSeatTypeDefine->ReadData(-1);
	InitToolBar();
	InitFlightList();
	CreatSeatTypeDefList();

	SetResize(IDC_STATIC,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_LIST_FLIGHT,SZ_TOP_LEFT,SZ_BOTTOM_CENTER);
	SetResize(IDC_LIST_SEATTYPE,SZ_TOP_CENTER,SZ_BOTTOM_RIGHT);

	SetResize(IDC_BUTTON_CANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_OK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control

}
void DlgOnboardSeatTypeDefine::InitToolBar()
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
	
		m_listSeatType.GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.top=rect.top-26;
		rect.left=rect.left+2;
		rect.bottom=rect.top+22;
		m_toolBarSeatType.MoveWindow(rect);
		m_toolBarSeatType.ShowWindow(SW_SHOW);
		m_toolBarSeatType.GetToolBarCtrl().EnableButton(ID_SEATTYPE_ADD,false);
		m_toolBarSeatType.GetToolBarCtrl().EnableButton(ID_SEATTYPE_DEL,false);			
}
void DlgOnboardSeatTypeDefine::InitFlightList()
{
	CDlgOnboardFlightList dlg(m_pTerminal);
	CFlightSeatTypeDefine *curDef;
	for(int i=0;i<(int)m_pSeatTypeDefine->GetItemCount();i++)
	{
		curDef=(CFlightSeatTypeDefine *)m_pSeatTypeDefine->GetItem(i);
		COnboardFlight *onboardFlight=curDef->getonboardFlight();
		CAircaftLayOut* _layout = CAircarftLayoutManager::GetInstance()->GetAircraftLayOutByFlightID(onboardFlight->getID()) ;
		CString strLayout;
		if(_layout != NULL)
			strLayout = _layout->GetName() ;
		int tmpIndex=m_listFlight.InsertString(m_listFlight.GetCount(),dlg.FormatFlightIdNodeForCandidate(strLayout,onboardFlight));
		m_listFlight.SetItemData(tmpIndex,(DWORD)curDef);
	}

}


void DlgOnboardSeatTypeDefine::OnSize(UINT nType, int cx, int cy)
{
	if (!m_listFlight||!m_listSeatType)
	{
		return;
	}
	CXTResizeDialog::OnSize(nType,cx,cy);
	CRect rect;

	m_listSeatType.GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.top=rect.top-26;
	rect.left=rect.left+2;
	rect.bottom=rect.top+22;
	m_toolBarSeatType.MoveWindow(rect);
}
void DlgOnboardSeatTypeDefine::CreatSeatTypeDefList()
{

	DWORD dwStyle = m_listSeatType.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listSeatType.SetExtendedStyle(dwStyle);

	CStringList strList;
	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;
	lvc.pszText = _T("Seat Group");
	lvc.cx = 100;
 	lvc.fmt = LVCFMT_NOEDIT;
	lvc.csList = &strList;
	m_listSeatType.InsertColumn(0, &lvc);

	lvc.fmt = LVCFMT_DROPDOWN;
	lvc.pszText = _T("Seat Type");
	m_listSeatType.InsertColumn(1, &lvc);

}
CFlightSeatTypeDefine *DlgOnboardSeatTypeDefine::getCurFlightSeatTypeDef()
{
	int curFlightIndex=m_listFlight.GetCurSel();
	if (curFlightIndex<0)
	{
		return NULL;
	}
	return (CFlightSeatTypeDefine *)m_listFlight.GetItemData(curFlightIndex);
}
CSeatTypeDefine *DlgOnboardSeatTypeDefine::getCurSeatTypeDef()
{
	int curIndex=m_listSeatType.GetCurSel();
	if (curIndex<0)
	{
		return NULL;
	}
	return (CSeatTypeDefine *)m_listSeatType.GetItemData(curIndex);
}
void DlgOnboardSeatTypeDefine::OnSelChangeFlightList()
{

	int curFlightIndex=m_listFlight.GetCurSel();
	if (curFlightIndex<0)
	{
		return;
	}
	m_toolBarSeatType.GetToolBarCtrl().EnableButton(ID_SEATTYPE_ADD,true);
	m_toolBarFlight.GetToolBarCtrl().EnableButton(ID_FLIGHT_DEL,true);
	m_listSeatType.resetListContent(getCurFlightSeatTypeDef());

}

// DlgOnboardSeatTypeDefine message handlers
void DlgOnboardSeatTypeDefine::OnNewFlight()
{

	CDlgOnboardFlightList dlg(m_pTerminal);
	if(dlg.DoModal()==IDOK)
	{
		COnboardFlight *onboardFlight=dlg.getOnboardFlight();
		if(FlightExist(onboardFlight))
		{			
			MessageBox("Flight already exists in list.");
			return;
		}		
	
		CFlightSeatTypeDefine *newDefine=new CFlightSeatTypeDefine();
		newDefine->setonboardFlight(onboardFlight);
		m_pSeatTypeDefine->AddItem(newDefine);
        int tmpIndex=m_listFlight.InsertString(m_listFlight.GetCount(),dlg.getFlightString());
		m_listFlight.SetItemData(tmpIndex,(DWORD)newDefine);
	}	

}
void DlgOnboardSeatTypeDefine::OnDelFlight()
{

		int curFlightIndex=m_listFlight.GetCurSel();
		if (curFlightIndex<0)
		{
			return;
		}	
		m_pSeatTypeDefine->DeleteItem(getCurFlightSeatTypeDef());
		m_listFlight.DeleteString(curFlightIndex);	 
		if (curFlightIndex<m_listFlight.GetCount())
		{
			m_listFlight.SetCurSel(curFlightIndex);
 			m_listSeatType.resetListContent(getCurFlightSeatTypeDef());
		}else if(m_listFlight.GetCount()>0)
		{
			m_listFlight.SetCurSel(m_listFlight.GetCount()-1);
 			m_listSeatType.resetListContent(getCurFlightSeatTypeDef());
		}else
		{
			m_toolBarSeatType.GetToolBarCtrl().EnableButton(ID_SEATTYPE_ADD,false);	
			m_toolBarFlight.GetToolBarCtrl().EnableButton(ID_FLIGHT_DEL,false);
			m_listSeatType.DeleteAllItems();
		}	
		m_toolBarSeatType.GetToolBarCtrl().EnableButton(ID_SEATTYPE_DEL,false);
}
void DlgOnboardSeatTypeDefine::OnNewSeatTypeDef()
{

	CFlightSeatTypeDefine *curFlightSeatTypeDefine=getCurFlightSeatTypeDef();
	CSeatTypeDefine *SeatTypeDef=new CSeatTypeDefine();	
	curFlightSeatTypeDefine->AddItem(SeatTypeDef);
    m_listSeatType.resetListContent(curFlightSeatTypeDefine);
	m_listSeatType.SetCurSel(m_listSeatType.GetItemCount()-1);
	CRect rect;
	m_listSeatType.GetSubItemRect(m_listSeatType.GetItemCount()-1,1,LVIR_LABEL,rect);
	rect.left+=rect.Width()/2;
	rect.top+=rect.Height()/2;
	m_listSeatType.OnLButtonDblClk(1,CPoint(rect.left,rect.top));
}
void DlgOnboardSeatTypeDefine::OnDelSeatTypeDef()
{
		int curIndex=m_listSeatType.GetCurSel();
		if (curIndex<0)
		{
			return;
		}
		CFlightSeatTypeDefine *curDef=getCurFlightSeatTypeDef();
		if (curDef!=NULL)
		{
			curDef->DeleteItem(getCurSeatTypeDef());
		}
		m_listSeatType.DeleteItemEx(curIndex);	
		if (m_listSeatType.GetItemCount()==0)
		{
			m_toolBarSeatType.GetToolBarCtrl().EnableButton(ID_SEATTYPE_DEL,false);
		}
}
bool DlgOnboardSeatTypeDefine::FlightExist(COnboardFlight *flight)
{
	COnboardFlight *curFlight;
	for (int i=0;i<m_listFlight.GetCount();i++)
	{
		curFlight=((CFlightSeatTypeDefine *)m_listFlight.GetItemData(i))->getonboardFlight();
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

void DlgOnboardSeatTypeDefine::OnButtonOK()
{
	m_pSeatTypeDefine->SaveData(-1);
	OnOK();
}

void DlgOnboardSeatTypeDefine::OnButtonCancel()
{
	OnCancel();
}


void DlgOnboardSeatTypeDefine::OnBnClickedButtonCurbsave()
{
	// TODO: Add your control notification handler code here
	m_pSeatTypeDefine->SaveData(-1);
}

void DlgOnboardSeatTypeDefine::OnLvnItemchangedListSeattype(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	if (getCurSeatTypeDef()>0)
	{
		m_toolBarSeatType.GetToolBarCtrl().EnableButton(ID_SEATTYPE_DEL,true);
	}else
	{
		m_toolBarSeatType.GetToolBarCtrl().EnableButton(ID_SEATTYPE_DEL,false);
	}
}
