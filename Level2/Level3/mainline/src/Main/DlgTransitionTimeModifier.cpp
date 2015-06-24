// DlgTransitionTimeModifier.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgTransitionTimeModifier.h"
#include ".\dlgtransitiontimemodifier.h"
#include "../InputOnboard/TransitionTimeModifiers.h"
#include "../Database/ADODatabase.h"
#include "PassengerTypeDialog.h"
#include "DlgNewPassengerType.h"
// CDlgTransitionTimeModifier dialog

IMPLEMENT_DYNAMIC(CDlgTransitionTimeModifier, CXTResizeDialog)
CDlgTransitionTimeModifier::CDlgTransitionTimeModifier(InputTerminal *pInputTerm,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgTransitionTimeModifier::IDD, pParent)
{
	m_pInputTerminal = pInputTerm;
	m_pTransTimeModifier = NULL;
}

CDlgTransitionTimeModifier::~CDlgTransitionTimeModifier()
{
	delete m_pTransTimeModifier;
}

void CDlgTransitionTimeModifier::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DATA, m_lstData);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
}


BEGIN_MESSAGE_MAP(CDlgTransitionTimeModifier, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLBARBUTTONADD,OnButtonAdd)
	ON_COMMAND(ID_TOOLBARBUTTONEDIT,OnButtonEdit)
	ON_COMMAND(ID_TOOLBARBUTTONDEL,OnButtonDel)
	ON_BN_CLICKED(ID_BUTTON_SAVE, OnSave)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DATA, OnSelChangePassengerType)

END_MESSAGE_MAP()


// CDlgTransitionTimeModifier message handlers

int CDlgTransitionTimeModifier::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if(!m_toolbar.CreateEx(this,TBSTYLE_FLAT,WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP| CBRS_TOOLTIPS) || !m_toolbar.LoadToolBar(IDR_TOOLBAR_TRANSITIONTIMEMODIFIER))
	{
		TRACE("Load toolbar failed\r\n");
        return -1;
	}
	// TODO:  Add your specialized creation code here

	return 0;
}

void CDlgTransitionTimeModifier::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CXTResizeDialog::OnCancel();
}

void CDlgTransitionTimeModifier::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	if(SaveData())
		CXTResizeDialog::OnOK();
}

BOOL CDlgTransitionTimeModifier::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	
	InitToolbar();
	InitListCtrl();

	SetResize(IDC_STATIC_OUTERFRAME,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(m_toolbar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_RIGHT);
	SetResize(IDC_LIST_DATA,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);


	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	try
	{
		m_pTransTimeModifier = new CTransitionTimeModifiers();
		m_pTransTimeModifier->ReadData(m_pInputTerminal->inStrDict);
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
		MessageBox(_T("Load Data failed"));
		return FALSE;
	}

	LoadListData();
	if (m_lstData.GetItemCount() >0)
	{
		m_lstData.SetFocus();
		m_lstData.SetItemState(0,LVIS_SELECTED,LVIS_SELECTED) ;
	}
	UpdateToolbarState();
	EnableSaveButton(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgTransitionTimeModifier::InitToolbar()
{
	CRect rc;
	GetDlgItem(IDC_STATIC_TOOLBAR)->GetWindowRect( &rc );
	ScreenToClient(&rc);
	rc.top  += 2;
	rc.left += 2;
	m_toolbar.MoveWindow(&rc);
	m_toolbar.ShowWindow(SW_SHOW);
	
}

void CDlgTransitionTimeModifier::InitListCtrl()
{
	DWORD dwStyle = m_lstData.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_lstData.SetExtendedStyle( dwStyle );


	LV_COLUMNEX lvc;
	CStringList strList;
	lvc.csList = &strList;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;

	lvc.fmt = LVCFMT_NOEDIT;
	lvc.pszText = _T("Passenger Type");
	lvc.cx = 100;
	m_lstData.InsertColumn(0, &lvc);

	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = _T("Inhibitor(%)");
	lvc.cx = 100;
	m_lstData.InsertColumn(1, &lvc);
	
	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = _T("Mulipilier Trigger(%)");
	lvc.cx = 110;
	m_lstData.InsertColumn(2, &lvc);

	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = _T("Packing Coefficient(Factor)");
	lvc.cx = 140;
	m_lstData.InsertColumn(3, &lvc);

}

void CDlgTransitionTimeModifier::UpdateToolbarState()
{
	m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,TRUE);
	if(m_lstData.GetSelectedCount() > 0)
	{
		m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONEDIT,TRUE);
		m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,TRUE);
	}
	else
	{
		m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONEDIT,FALSE);
		m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,FALSE);
	}
}

