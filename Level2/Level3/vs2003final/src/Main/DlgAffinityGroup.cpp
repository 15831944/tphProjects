#include "StdAfx.h"
#include "Resource.h"
#include ".\dlgaffinitygroup.h"
#include "TermPlan.h"
#include "../InputOnBoard/AffinityGroup.h"
#include "../Common/NewPassengerType.h"
#include "DlgNewPassengerType.h"
#include "../Common/FLT_CNST.H"
#include "FlightDialog.h"
#include "../Inputs/IN_TERM.H"
#include "../Common/ProDistrubutionData.h"
#include "../Common/ProbDistEntry.h"
#include "../Common/AirportDatabase.h"
#include "../Common/ProbDistManager.h"
#include "DlgProbDist.h"
#include "../Common/template.h"


IMPLEMENT_DYNAMIC(CDlgAffinityGroup, CXTResizeDialog)
CDlgAffinityGroup::CDlgAffinityGroup( InputTerminal* _terminal , CWnd* pParent /*= NULL*/ )
:CXTResizeDialog(CDlgAffinityGroup::IDD, pParent)
,m_pTerminal(_terminal)
{
	m_pAffinityGroup = m_pTerminal->m_pAffinityGroup;
	
}
CDlgAffinityGroup::~CDlgAffinityGroup(void)
{

}

void CDlgAffinityGroup::DoDataExchange( CDataExchange* pDX )
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC, m_wndToolbar);
	DDX_Control(pDX, IDC_TREE_AFFINITY, m_wndTreeCtrl);
}

BEGIN_MESSAGE_MAP(CDlgAffinityGroup, CXTResizeDialog)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnSave)
	ON_BN_CLICKED(IDOK,OnOK)
	ON_BN_CLICKED(IDCANCEL,OnCancel)

	ON_COMMAND(ID_TOOLBAR_NEWGROUP,OnNewToolBar)
	ON_COMMAND(ID_TOOLBAR_DELGROUP,OnDelToolBar)
	
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_AFFINITY, OnTvnSelchangedTree)
	ON_WM_CREATE() 
END_MESSAGE_MAP()

BOOL CDlgAffinityGroup::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolbar.CreateEx(this,  TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP ) ||
		!m_wndToolbar.LoadToolBar(ID_TOOLBAR_EDITAFFINITYGROUP))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	return 0;
}

BOOL CDlgAffinityGroup::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog() ;
	//m_pAffinityGroup->ReadData() ;

	OnInitToolBar() ;
	OnInitTree() ;

	SetResize(IDC_STATIC,SZ_TOP_LEFT,SZ_TOP_RIGHT) ;
	SetResize(IDC_TREE_AFFINITY,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT) ;
	SetResize(m_wndToolbar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_BUTTON_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC1,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT) ;

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);


	return TRUE;
}

void CDlgAffinityGroup::OnInitToolBar()
{
	CRect rect ;
	GetDlgItem(IDC_STATIC)->GetWindowRect(&rect);
	ScreenToClient(&rect);

	m_wndToolbar.MoveWindow(rect);
	m_wndToolbar.ShowWindow(SW_SHOW);

	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_NEWGROUP,TRUE);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DELGROUP,FALSE);
	//m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_EDITGROUP,FALSE);
	GetDlgItem(IDC_STATIC)->ShowWindow(FALSE);
}

void CDlgAffinityGroup::OnInitTree()
{
	int nCount = m_pAffinityGroup->GetFltDataCount();
	if (nCount == 0)
		return;

	for (int i =0; i < nCount; i++)		//flt type
	{
		CAffinity_FlightTypeData* pFltData = m_pAffinityGroup->GetFltDataByIdx(i);
		AddFltDataToTree(pFltData);
	}
}

