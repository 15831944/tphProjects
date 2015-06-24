#include "StdAfx.h"
#include ".\dlgcarryonownership.h"
#include "DlgProbDist.h"
#include "../Database/ADODatabase.h"
#include "../inputs/in_term.h"
#include "../Inputs/probab.h"
#include "../Common/ProDistrubutionData.h"
#include "../Common/AirportDatabase.h"
#include "../Common/ProbDistManager.h"
#include "TermPlanDoc.h"
#include "../InputOnBoard/CarryonOwnership.h"
#include "../InputOnBoard/CarryonOwnershipData.h"
#include "DlgPaxLoadedStatesAndShapes.h"
#include "MFCExControl\SmartEdit.h"
#include "DlgNewPassengerType.h"
#include "../Common/NewPassengerType.h"
#include "DlgNewCarryonItemName.h"
#include "../InputOnboard/Carryon.h"

IMPLEMENT_DYNAMIC(DlgCarryonOwnership, CXTResizeDialog)
DlgCarryonOwnership::DlgCarryonOwnership(CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(DlgCarryonOwnership::IDD, pParent)
	,m_pParentWnd(pParent)
{
	m_nListPreIndex = -1;

	try
	{	
		m_pCarryonOwnership = new CCarryonOwnership;
		m_pCarryonOwnership->ReadData(GetInputTerminal()->inStrDict);
	}
	catch (CADOException* error)
	{
		CString strError = _T("");
		strError.Format("Database operation error: %s",error->ErrorMessage());
		MessageBox(strError);

		delete error;
		error = NULL;
		if (m_pCarryonOwnership)
		{
			delete m_pCarryonOwnership;
			m_pCarryonOwnership = NULL;
		}
		return;
	}

}

DlgCarryonOwnership::~DlgCarryonOwnership(void)
{
	if (m_pCarryonOwnership)
	{
		delete m_pCarryonOwnership;
		m_pCarryonOwnership = NULL;
	}
}
void DlgCarryonOwnership::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ELEMENT, m_listElem);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	DDX_Control(pDX, IDC_BUTTON_EDITMOBILE, m_btnEditName);
	DDX_Control(pDX, IDC_BUTTON_ATTACHSHAPE,m_btnAttachShapes);
	DDX_Control(pDX, IDC_LIST_DATA, m_listctrlData);
}


BEGIN_MESSAGE_MAP(DlgCarryonOwnership, CXTResizeDialog)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL,OnCancel)

	ON_WM_CREATE()

	ON_COMMAND(ID_TOOLBARBUTTONADD, OnToolbarbuttonadd)
	ON_COMMAND(ID_TOOLBARBUTTONDEL, OnToolbarbuttondel)

	ON_LBN_SELCHANGE(IDC_LIST_ELEMENT, OnSelchangeListBoxElement)

	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DATA, OnItemchangedListData)
	ON_NOTIFY(LVN_ENDLABELEDIT,IDC_LIST_DATA,OnSelComboBox)
	ON_BN_CLICKED(IDC_BUTTON_EDITMOBILE, OnButEdititem)
	ON_BN_CLICKED(IDC_BUTTON_ATTACHSHAPE, OnAttachShape)

	ON_MESSAGE(WM_COLLUM_INDEX,OnDbClickListItem)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMobileCountDlg message handlers