void CDlgTransitionTimeModifier::EnableSaveButton( BOOL bEnable )
{
	m_btnSave.EnableWindow(bEnable);
}
void CDlgTransitionTimeModifier::OnButtonDel()
{
	int nCurrentSel = m_lstData.GetCurSel();
	if(nCurrentSel == -1)
		return;	

	CTransitionTimeModifiersData* pData = (CTransitionTimeModifiersData* )m_lstData.GetItemData(nCurrentSel);
	m_pTransTimeModifier->DelItem(pData);
	m_lstData.DeleteItem(nCurrentSel);
	EnableSaveButton(TRUE);
	UpdateToolbarState();
}
void CDlgTransitionTimeModifier::OnButtonEdit()
{
	int nCurrentSel = m_lstData.GetCurSel();
	if(nCurrentSel == -1)
		return;

	CPassengerType paxtype(m_pInputTerminal->inStrDict);
	CDlgNewPassengerType dlg(&paxtype,m_pInputTerminal,this);
	if( dlg.DoModal() == IDOK )
	{
		CTransitionTimeModifiersData* pData = (CTransitionTimeModifiersData* )m_lstData.GetItemData(nCurrentSel);
		pData->setPaxType(paxtype);

		m_lstData.SetItemText(nCurrentSel,0,pData->getPaxTypeString());

		EnableSaveButton(TRUE);
	}
}
void CDlgTransitionTimeModifier::OnButtonAdd()
{
	CPassengerType paxtype(m_pInputTerminal->inStrDict);
	CDlgNewPassengerType dlg(&paxtype,m_pInputTerminal,this);
	if( dlg.DoModal() == IDOK )
	{
		if (m_pTransTimeModifier->FindItem(paxtype))
		{
			MessageBox(_T("The passenger type already exists!"), _T("Error"), MB_OK | MB_ICONWARNING);
			return;
		}
		CTransitionTimeModifiersData *pData = new CTransitionTimeModifiersData(m_pInputTerminal->inStrDict);
		pData->setPaxType(paxtype);
		m_pTransTimeModifier->AddItem(pData);
		AddDataToList(pData);

		EnableSaveButton(TRUE);
	}
	UpdateToolbarState();
}

void CDlgTransitionTimeModifier::LoadListData()
{
	int nCount = m_pTransTimeModifier->GetItemCount();
	for (int i =0; i < nCount; ++i)
	{
		CTransitionTimeModifiersData *pData = m_pTransTimeModifier->GetItem(i);
		AddDataToList(pData);
	}
}

void CDlgTransitionTimeModifier::AddDataToList( CTransitionTimeModifiersData *pData )
{
	int nItemCount = m_lstData.GetItemCount();

	CString strPaxType = pData->getPaxTypeString();	
	m_lstData.InsertItem(nItemCount,strPaxType);
	
	CString strInhibitor;
	strInhibitor.Format(_T("%d"), pData->getInbibitor());
	m_lstData.SetItemText(nItemCount,1,strInhibitor);

	CString strMTrigger;
	strMTrigger.Format(_T("%d"),pData->getMTrigger());
	m_lstData.SetItemText(nItemCount,2,strMTrigger);


	CString strFactor;
	strFactor.Format(_T("%.2f"),pData->getFactor());
	m_lstData.SetItemText(nItemCount,3,strFactor);

	m_lstData.SetItemData(nItemCount,(DWORD_PTR)pData);
	
}

BOOL CDlgTransitionTimeModifier::SaveData()
{
	int nItemCount = m_lstData.GetItemCount();
	for (int nItem = 0; nItem < nItemCount; ++nItem)
	{
	
		CTransitionTimeModifiersData* pData = (CTransitionTimeModifiersData* )m_lstData.GetItemData(nItem);

		CString strInhibitor = m_lstData.GetItemText(nItem,1);
		pData->setInbibitor(static_cast<int>(atoi(strInhibitor)));

		CString strMTrigger = m_lstData.GetItemText(nItem,2);
		pData->setMTrigger(static_cast<int>(atoi(strMTrigger)));

		CString strFactor = m_lstData.GetItemText(nItem,3);
		pData->setFactor(static_cast<double>(atof(strFactor)));

	}


	
	try
	{
		CADODatabase::BeginTransaction();
		m_pTransTimeModifier->SaveData();
		CADODatabase::CommitTransaction();
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
		EnableSaveButton(TRUE);
		CADODatabase::RollBackTransation();
		MessageBox(_T("Save data failed."));
		return false;
	}
	EnableSaveButton(FALSE);
	return true;
	
}

void CDlgTransitionTimeModifier::OnSave()
{
	SaveData();
}
void CDlgTransitionTimeModifier::OnSelChangePassengerType(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	UpdateToolbarState();
}






