HTREEITEM CDlgAffinityGroup::AddFltDataToTree( CAffinity_FlightTypeData* pFltData )
{
	COOLTREE_NODE_INFO info ;
	CCoolTree::InitNodeInfo(this,info);
	info.nt=NT_NORMAL;
	info.net = NET_SHOW_DIALOGBOX;
	info.bAutoSetItemText = FALSE ;

	char szBuffer[1024] = {0};
	pFltData->GetFltConstraint().screenPrint(szBuffer);

	CString strName;
	strName.Format("Flight Type: %s",szBuffer) ;
	HTREEITEM hRoot = m_wndTreeCtrl.InsertItem(strName,info,TRUE,FALSE);
	m_wndTreeCtrl.SetItemData(hRoot, (DWORD_PTR)pFltData);

	int nGroupCount = pFltData->GetGroupDataCount();
	for (int j =0; j < nGroupCount; j++)			//group
	{
		CAffinity_GroupTypeData* pGroupData = pFltData->GetGroupDataByIdx(j);
		AddGroupDataToTree(pGroupData, hRoot);
	}
	m_wndTreeCtrl.Expand(hRoot, TVE_EXPAND);
	return hRoot;
}

HTREEITEM CDlgAffinityGroup::AddGroupDataToTree( CAffinity_GroupTypeData* pGroupData, HTREEITEM hRoot )
{
	COOLTREE_NODE_INFO info ;
	CCoolTree::InitNodeInfo(this,info);
	info.nt=NT_NORMAL;
	info.net = NET_LABLE;
	info.bAutoSetItemText = FALSE ;

	CString strName;
	strName.Format("Group name: %s",pGroupData->GetGroupName());
	HTREEITEM hGroup = m_wndTreeCtrl.InsertItem(strName,info,TRUE,FALSE,hRoot);
	m_wndTreeCtrl.SetItemData(hGroup, (DWORD_PTR)pGroupData);

	info.net = NET_COMBOBOX;
	if (pGroupData->IsAllNum())
		strName = "Number of groups on aircraft: All";
	else
		strName.Format("Number of groups on aircraft: %s", pGroupData->getNumber()->getPrintDist());
	m_wndTreeCtrl.InsertItem(strName, info, TRUE, FALSE, hGroup);

	//info.net = NET_COMBOBOX;
	if (pGroupData->IsCohesionOn())
		strName = "Cohesion: ON";
	else
		strName = "Cohesion: OFF";
	m_wndTreeCtrl.InsertItem(strName, info, TRUE, FALSE, hGroup);

	int nPaxCount = pGroupData->getPaxTypeCount();			//pax type
	for (int m =0; m < nPaxCount; m++)
	{
		CAffinity_PassengerTypeData* pPaxData = pGroupData->getPaxTypeDataByIdx(m);
		AddPaxDataToTree(pPaxData, hGroup);

	}
	m_wndTreeCtrl.Expand(hGroup, TVE_EXPAND);
	return hGroup;
}

HTREEITEM CDlgAffinityGroup::AddPaxDataToTree( CAffinity_PassengerTypeData* pPaxData, HTREEITEM hGroup )
{
	COOLTREE_NODE_INFO info ;
	CCoolTree::InitNodeInfo(this,info);
	info.nt=NT_NORMAL;
	info.net = NET_SHOW_DIALOGBOX;
	info.bAutoSetItemText = FALSE ;

	CString strName;
	strName.Format("Passenger type: %s", pPaxData->GetPaxType()->PrintStringForShow());
	HTREEITEM hPax = m_wndTreeCtrl.InsertItem(strName, info, TRUE, FALSE, hGroup);
	m_wndTreeCtrl.SetItemData(hPax, (DWORD_PTR)pPaxData);

	info.net = NET_COMBOBOX;
	strName.Format("Number in group: %s", pPaxData->getNumber()->getPrintDist());
	m_wndTreeCtrl.InsertItem(strName, info, TRUE, FALSE, hPax);

	//designation
	Designation eValue = pPaxData->getDesignation1();
	if (eValue != NOVALUE)
	{
		if (eValue == LEADER)
			strName = "Designation 1: LEADER";
		else
			strName = "Designation 1: PACER";
		m_wndTreeCtrl.InsertItem(strName, info, TRUE, FALSE, hPax);
	}

	eValue = pPaxData->getDesignation2();
	if (eValue != NOVALUE)
	{
		if (eValue == LEADER)
			strName = "Designation 2: LEADER";
		else
			strName = "Designation 2: PACER";
		m_wndTreeCtrl.InsertItem(strName, info, TRUE, FALSE, hPax);
	}
	m_wndTreeCtrl.Expand(hPax, TVE_EXPAND);	

	return hPax;
}