BOOL DlgCarryonOwnership::OnInitDialog() 
{
	CXTResizeDialog::OnInitDialog();

	// TODO: Add extra initialization here
	SetWindowText("Carryon Ownership Specification");
	m_btnEditName.SetWindowText("Edit user defined");

	CRect rectFltToolbar;
	GetDlgItem(IDC_STATIC_TOOLBARCONTENTER)->GetWindowRect(&rectFltToolbar);
	GetDlgItem(IDC_STATIC_TOOLBARCONTENTER)->ShowWindow(FALSE);
	ScreenToClient(&rectFltToolbar);
	m_toolbarcontenter.MoveWindow(&rectFltToolbar, true);
	m_toolbarcontenter.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD, TRUE);
	m_toolbarcontenter.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL, FALSE);
	m_toolbarcontenter.GetToolBarCtrl().HideButton(ID_TOOLBARBUTTONEDIT);

	m_btnAttachShapes.ShowWindow(SW_SHOW);

	SetResize(m_toolbarcontenter.GetDlgCtrlID(), SZ_TOP_CENTER, SZ_TOP_CENTER);
	SetResize(IDC_LIST_ELEMENT, SZ_TOP_LEFT , SZ_BOTTOM_CENTER);
	SetResize(IDC_LIST_DATA, SZ_TOP_CENTER , SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_EDITMOBILE, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_BUTTON_ATTACHSHAPE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	InitListBox();
	SetListCtrl();
	InitListCtrl();

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_NONPAXPOSSPEC)->ShowWindow(FALSE);
	GetDlgItem(IDC_BUTTON_HOST)->ShowWindow(FALSE);
	GetDlgItem(IDC_BUTTON_EDITMOBILE)->EnableWindow(FALSE);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void DlgCarryonOwnership::SetListCtrl()
{
	// set list ctrl	
	DWORD dwStyle = m_listctrlData.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listctrlData.SetExtendedStyle( dwStyle );
	m_listctrlData.ModifyStyle(LVS_OWNERDRAWFIXED,0,0);

	LV_COLUMNEX	lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;
	char columnLabel[6][128];

	strcpy( columnLabel[0], "Passenger Type");	
	strcpy( columnLabel[1], "Probability Distribution ( COUNT )");
	strcpy( columnLabel[2], "Impact Major(%)");	
	strcpy( columnLabel[3], "Impact Minor(%)");	
	strcpy( columnLabel[4], "Impact Height(%)");	
	strcpy( columnLabel[5], "Impact Speed(%)");	
	CString csLabel;
	int DefaultColumnWidth[] = { 110,170,110,110,110,110 };
	int nFormat[] = {	LVCFMT_CENTER, LVCFMT_CENTER };
	for( int i=0; i<6; i++ )
	{
		CStringList strList;
		CString s;
		s.LoadString( IDS_STRING_NEWDIST );
		strList.InsertAfter( strList.GetTailPosition(), s );
		int nCount = _g_GetActiveProbMan( GetInputTerminal() )->getCount();
		for( int m=0; m<nCount; m++ )
		{
			CProbDistEntry* pPBEntry = _g_GetActiveProbMan( GetInputTerminal() )->getItem( m );			
			strList.InsertAfter( strList.GetTailPosition(), pPBEntry->m_csName );
		}
		lvc.csList = &strList;
		lvc.pszText = columnLabel[i];
		lvc.cx = DefaultColumnWidth[i];
		if( i == 0 )
			lvc.fmt = LVCFMT_NOEDIT;
		else
			lvc.fmt = LVCFMT_DROPDOWN;
		m_listctrlData.InsertColumn( i, &lvc );
	}

}


InputTerminal* DlgCarryonOwnership::GetInputTerminal()
{
	if (m_pParentWnd == NULL)
		return NULL;

	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return (InputTerminal*)&pDoc->GetTerminal();
}

void DlgCarryonOwnership::OnButtonSave() 
{
	// TODO: Add your control notification handler code here
	try
	{
		CADODatabase::BeginTransaction() ;
		m_pCarryonOwnership->SaveData();
		CADODatabase::CommitTransaction() ;
		m_btnSave.EnableWindow( FALSE );
	}
	catch (CADOException* error)
	{
		CADODatabase::RollBackTransation() ;
		CString strError = _T("");
		strError.Format("Database operation error: %s",error->ErrorMessage());
		MessageBox(strError);

		delete error;
		error = NULL;
		return;
	}
}

void DlgCarryonOwnership::OnCancel() 
{
	// TODO: Add extra cleanup here

	CXTResizeDialog::OnCancel();

}

void DlgCarryonOwnership::OnItemchangedListData(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	if( GetSelectedItem() != -1 )
		m_toolbarcontenter.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL );

	*pResult = 0;
}

int DlgCarryonOwnership::GetSelectedItem()
{
	int nCount = m_listctrlData.GetItemCount();
	for( int i=0; i<nCount; i++ )
	{
		if( m_listctrlData.GetItemState( i, LVIS_SELECTED ) == LVIS_SELECTED )
			return i;
	}
	return -1;
}

