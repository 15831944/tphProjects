// DlgUnimpededBodyStateTransTime.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgUnimpededBodyStateTransTime.h"
#include ".\dlgunimpededbodystatetranstime.h"
#include "../InputOnboard/UnimpededStateTransTime.h"
#include "../Database/ADODatabase.h"
#include "DlgProbDist.h"
#include "../inputs/in_term.h"
#include "PassengerTypeDialog.h"
#include "DlgNewPassengerType.h"
// CDlgUnimpededBodyStateTransTime dialog

IMPLEMENT_DYNAMIC(CDlgUnimpededBodyStateTransTime, CXTResizeDialog)
CDlgUnimpededBodyStateTransTime::CDlgUnimpededBodyStateTransTime(InputTerminal *pInputTerminal,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgUnimpededBodyStateTransTime::IDD, pParent)
{
	m_pInputTerminal = pInputTerminal;
	m_pUnimpededData = NULL;
}

CDlgUnimpededBodyStateTransTime::~CDlgUnimpededBodyStateTransTime()
{
}

void CDlgUnimpededBodyStateTransTime::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PAXTYPE, m_lstBoxPaxType);
	DDX_Control(pDX, IDC_LIST_DATA, m_lstData);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
}


BEGIN_MESSAGE_MAP(CDlgUnimpededBodyStateTransTime, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLBARBUTTONADD, OnAddPaxType)
	ON_COMMAND(ID_TOOLBARBUTTONDEL, OnDelePaxType)
	ON_COMMAND(ID_TOOLBARBUTTONEDIT,OnEditPaxType)
	ON_LBN_SELCHANGE(IDC_LIST_PAXTYPE, OnLbnSelchangeListPaxtype)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_DATA, OnNMDblclkListData)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnSave)
END_MESSAGE_MAP()


// CDlgUnimpededBodyStateTransTime message handlers
int CDlgUnimpededBodyStateTransTime::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if( !m_toolBar.CreateEx(this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP) || !m_toolBar.LoadToolBar(IDR_TOOLBAR_UNIMPEDEDSTATETRANS))
	{
		TRACE("Load toolbar failed.");
		return-1;
	}
	// TODO:  Add your specialized creation code here

	return 0;
}

BOOL CDlgUnimpededBodyStateTransTime::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();
	InitToolbar();
	DWORD dwStyle = m_lstData.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_lstData.SetExtendedStyle(dwStyle);
	// TODO:  Add extra initialization here


	SetResize(m_toolBar.GetDlgCtrlID(),SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_STATIC_ALL,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_OUTERFRAME,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_PAXTYPE,SZ_TOP_LEFT,SZ_BOTTOM_CENTER);
	SetResize(IDC_STATIC_PAXTYPETEXT,SZ_TOP_LEFT,SZ_TOP_CENTER);
	SetResize(IDC_LIST_PAXTYPE,SZ_TOP_LEFT,SZ_BOTTOM_CENTER);
	SetResize(IDC_STATIC_TIMETEXT,SZ_TOP_CENTER,SZ_TOP_RIGHT);
	SetResize(IDC_LIST_DATA,SZ_TOP_CENTER,SZ_BOTTOM_RIGHT);


	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);



	InitStateList();
	try
	{
		m_pUnimpededData = new UnimpededStateTransTime();
		m_pUnimpededData->ReadData(m_pInputTerminal->inStrDict);
	}
	catch(CADOException& )
	{
		MessageBox(_T("Cannot read data."),_T("ARCPort ALTOCEF"),MB_OK);
	}
	LoadPaxTypeList();

	UpdateToolbarState();
	EnableSaveButton(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
void CDlgUnimpededBodyStateTransTime::InitToolbar()
{
	GetDlgItem(IDC_STATIC_TOOLBAR)->ShowWindow(SW_HIDE);
	CRect rc;
	GetDlgItem(IDC_STATIC_TOOLBAR)->GetWindowRect( &rc );
	ScreenToClient(&rc);
	rc.top += 2;
	rc.left += 2;
	m_toolBar.MoveWindow(&rc);
	m_toolBar.ShowWindow(SW_SHOW);


	m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD);
	m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONEDIT);
	m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL);



}
void CDlgUnimpededBodyStateTransTime::UpdateToolbarState()
{

	if(m_lstBoxPaxType.GetCurSel() != LB_ERR)
	{
		m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,TRUE);
		m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONEDIT,TRUE);
	}
	else
	{
		m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,FALSE);
		m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONEDIT,FALSE);
	}

}
void CDlgUnimpededBodyStateTransTime::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	if(SaveData())
		CXTResizeDialog::OnOK();
}