void CDlgAffinityGroup::OnTvnSelchangedTree( NMHDR *pNMHDR, LRESULT *pResult )
{
	HTREEITEM hSelItem = m_wndTreeCtrl.GetSelectedItem();
	if (hSelItem == NULL)		//no selected
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_NEWGROUP,TRUE) ;
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DELGROUP,FALSE) ;
		return;
	}

	HTREEITEM hParItem = m_wndTreeCtrl.GetParentItem(hSelItem);
	if (hParItem == NULL)			//select flt type
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_NEWGROUP,TRUE) ;
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DELGROUP,TRUE) ;
		return;
	}

	HTREEITEM hGrandItem = m_wndTreeCtrl.GetParentItem(hParItem);
	if (hGrandItem == NULL)		// select group
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_NEWGROUP,TRUE) ;
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DELGROUP,TRUE) ;
		return;
	}

	HTREEITEM hGrandParItem = m_wndTreeCtrl.GetParentItem(hGrandItem);
	if (hGrandParItem == NULL && m_wndTreeCtrl.GetItemText(hSelItem).Find("Passenger") >= 0)		//select pax type
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_NEWGROUP,TRUE) ;
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DELGROUP,TRUE) ;
		return;
	}

	if (m_wndTreeCtrl.GetItemText(hSelItem).Find("Designation") >= 0)
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_NEWGROUP,FALSE) ;
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DELGROUP,TRUE) ;
		return;
	}

	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_NEWGROUP,FALSE) ;
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DELGROUP,FALSE) ;

}