int DlgCarryonOwnership::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO: Add your specialized creation code here
	if (!m_toolbarcontenter.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_toolbarcontenter.LoadToolBar(IDR_ADDDELEDITTOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	return 0;
}


void DlgCarryonOwnership::OnToolbarbuttonadd() 
{
	// TODO: Add your command handler code here
	int nSel = m_listElem.GetCurSel();
	if(nSel < 0)
		return;
	CCarryonOwnershipItem* pItem = (CCarryonOwnershipItem*)m_listElem.GetItemData(nSel);
	if (pItem == NULL)
	{
		return;
// 		pItem = new CCarryonOwnershipItem;
// 		pItem->setItemType((CarryonItem)nSel);
// 		CString strName;
// 		int nLength = m_listElem.GetTextLen(nSel);
// 		m_listElem.GetText(nSel,strName.GetBuffer(nLength));
// 		strName.ReleaseBuffer();
// 		pItem->setItemName(strName);
// 		m_pCarryonOwnership->AddData(pItem);
// 		m_listElem.SetItemData(nSel,(DWORD_PTR)pItem);
	}
	CarryonOwnershipData* pData = new CarryonOwnershipData(GetInputTerminal()->inStrDict);
	CPassengerType* pPaxtype = pData->getPaxType();
	CDlgNewPassengerType dlg(pPaxtype,GetInputTerminal(),this);

	if (pItem->FindData(*(pData->getPaxType())))
	{
		MessageBox(_T("The passenger type already exists!"), _T("Error"), MB_OK | MB_ICONWARNING);
		delete pData;
		return;
	}





	if (dlg.DoModal() == IDOK)
	{
		CString strPaxType = pPaxtype->PrintStringForShow();

		if (!pItem->findExistName(strPaxType))
		{
			int nCount = m_listctrlData.GetItemCount();

			m_listctrlData.InsertItem(nCount, strPaxType);
			//CString strDistType = "Select a probability distribution...";

			m_listctrlData.SetItemText(nCount, 1, pData->getCount()->getDistName());
			m_listctrlData.SetItemText(nCount, 2, pData->getMajor()->getDistName());
			m_listctrlData.SetItemText(nCount, 3, pData->getMinor()->getDistName());
			m_listctrlData.SetItemText(nCount, 4, pData->getHeight()->getDistName());
			m_listctrlData.SetItemText(nCount, 5, pData->getSpeed()->getDistName());
			m_listctrlData.SetItemData(nCount,(DWORD_PTR)pData);
			pItem->AddData(pData);

			m_listctrlData.SetItemState(nCount, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);

			GetDlgItem(IDC_BTN_MAINTENANCE_SAVE)->EnableWindow(TRUE);
			return;
		}
		else
		{
			::AfxMessageBox(_T("The new passenger type is exist!"));
		}
	}
	if (pData)
	{
		delete pData;
		pData = NULL;
	}
}

void DlgCarryonOwnership::OnToolbarbuttondel() 
{
	// TODO: Add your command handler code here
	int nSel = m_listElem.GetCurSel();
	if (nSel <0)
		return;

	CCarryonOwnershipItem* pItem = (CCarryonOwnershipItem*)m_listElem.GetItemData(nSel);
	if (pItem == NULL)
		return;

	nSel =  -1;

	POSITION pos = m_listctrlData.GetFirstSelectedItemPosition();
	if (pos)
		nSel = m_listctrlData.GetNextSelectedItem(pos);

	if (nSel == -1)
		return;

	CarryonOwnershipData* pDelData = (CarryonOwnershipData*)m_listctrlData.GetItemData(nSel);
	if (pDelData)
		pItem->DelData(pDelData);

	m_listctrlData.DeleteItemEx(nSel);

	int nCount = m_listctrlData.GetItemCount();
	if(nCount > 0)
		m_listctrlData.SetItemState(nCount -1, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);

	GetDlgItem(IDC_BTN_MAINTENANCE_SAVE)->EnableWindow(TRUE);
}

void DlgCarryonOwnership::InitListBox()
{

	m_listElem.ResetContent();

	for( int i=PURSE; i< CARRYON_NUM; i++ )
	{
		CCarryonOwnershipItem* pItem = m_pCarryonOwnership->GetItemByType((EnumCarryonType)i);
		if (pItem)
		{
			int iIndex = m_listElem.AddString(CarryonItemName[i]);
			ASSERT( iIndex!=LB_ERR );	//only judge error when insert string to listbox
			m_listElem.SetItemData(iIndex, (DWORD_PTR)pItem); 
		}
		else
		{
			int iIndex = m_listElem.AddString(CarryonItemName[i]);
			ASSERT( iIndex!=LB_ERR );	//only judge error when insert string to listbox
			pItem =new CCarryonOwnershipItem;
			pItem->setItemType((EnumCarryonType)i);
			m_pCarryonOwnership->AddData(pItem);
			m_listElem.SetItemData(iIndex, (DWORD_PTR)pItem); 
		}

	}

	m_listElem.SetCurSel(0);
}

void DlgCarryonOwnership::InitListCtrl()
{
	int nIdx = m_listElem.GetCurSel();
	if (nIdx >=0)
	{
		CCarryonOwnershipItem* pItem = (CCarryonOwnershipItem*)m_listElem.GetItemData(nIdx);
		if (pItem == NULL)
			return;

		int nCount = pItem->getDataCount();
		for (int i = 0; i < nCount; i++)
		{
			CarryonOwnershipData* pData = pItem->getData(i);
			CString strPaxType = pData->getPaxType()->PrintStringForShow();
			m_listctrlData.InsertItem(i,strPaxType);
			m_listctrlData.SetItemText(i,1, pData->getCount()->getPrintDist());
			m_listctrlData.SetItemText(i,2, pData->getMajor()->getPrintDist());
			m_listctrlData.SetItemText(i,3, pData->getMinor()->getPrintDist());
			m_listctrlData.SetItemText(i,4, pData->getHeight()->getPrintDist());
			m_listctrlData.SetItemText(i,5, pData->getSpeed()->getPrintDist());
			m_listctrlData.SetItemData(i,(DWORD_PTR)pData);
		}
	}
}

void DlgCarryonOwnership::OnSelchangeListBoxElement() 
{
	// TODO: Add your control notification handler code here

	m_listctrlData.DeleteAllItems();
	InitListCtrl();
	m_toolbarcontenter.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, FALSE );

	//enable user define button
	int nIdx = m_listElem.GetCurSel();

	if (nIdx < 6)
	{
		GetDlgItem(IDC_BUTTON_EDITMOBILE)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_EDITMOBILE)->EnableWindow(TRUE);
	}

}