void CDlgUnimpededBodyStateTransTime::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CXTResizeDialog::OnCancel();
}

void CDlgUnimpededBodyStateTransTime::OnAddPaxType()
{
	CPassengerType paxtype(m_pInputTerminal->inStrDict);
	CDlgNewPassengerType dlg(&paxtype,m_pInputTerminal,this);
	if( dlg.DoModal() == IDOK )
	{
		if (m_pUnimpededData->FindItem(paxtype))
		{
			MessageBox(_T("The passenger type already exists!"), _T("Error"), MB_OK | MB_ICONWARNING);
			return;
		}
		UnimpededStateTransTimePaxType *pData = new UnimpededStateTransTimePaxType(m_pInputTerminal->inStrDict);
		pData->setPaxType(paxtype);
		m_pUnimpededData->AddItem(pData);
		AddPaxType(pData);

		int nItemCount = m_lstBoxPaxType.GetCount();
		m_lstBoxPaxType.SetCurSel(nItemCount -1);
		LoadStateList();
		EnableSaveButton(TRUE);
	}

}

void CDlgUnimpededBodyStateTransTime::OnEditPaxType()
{
	int nCursel = m_lstBoxPaxType.GetCurSel();
	if (nCursel == LB_ERR)
		return;

	CPassengerType paxtype(m_pInputTerminal->inStrDict);
	CDlgNewPassengerType dlg(&paxtype,m_pInputTerminal,this);
	if( dlg.DoModal() == IDOK )
	{	
		UnimpededStateTransTimePaxType *pPaxType =(UnimpededStateTransTimePaxType *)m_lstBoxPaxType.GetItemData(nCursel);
		pPaxType->setPaxType(paxtype);

		m_lstBoxPaxType.DeleteString(nCursel);

		CString strLabel = pPaxType->GetPaxTypeString();

		m_lstBoxPaxType.InsertString(nCursel,strLabel);
		m_lstBoxPaxType.SetItemData(nCursel,(DWORD_PTR)pPaxType);
		m_lstBoxPaxType.SetCurSel(nCursel);
		EnableSaveButton(TRUE);
	}
}

void CDlgUnimpededBodyStateTransTime::OnDelePaxType()
{
	int nCursel = m_lstBoxPaxType.GetCurSel();
	if (nCursel == LB_ERR)
		return;
	
	UnimpededStateTransTimePaxType *pPaxType =(UnimpededStateTransTimePaxType *)m_lstBoxPaxType.GetItemData(nCursel);

	m_pUnimpededData->DelItem(pPaxType);
	//delete selected one
	m_lstBoxPaxType.DeleteString(nCursel);

	if(nCursel > 0)
	{
		m_lstBoxPaxType.SetCurSel(nCursel-1);
	}
	else //==0
	{
		if(m_lstBoxPaxType.GetCount() > 0)
		{
			m_lstBoxPaxType.SetCurSel(0);		
		}
	}

	EnableSaveButton(TRUE);
	LoadStateList();
}

void CDlgUnimpededBodyStateTransTime::LoadPaxTypeList()
{
	//delete old items
	while(m_lstBoxPaxType.GetCount())
		m_lstBoxPaxType.DeleteString(0);
	
	int nCount = m_pUnimpededData->GetItemCount();
	for (int i = 0; i < nCount; ++i)
	{
		AddPaxType(m_pUnimpededData->GetItem(i));
	}

	if(m_lstBoxPaxType.GetCount())
	{
		m_lstBoxPaxType.SetCurSel(0);
		LoadStateList();
	}

}