void CDlgAffinityGroup::OnNewToolBar()
{
	HTREEITEM hSelItem = m_wndTreeCtrl.GetSelectedItem();
	if (hSelItem == NULL)		//no selected
	{
		OnNewAffinityFltType();
		return;
	}

	HTREEITEM hParItem = m_wndTreeCtrl.GetParentItem(hSelItem);
	if (hParItem == NULL)			//select flt type
	{
		OnNewAffinityGroup();
		return;
	}

	HTREEITEM hGrandItem = m_wndTreeCtrl.GetParentItem(hParItem);
	if (hGrandItem == NULL)		// select group
	{
		OnNewAffinityPaxType();
		return;
	}

	//select pax type

	CAffinity_GroupTypeData* pGroupData = (CAffinity_GroupTypeData*)m_wndTreeCtrl.GetItemData(hParItem);
	if (pGroupData)
	{
		if(pGroupData->IsPacerExist() && pGroupData->IsLeaderExist())
		{
			AfxMessageBox("The Leader and Pacer already exist!");
			return;
		}

		CAffinity_PassengerTypeData* pPaxData = (CAffinity_PassengerTypeData*)m_wndTreeCtrl.GetItemData(hSelItem);
		int nChildCount = 0;
		if (m_wndTreeCtrl.ItemHasChildren(hSelItem))
		{
			HTREEITEM hNextItem;
			HTREEITEM hChildItem = m_wndTreeCtrl.GetChildItem(hSelItem);


			while (hChildItem != NULL)
			{
				hNextItem = m_wndTreeCtrl.GetNextItem(hChildItem, TVGN_NEXT);
				hChildItem = hNextItem;
				nChildCount++;
			}

		}

		COOLTREE_NODE_INFO info ;
		CCoolTree::InitNodeInfo(this,info);
		info.nt=NT_NORMAL;
		info.net = NET_COMBOBOX;
		info.bAutoSetItemText = FALSE ;

		if (nChildCount ==1)		//no designation
		{			
			if (pGroupData->IsLeaderExist())
			{
				CString strName = "Designation 1: PACER";
				pPaxData->setDesignation1(PACER);
				m_wndTreeCtrl.InsertItem(strName,info, TRUE, FALSE, hSelItem);
			}
			else
			{
				CString strName = "Designation 1: LEADER";
				pPaxData->setDesignation1(LEADER);
				m_wndTreeCtrl.InsertItem(strName,info, TRUE, FALSE, hSelItem);
			}
		}
		else
		{
			CString strName;
			if (pPaxData->getDesignation1() == PACER)
			{
				strName = "Designation 2: LEADER";
				pPaxData->setDesignation2(LEADER);
			}
			else
			{
				strName = "Designation 2: PACER";
				pPaxData->setDesignation2(PACER);
			}

			m_wndTreeCtrl.InsertItem(strName,info, TRUE, FALSE, hSelItem);
		}
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
}

void CDlgAffinityGroup::OnDelToolBar()
{
	HTREEITEM hSelItem = m_wndTreeCtrl.GetSelectedItem();
	if (hSelItem == NULL)		//no selected
		return;
	
	HTREEITEM hParItem = m_wndTreeCtrl.GetParentItem(hSelItem);
	if (hParItem == NULL)			//select flt type
	{
		OnDelAffinityFltType();
		return;
	}

	HTREEITEM hGrandItem = m_wndTreeCtrl.GetParentItem(hParItem);
	if (hGrandItem == NULL)		// select group
	{
		OnDelAffinityGroup();
		return;
	}
	HTREEITEM hGrandParItem = m_wndTreeCtrl.GetParentItem(hGrandItem);
	if (hGrandParItem == NULL)
	{
		OnDelAffinityPaxType();		//select pax type
		return;
	}

	//select designation
	CAffinity_PassengerTypeData* pPaxData = (CAffinity_PassengerTypeData*)m_wndTreeCtrl.GetItemData(hParItem);
	CString strName = m_wndTreeCtrl.GetItemText(hSelItem);
	if (strName.Find("Designation 1") >= 0)
	{
		Designation eValue = pPaxData->getDesignation2();
		pPaxData->setDesignation1(eValue);
		pPaxData->setDesignation2(NOVALUE);
		m_wndTreeCtrl.DeleteItem(hSelItem);

		HTREEITEM hChild = m_wndTreeCtrl.GetChildItem(hParItem);
		HTREEITEM hNext = m_wndTreeCtrl.GetNextItem(hChild, TVGN_NEXT);
		if (hNext != NULL)
		{
			if (pPaxData->getDesignation1() == LEADER)
				strName = "Designation 1: LEADER";
			else
				strName = "Designation 1: PACER";

			m_wndTreeCtrl.SetItemText(hNext, strName);
		}

	}
	else
	{
		pPaxData->setDesignation2(NOVALUE);
		m_wndTreeCtrl.DeleteItem(hSelItem);
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgAffinityGroup::OnSave()
{
	m_pAffinityGroup->SaveData();
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
	return;
}

void CDlgAffinityGroup::OnOK()
{
	OnSave();
	CXTResizeDialog::OnOK();
}

void CDlgAffinityGroup::OnCancel()
{
	CXTResizeDialog::OnCancel();
}

void CDlgAffinityGroup::OnNewAffinityFltType()
{
	FlightConstraint fltType;
	CFlightDialog flightDlg(NULL);
	if(IDOK == flightDlg.DoModal())
	{
		fltType = flightDlg.GetFlightSelection();

		CAffinity_FlightTypeData* pFltData = new CAffinity_FlightTypeData;
		pFltData->SetFltConstraint(fltType);
		CAffinity_GroupTypeData* pGroupData = new CAffinity_GroupTypeData;
		pFltData->AddGroupData(pGroupData);
		m_pAffinityGroup->AddFltData(pFltData);

		HTREEITEM hFltItem = AddFltDataToTree(pFltData);
		HTREEITEM hGroupItem = m_wndTreeCtrl.GetChildItem(hFltItem);
		if (hGroupItem)
		{
			m_wndTreeCtrl.DoEdit(hGroupItem) ;
		}
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
}

void CDlgAffinityGroup::OnDelAffinityFltType()
{
	HTREEITEM hSelItem = m_wndTreeCtrl.GetSelectedItem();
	if (hSelItem == NULL)
		return;

	HTREEITEM hParItem = m_wndTreeCtrl.GetParentItem(hSelItem);
	if (hParItem == NULL)
	{
		CAffinity_FlightTypeData* pFltData = (CAffinity_FlightTypeData*)m_wndTreeCtrl.GetItemData(hSelItem);
		m_pAffinityGroup->DelFltData(pFltData);
		m_wndTreeCtrl.DeleteItem(hSelItem);
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgAffinityGroup::OnEditAffinityFltType()
{
	HTREEITEM hSelItem = m_wndTreeCtrl.GetSelectedItem();
	if (hSelItem == NULL)
		return;

	CAffinity_FlightTypeData* pFltData = (CAffinity_FlightTypeData*)m_wndTreeCtrl.GetItemData(hSelItem);
	FlightConstraint fltType;
	CFlightDialog flightDlg(NULL);
	if(IDOK == flightDlg.DoModal())
	{
		fltType = flightDlg.GetFlightSelection();

		CString fltName;
		fltType.screenPrint(fltName);
		
		CString strName;
		strName.Format("Flight Type: %s",fltName);
		pFltData->SetFltConstraint(fltType);
		m_wndTreeCtrl.SetItemText(hSelItem, strName);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
}

void CDlgAffinityGroup::OnNewAffinityGroup()
{
	HTREEITEM hSelItem = m_wndTreeCtrl.GetSelectedItem();
	if (hSelItem == NULL)
		return;

	CAffinity_FlightTypeData* pFltData = (CAffinity_FlightTypeData*)m_wndTreeCtrl.GetItemData(hSelItem);
	CAffinity_GroupTypeData* pGroupData = new CAffinity_GroupTypeData;
	pFltData->AddGroupData(pGroupData);

	HTREEITEM hGroup = AddGroupDataToTree(pGroupData,hSelItem);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	m_wndTreeCtrl.DoEdit(hGroup);
}

void CDlgAffinityGroup::OnDelAffinityGroup()
{
	HTREEITEM hSelItem = m_wndTreeCtrl.GetSelectedItem();
	HTREEITEM hParItem = m_wndTreeCtrl.GetParentItem(hSelItem);

	CAffinity_FlightTypeData* pFltData = (CAffinity_FlightTypeData*)m_wndTreeCtrl.GetItemData(hParItem);
	CAffinity_GroupTypeData* pGroupData = (CAffinity_GroupTypeData*)m_wndTreeCtrl.GetItemData(hSelItem);
	pFltData->DelGroupData(pGroupData);
	m_wndTreeCtrl.DeleteItem(hSelItem);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgAffinityGroup::OnNewAffinityPaxType()
{	

	CAffinity_PassengerTypeData* pPaxData = new CAffinity_PassengerTypeData(m_pTerminal->inStrDict);
	CDlgNewPassengerType dlg(pPaxData->getPaxType(),m_pTerminal,this);
	if(dlg.DoModal() == IDOK)
	{
		HTREEITEM hSelItem = m_wndTreeCtrl.GetSelectedItem() ;
		CAffinity_GroupTypeData* pGroupData = (CAffinity_GroupTypeData*)m_wndTreeCtrl.GetItemData(hSelItem);
		pGroupData->AddPaxData(pPaxData);

		AddPaxDataToTree(pPaxData, hSelItem);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}

}

void CDlgAffinityGroup::OnDelAffinityPaxType()
{
	HTREEITEM hSelItem = m_wndTreeCtrl.GetSelectedItem() ;
	HTREEITEM hParItem = m_wndTreeCtrl.GetParentItem(hSelItem) ;
	CAffinity_GroupTypeData* pGroupData = (CAffinity_GroupTypeData*)m_wndTreeCtrl.GetItemData(hParItem);
	CAffinity_PassengerTypeData* pPaxData = (CAffinity_PassengerTypeData*)m_wndTreeCtrl.GetItemData(hSelItem);
	pGroupData->DelPaxData(pPaxData);
	m_wndTreeCtrl.DeleteItem(hSelItem);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgAffinityGroup::OnEditAffinityPaxType()
{
	HTREEITEM hSelItem = m_wndTreeCtrl.GetSelectedItem() ;
	CAffinity_PassengerTypeData* pPaxData = (CAffinity_PassengerTypeData*)m_wndTreeCtrl.GetItemData(hSelItem);
	CDlgNewPassengerType dlg(pPaxData->getPaxType(),m_pTerminal,this);
	if(dlg.DoModal() == IDOK)
	{
		CString strPaxType;
		strPaxType.Format("Passenger type: %s", pPaxData->GetPaxType()->PrintStringForShow());
		m_wndTreeCtrl.SetItemText(hSelItem, strPaxType);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
}

LRESULT CDlgAffinityGroup::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	switch(message)
	{
	case WM_LBUTTONDBLCLK:
		{
			HTREEITEM hItem = (HTREEITEM)wParam;
			if(hItem == NULL)
				break ;

			m_wndTreeCtrl.EditLabel(hItem) ;
		}
		break;
	case UM_CEW_LABLE_END:
		{
			HTREEITEM hItem = (HTREEITEM)wParam;
			CString strValue = *((CString*)lParam);

			CAffinity_GroupTypeData* pGroupData = (CAffinity_GroupTypeData*)m_wndTreeCtrl.GetItemData(hItem);			
			pGroupData->SetGroupName(strValue);

			CString strName;
			strName.Format("Group name: %s",pGroupData->GetGroupName());
			m_wndTreeCtrl.SetItemText(hItem,strName);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
		break;
	case UM_CEW_COMBOBOX_BEGIN:
		{
			CWnd* pWnd = m_wndTreeCtrl.GetEditWnd((HTREEITEM)wParam);
			CRect rectWnd;
			HTREEITEM hItem = (HTREEITEM)wParam;

			m_wndTreeCtrl.GetItemRect((HTREEITEM)wParam,rectWnd,TRUE);
			pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
			CComboBox* pCB=(CComboBox*)pWnd;
			pCB->ResetContent();

			CString strName = m_wndTreeCtrl.GetItemText(hItem);
			if (strName.Find("Number") >=0)
			{
				pCB->AddString( "NEW PROBABILITY DISTRIBUTION" );
				CProbDistManager* pProbDistMan = m_pTerminal->m_pAirportDB->getProbDistMan();
				int nCount = static_cast<int>(pProbDistMan->getCount());
				for( int m=0; m<nCount; m++ )
				{
					CProbDistEntry* pPBEntry = pProbDistMan->getItem( m );
					pCB->AddString( pPBEntry->m_csName );
				}
				pCB->SetCurSel(0);
				
			}
			else if (strName.Find("Cohesion") >=0)
			{
				int nIdx = pCB->AddString("OFF");
				pCB->SetItemData(nIdx, 0);
				nIdx = pCB->AddString("ON");
				pCB->SetItemData(nIdx, 1);
			}
			else
			{
				int nIdx = pCB->AddString("LEADER");
				pCB->SetItemData(nIdx, 0);
				nIdx = pCB->AddString("PACER");
				pCB->SetItemData(nIdx, 1);
			}
			pCB->SetCurSel(0);
		}
		break;
	case UM_CEW_COMBOBOX_SELCHANGE:
		{
			HTREEITEM hItem = (HTREEITEM)wParam;
			HTREEITEM hParItem = m_wndTreeCtrl.GetParentItem(hItem);

			CWnd* pWnd = m_wndTreeCtrl.GetEditWnd(hItem);
			CComboBox* pCB=(CComboBox*)pWnd;

			CString strName = m_wndTreeCtrl.GetItemText(hItem);
			if (strName.Find("Number") >=0)
			{
				int nIndex = pCB->GetCurSel();
				CString strSel;

				int n = pCB->GetLBTextLen( nIndex );
				pCB->GetLBText( nIndex, strSel.GetBuffer(n) );
				strSel.ReleaseBuffer();

				char szBuffer[1024] = {0};
				ProbabilityDistribution* pProbDist = NULL;
				CProbDistEntry* pPDEntry = NULL;
				CProbDistManager* pProbDistMan = m_pTerminal->m_pAirportDB->getProbDistMan();
				if(strSel == _T("NEW PROBABILITY DISTRIBUTION" ) )
				{
					CDlgProbDist dlg( m_pTerminal->m_pAirportDB,true,this);
					if (dlg.DoModal() == IDOK)
					{
						pPDEntry = dlg.GetSelectedPD();

						if(pPDEntry == NULL)
						{
							m_wndTreeCtrl.SetItemText(hItem, strName);
							return 0;
						}
						pProbDist = pPDEntry->m_pProbDist;
						ASSERT( pProbDist );	//the pProbDist cannot be null

						strName = pPDEntry->m_csName;
						pProbDist->printDistribution(szBuffer);

					}
				}
				else
				{
					int nCount = pProbDistMan->getCount();
					for( int i=0; i<nCount; i++ )
					{
						pPDEntry = pProbDistMan->getItem( i );
						if(pPDEntry->m_csName == strSel)
							break;
					}

					pProbDist = pPDEntry->m_pProbDist;
					ASSERT( pProbDist );		//the pProbDist cannot be null

					strName = pPDEntry->m_csName;	
					pProbDist->printDistribution(szBuffer);
				} 

				if(pPDEntry == NULL)
				{
					m_wndTreeCtrl.SetItemText(hItem, strName);
					return 0;
				}

				CString strContent = m_wndTreeCtrl.GetItemText(hParItem);
				if (strContent.Find("Passenger") >= 0)
				{
					CAffinity_PassengerTypeData* pPaxData = (CAffinity_PassengerTypeData*)m_wndTreeCtrl.GetItemData(hParItem);
					pPaxData->setNumber(pPDEntry);
					strName = "Number in group: " + strName;
				}
				else
				{
					CAffinity_GroupTypeData* pGroupData = (CAffinity_GroupTypeData*)m_wndTreeCtrl.GetItemData(hParItem);
					pGroupData->setNumber(pPDEntry);
					pGroupData->SetAllNum(false);
					strName = "Number of groups on aircraft: " + strName;
				}

				m_wndTreeCtrl.SetItemText(hItem, strName);				
			}
			else if (strName.Find("Cohesion") >=0)
			{
				int nValue = (int)pCB->GetItemData(pCB->GetCurSel());
				CAffinity_GroupTypeData* pGroupData = (CAffinity_GroupTypeData*)m_wndTreeCtrl.GetItemData(hParItem);
				if (nValue == 0)
				{
					pGroupData->SetCohesion(false);
					strName = "Cohesion: OFF";
				}
				else
				{
					pGroupData->SetCohesion(true);
					strName = "Cohesion: ON";
				}
				m_wndTreeCtrl.SetItemText(hItem, strName);
			}
			else
			{
				int nValue = (int)pCB->GetItemData(pCB->GetCurSel());
				CAffinity_PassengerTypeData* pPaxData = (CAffinity_PassengerTypeData*)m_wndTreeCtrl.GetItemData(hParItem);
				HTREEITEM hGrandItem = m_wndTreeCtrl.GetParentItem(hParItem);
				CAffinity_GroupTypeData* pGroupData = (CAffinity_GroupTypeData*)m_wndTreeCtrl.GetItemData(hGrandItem);
				if (strName.Find("Designation 1") >=0)
				{
					if (PACER == (Designation)nValue && pGroupData->IsPacerExist())
					{
						MessageBox("The Pacer already exist!");
						return 0;
					}

					if (LEADER == (Designation)nValue && pGroupData->IsLeaderExist())
					{
						MessageBox("The Leader already exist!");
						return 0;
					}
					pPaxData->setDesignation1((Designation)nValue);
					strName = "Designation 1: ";
				}
				else
				{
					if (PACER == (Designation)nValue && pGroupData->IsPacerExist())
					{
						MessageBox("The Pacer already exist!");
						return 0;
					}

					if (LEADER == (Designation)nValue && pGroupData->IsLeaderExist())
					{
						MessageBox("The Leader already exist!");
						return 0;
					}
					pPaxData->setDesignation2((Designation)nValue);
					strName = "Designation 2: ";
				}

				if(nValue == 0)
					strName += "LEADER";
				else
					strName += "PACER";

				m_wndTreeCtrl.SetItemText(hItem, strName);
			}
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);	
		}
		break;
	case UM_CEW_SHOW_DIALOGBOX_BEGIN:
		{
			HTREEITEM hItem = (HTREEITEM)wParam;
			HTREEITEM hParItem = m_wndTreeCtrl.GetParentItem(hItem);
			if (hParItem == NULL)			//select flt type
				OnEditAffinityFltType();
			else
				OnEditAffinityPaxType();
		}
		break;
	default:
		break;
	}

	return CXTResizeDialog::DefWindowProc(message,wParam,lParam) ;
}