void  DlgCarryonOwnership::OnSelComboBox(NMHDR*   pNMHDR,   LRESULT*   pResult)     
{     	

	LV_DISPINFO* dispinfo = (LV_DISPINFO * )pNMHDR;
	if(m_nColumnSel == 0 || m_nRowSel <0)
		return;
	*pResult = 0;
	if(!dispinfo)
		return;

	// TODO: Add your control notification handler code here
	CCarryonOwnershipItem* pItem = (CCarryonOwnershipItem*)m_listElem.GetItemData(m_listElem.GetCurSel());
	if (pItem == NULL)
		return;

	CarryonOwnershipData* pData = pItem->getData(m_nRowSel);
	CString strSel;
	strSel = dispinfo->item.pszText;

	char szBuffer[1024] = {0};
	ProbabilityDistribution* pProbDist = NULL;
	CProbDistManager* pProbDistMan = GetInputTerminal()->m_pAirportDB->getProbDistMan();
	if(strSel == "NEW PROBABILITY DISTRIBUTION" )
	{
		CDlgProbDist dlg(GetInputTerminal()->m_pAirportDB,true,this);
		if (dlg.DoModal() != IDOK)
		{
			CString strDistName = "";
			
				switch(m_nColumnSel)
			{
				case 1:
					strDistName = pData->getCount()->getPrintDist();
					break;
				case 2:
					strDistName = pData->getMajor()->getPrintDist();
					break;
				case 3:
					strDistName = pData->getMinor()->getPrintDist();
					break;
				case 4:
					strDistName = pData->getHeight()->getPrintDist();
					break;
				case 5:
					strDistName = pData->getSpeed()->getPrintDist();
					break;
				default:
					break;
			}
			m_listctrlData.SetItemText(m_nRowSel,m_nColumnSel,strDistName);
			return;
		}


		CProbDistEntry* pPDEntry = dlg.GetSelectedPD();

		if(pPDEntry == NULL)
			return;

		pProbDist = pPDEntry->m_pProbDist;
		CString strDistName = pPDEntry->m_csName;

		pProbDist->printDistribution(szBuffer);
		switch(m_nColumnSel)
		{
		case 1:
			pData->getCount()->SetProDistrubution(pPDEntry);
			break;
		case 2:
			pData->getMajor()->SetProDistrubution(pPDEntry);
			break;
		case 3:
			pData->getMinor()->SetProDistrubution(pPDEntry);
		    break;
		case 4:
			pData->getHeight()->SetProDistrubution(pPDEntry);
		    break;
		case 5:
			pData->getSpeed()->SetProDistrubution(pPDEntry);
			break;
		default:
		    break;
		}
		m_listctrlData.SetItemText(m_nRowSel,m_nColumnSel,strDistName);

	}
	else
	{
		CProbDistEntry* pPDEntry = NULL;

		int nCount = pProbDistMan->getCount();
		for( int i=0; i<nCount; i++ )
		{
			pPDEntry = pProbDistMan->getItem( i );
			if(pPDEntry->m_csName == strSel)
				break;
		}

		pProbDist = pPDEntry->m_pProbDist;
		ASSERT( pProbDist );		//pPorbDist cannot be null

		CString strDistName = pPDEntry->m_csName;	
		pProbDist->printDistribution(szBuffer);

		switch(m_nColumnSel)
		{
		case 1:
			pData->getCount()->SetProDistrubution(pPDEntry);
			break;
		case 2:
			pData->getMajor()->SetProDistrubution(pPDEntry);
			break;
		case 3:
			pData->getMinor()->SetProDistrubution(pPDEntry);
			break;
		case 4:
			pData->getHeight()->SetProDistrubution(pPDEntry);
			break;
		case 5:
			pData->getSpeed()->SetProDistrubution(pPDEntry);
			break;
		default:
			break;
		}
		m_listctrlData.SetItemText(m_nRowSel,m_nColumnSel,strDistName);

	} 
}