void CDlgUnimpededBodyStateTransTime::AddPaxType( UnimpededStateTransTimePaxType *pPaxType )
{
	CString strLabel = pPaxType->GetPaxTypeString();
	int nItem = m_lstBoxPaxType.AddString(strLabel);
	m_lstBoxPaxType.SetItemData(nItem,(DWORD_PTR)pPaxType);

}

void CDlgUnimpededBodyStateTransTime::LoadStateList()
{
	//
	int nCurSel = m_lstBoxPaxType.GetCurSel();
	if(nCurSel == LB_ERR)
		return;

	UnimpededStateTransTimePaxType *pPaxType =(UnimpededStateTransTimePaxType *)m_lstBoxPaxType.GetItemData(nCurSel);
	int nItemCount = pPaxType->GetItemCount();
	for(int nItem = 0; nItem < nItemCount; ++ nItem)
	{
		SetStateValue(pPaxType->GetItem(nItem));
	}
}

void CDlgUnimpededBodyStateTransTime::InitStateList()
{
	m_lstData.InsertColumn(0,_T("State/State"),LVCFMT_LEFT, 80);

	for (int stateV = BodyState_Upright; stateV < BodyState_Count; ++ stateV)
	{
		//insert column 
		m_lstData.InsertColumn(stateV,OnBoard_BodyState_Name[stateV],LVCFMT_LEFT, 80);
	}
	for (int stateH = BodyState_Upright; stateH < BodyState_Count; ++ stateH)
	{
		m_lstData.InsertItem(stateH - 1,OnBoard_BodyState_Name[stateH]);
	}
}

void CDlgUnimpededBodyStateTransTime::SetStateValue( UnimpededStateTransTimePaxTypeData *pData )
{
	int nStateV = pData->getStateV();
	int nStateH = pData->getStateH();
	CString strDistName = pData->getProbDistribution().getPrintDist();
	m_lstData.SetItemText(nStateH-1,nStateV,strDistName);

}
void CDlgUnimpededBodyStateTransTime::OnLbnSelchangeListPaxtype()
{
	// TODO: Add your control notification handler code here
	LoadStateList();
}

void CDlgUnimpededBodyStateTransTime::OnNMDblclkListData(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;	

	//line 
	int nItem = pNMListView->iItem; 
	int nSubItem = pNMListView->iSubItem;

	if(nSubItem == 0)//have not click the probabilities
		return;
	if(nItem == LB_ERR)
		return;

	size_t nStateV = nSubItem;
	size_t nStateH = nItem + 1;
	//check it is valid

	if(nStateV >= BodyState_Count)
		return;
	if(nStateH >= BodyState_Count)
		return;
	
	int nCurSel = m_lstBoxPaxType.GetCurSel();
	if(nCurSel == LB_ERR)
		return;

	//open probability dialog
	CDlgProbDist dlg(m_pInputTerminal->m_pAirportDB,true,this);
	if(dlg.DoModal() == IDCANCEL)
		return;
	
	CProbDistEntry* pPDEntry = dlg.GetSelectedPD();
	if(pPDEntry == NULL)
		return;

	UnimpededStateTransTimePaxType *pPaxType =(UnimpededStateTransTimePaxType *)m_lstBoxPaxType.GetItemData(nCurSel);
	UnimpededStateTransTimePaxTypeData *pData = pPaxType->GetData((OnBoard_BodyState)nStateV, (OnBoard_BodyState)nStateH);
	pData->getProbDistribution().SetProDistrubution(pPDEntry);
	SetStateValue(pData);

	EnableSaveButton(TRUE);


}

void CDlgUnimpededBodyStateTransTime::OnSave()
{
	SaveData();
}

bool CDlgUnimpededBodyStateTransTime::SaveData()
{
	
	try
	{
		CADODatabase::BeginTransaction();
		m_pUnimpededData->SaveData();
		CADODatabase::CommitTransaction();
	}
	catch (CADOException& e)
	{
		EnableSaveButton(TRUE);
		CADODatabase::RollBackTransation();
		CString strError = e.ErrorMessage();
		MessageBox(_T("Save data failed."));
		return false;
	}
	EnableSaveButton(FALSE);
	return true;
}

void CDlgUnimpededBodyStateTransTime::EnableSaveButton(BOOL bShow)
{
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(bShow);
}