LRESULT DlgCarryonOwnership::OnDbClickListItem( WPARAM wparam, LPARAM lparam)
{
	m_nRowSel = (int)wparam;
	m_nColumnSel = (int)lparam;

	return NULL;
}


void DlgCarryonOwnership::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnButtonSave();
	CXTResizeDialog::OnOK();
}

void DlgCarryonOwnership::OnButEdititem() 
{
/*
	int nIdx = m_listElem.GetCurSel();
	if (nIdx <0)
		return;

	if (nIdx <6)
	{
		m_listElem.SetCurSel(nIdx);
		return;
	}

	CCarryonOwnershipItem* pItem = (CCarryonOwnershipItem*)m_listElem.GetItemData(nIdx);

	if (pItem == NULL)
	{
		pItem = new CCarryonOwnershipItem;
		pItem->setItemType((EnumCarryonType)nIdx);
		m_pCarryonOwnership->AddData(pItem);
	}

	//CRect rectItem;
	//CRect rectLB;
	//m_listElem.GetWindowRect( &rectLB );
	//ScreenToClient( &rectLB );
	//m_listElem.GetItemRect( nIdx, &rectItem );

	//rectItem.OffsetRect( rectLB.left+2, rectLB.top+1 );

	//rectItem.right += 1;
	//rectItem.bottom += 4;

	//CString csItemText;
	//m_listElem.GetText( nIdx, csItemText );
	//CSmartEdit* pEdit = new CSmartEdit( csItemText );
	//DWORD dwStyle = ES_LEFT | WS_BORDER|WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL;
	//pEdit->Create( dwStyle, rectItem, this, IDC_IPEDIT );	

	CString strName = "";
	//int nLength = m_listElem.GetTextLen(nIdx);
	//m_listElem.GetText(nIdx,strName.GetBuffer(nLength));
	//strName.ReleaseBuffer();
	DlgNewCarryonItemName dlg;
	if (dlg.DoModal() == IDOK)
	{
		strName = dlg.m_strName;
		pItem->setItemName(strName);

		m_listElem.SetItemData(nIdx,NULL);
		m_listElem.DeleteString(nIdx);

		m_listElem.InsertString(nIdx, strName);
		m_listElem.SetItemData(nIdx,(DWORD_PTR)pItem);

		m_btnEditName.EnableWindow( TRUE );
	}
	m_listElem.SetCurSel(nIdx);*/

}

void DlgCarryonOwnership::OnAttachShape()
{
	DlgPaxLoadedStatesAndShapes dlg(GetInputTerminal(),m_pCarryonOwnership);
	dlg.DoModal();

}
