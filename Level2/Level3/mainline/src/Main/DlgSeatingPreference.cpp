// DlgSeatingPreference.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgSeatingPreference.h"
#include "../Common/ProbDistManager.h"
#include "../Common/AirportDatabase.h"
#include "../Common/NewPassengerType.h"
#include "DlgNewPassengerType.h"
#include "../Inputs/IN_TERM.H"
#include "../Common/WinMsg.h"

// CDlgSeatingPreference dialog
const char* szSeat[] = {"Bulk head","Emergency exit","Aisle","Window","Towards front","Towards back","Towards middle"\
,"Upper deck","Lower deck","Middle deck","","Mid block"}; // the same sequence as 'enum SeatType'

IMPLEMENT_DYNAMIC(CDlgSeatingPreference, CXTResizeDialog)
CDlgSeatingPreference::CDlgSeatingPreference(InputTerminal * pInterm,int nProjID,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgSeatingPreference::IDD, pParent)
	,m_pInterm(pInterm)
	,m_nPorjID(nProjID)
	,m_seatingPreferenceList(pInterm->inStrDict)
{
}

CDlgSeatingPreference::~CDlgSeatingPreference()
{
}

void CDlgSeatingPreference::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DATA, m_wndListCtrl);
}


BEGIN_MESSAGE_MAP(CDlgSeatingPreference, CXTResizeDialog)
	ON_WM_CREATE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DATA, OnSelChangePassengerType)
	ON_NOTIFY(LVN_ENDLABELEDIT,IDC_LIST_DATA,OnEndlabeledit)
	ON_COMMAND(ID_MODEL_NEW,OnAddPaxType)
	ON_COMMAND(ID_MODEL_DELETE,OnRemovePaxType)
	ON_COMMAND(ID_MODEL_EDIT,OnEditPaxType)
	ON_MESSAGE(WM_NOEDIT_DBCLICK, OnDBClick)
	ON_BN_CLICKED(IDC_BUTTON_SAVE,OnSave)
END_MESSAGE_MAP()


// CDlgSeatingPreference message handlers
BOOL CDlgSeatingPreference::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	SetResize(IDC_STATIC_GROUP,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(m_wndToolbar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_LIST_DATA,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);

	m_seatingPreferenceList.ReadData(m_nPorjID,m_pInterm->inStrDict);
	OnInitToolbar();
	OnInitListCtrl();
	DisplayData();
	OnUpdateToolbar();
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

int CDlgSeatingPreference::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolbar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP|CBRS_TOOLTIPS)||
		!m_wndToolbar.LoadToolBar(IDR_DATABASE_MODEL))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	return 0;
}

void CDlgSeatingPreference::OnInitToolbar()
{
	CRect rect;
	m_wndListCtrl.GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.bottom = rect.top - 6;
	rect.top = rect.bottom - 22;
	rect.left = rect.left + 2;
	m_wndToolbar.MoveWindow(&rect,true);
	m_wndToolbar.ShowWindow(SW_SHOW);

	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_MODEL_NEW,TRUE);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_MODEL_DELETE,TRUE);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_MODEL_EDIT,TRUE);
}

void CDlgSeatingPreference::OnOK()
{
   OnSave() ;
   CXTResizeDialog::OnOK();
}

void CDlgSeatingPreference::DisplayData()
{
	m_wndListCtrl.DeleteAllItems();
	int nCount = m_seatingPreferenceList.getCount();
	for (int i = 0; i < nCount; i++)
	{
		CSeatingPreferenceItem* pItem = m_seatingPreferenceList.getItem(i);
		m_wndListCtrl.InsertItem(i,pItem->getMobileElemCnt().PrintStringForShow());
		m_wndListCtrl.SetItemText(i,1,szSeat[pItem->getPriority1()]);
		m_wndListCtrl.SetItemText(i,2,szSeat[pItem->getPriority2()]);
		m_wndListCtrl.SetItemText(i,3,szSeat[pItem->getPriority3()]);
		m_wndListCtrl.SetItemText(i,4,szSeat[pItem->getPriority4()]);
		m_wndListCtrl.SetItemText(i,5,szSeat[pItem->getPriority5()]);
		m_wndListCtrl.SetItemText(i,6,szSeat[pItem->getPriority6()]);
		m_wndListCtrl.SetItemText(i,7,szSeat[pItem->getPriority7()]);
		m_wndListCtrl.SetItemText(i,8,szSeat[pItem->getPriority8()]);
		m_wndListCtrl.SetItemText(i,9,szSeat[pItem->getPriority9()]);
		m_wndListCtrl.SetItemText(i,10,szSeat[pItem->getPriority10()]);
		m_wndListCtrl.SetItemData(i,(DWORD_PTR)pItem);
	}
}

void CDlgSeatingPreference::OnSave()
{
	if (m_seatingPreferenceList.IsInvalid())
	{
		try
		{
			CADODatabase::BeginTransaction() ;
			m_seatingPreferenceList.SaveData(m_nPorjID);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
			CADODatabase::CommitTransaction() ;
		}
		catch (CADOException e)
		{
			CADODatabase::RollBackTransation() ;
		}
		
	}
	else
	{
		AfxMessageBox(_T("Priority is Invalid"));
	}
	
}

void CDlgSeatingPreference::OnUpdateToolbar()
{
	int nSel = m_wndListCtrl.GetCurSel();
	if (nSel != LB_ERR)
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_MODEL_NEW,TRUE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_MODEL_DELETE,TRUE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_MODEL_EDIT,TRUE);
	}
	else
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_MODEL_NEW,TRUE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_MODEL_DELETE,FALSE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_MODEL_EDIT,FALSE);
	}
}

void CDlgSeatingPreference::OnSelChangePassengerType(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	OnUpdateToolbar();
}

void CDlgSeatingPreference::OnInitListCtrl()
{
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle( dwStyle );

	char* columnLabel[] = {	"Passenger Type","Priority 1", "Priority 2","Priority 3","Priority 4",\
		"Priority 5","Priority 6","Priority 7","Priority 8","Priority 9","Priority 10"};
	int DefaultColumnWidth[] = { 150,100,100,100,100,100,100,100,100,100,100};

	int nColFormat[] = 
	{	
		LVCFMT_NOEDIT,
			LVCFMT_DROPDOWN,
			LVCFMT_DROPDOWN,
			LVCFMT_DROPDOWN,
			LVCFMT_DROPDOWN,
			LVCFMT_DROPDOWN,
			LVCFMT_DROPDOWN,
			LVCFMT_DROPDOWN,
			LVCFMT_DROPDOWN,
			LVCFMT_DROPDOWN,
			LVCFMT_DROPDOWN
	};

	CStringList strList;
	CProbDistManager* pProbDistMan = 0;	
	CProbDistEntry* pPDEntry = 0;
	
	for( int i=0; i< 10; i++ )
	{
		strList.AddTail(szSeat[i]);
	}
	strList.AddTail(szSeat[11]);// add 'mid block' to the list

	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;

	for (int i = 0; i < 11; i++)
	{
		lvc.fmt = nColFormat[i];
		lvc.pszText = columnLabel[i];
		lvc.cx = DefaultColumnWidth[i];
		lvc.csList = &strList;
		m_wndListCtrl.InsertColumn(i, &lvc);
	}
}

void CDlgSeatingPreference::OnAddPaxType()
{
	CPassengerType paxType(m_pInterm->inStrDict);
	CDlgNewPassengerType dlg(&paxType,m_pInterm);
	if (dlg.DoModal() == IDOK)
	{
		CString strPaxType = _T("");
		strPaxType = paxType.GetStringForDatabase();
		int nCount = m_seatingPreferenceList.getCount();
		for (int i=0; i<nCount; i++)
		{
			CSeatingPreferenceItem* pData = m_seatingPreferenceList.getItem(i);
			CString strType = _T("");
			strType = pData->getMobileElemCnt().GetStringForDatabase();
			if (!strType.Compare(strPaxType))
			{
				MessageBox(_T("The Passenger Type already exists!"));

				return;
			}
		}

		CSeatingPreferenceItem* pItem = new CSeatingPreferenceItem(m_pInterm->inStrDict);
		pItem->setMobileElemCnt(paxType);
		m_seatingPreferenceList.addItem(pItem);
		nCount = m_wndListCtrl.GetItemCount();

		CString strValue = _T("");
		m_wndListCtrl.InsertItem(nCount,pItem->getMobileElemCnt().PrintStringForShow());
		m_wndListCtrl.SetItemText(nCount,1,szSeat[pItem->getPriority1()]);
		m_wndListCtrl.SetItemText(nCount,2,szSeat[pItem->getPriority2()]);
		m_wndListCtrl.SetItemText(nCount,3,szSeat[pItem->getPriority3()]);
		m_wndListCtrl.SetItemText(nCount,4,szSeat[pItem->getPriority4()]);
		m_wndListCtrl.SetItemText(nCount,5,szSeat[pItem->getPriority5()]);
		m_wndListCtrl.SetItemText(nCount,6,szSeat[pItem->getPriority6()]);
		m_wndListCtrl.SetItemText(nCount,7,szSeat[pItem->getPriority7()]);
		m_wndListCtrl.SetItemText(nCount,8,szSeat[pItem->getPriority8()]);
		m_wndListCtrl.SetItemText(nCount,9,szSeat[pItem->getPriority9()]);
		m_wndListCtrl.SetItemText(nCount,10,szSeat[pItem->getPriority10()]);
		m_wndListCtrl.SetItemData(nCount,(DWORD_PTR)pItem);
		m_wndListCtrl.SetItemState( nCount,LVIS_SELECTED,LVIS_SELECTED );
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
}

void CDlgSeatingPreference::OnRemovePaxType()
{
	int nSel = m_wndListCtrl.GetCurSel();
	if (nSel != LB_ERR)
	{
		CSeatingPreferenceItem* pItem = (CSeatingPreferenceItem*)m_wndListCtrl.GetItemData(nSel);
		m_seatingPreferenceList.removeItem(pItem);
		m_wndListCtrl.DeleteItem(nSel);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
}

void CDlgSeatingPreference::OnEditPaxType()
{
	CPassengerType paxType(m_pInterm->inStrDict);
	CDlgNewPassengerType dlg(&paxType,m_pInterm);
	int nItem = m_wndListCtrl.GetCurSel();
	if (dlg.DoModal() == IDOK)
	{
		CString strPaxType = _T("");
		strPaxType = paxType.GetStringForDatabase();
		int nCount = m_seatingPreferenceList.getCount();
		CSeatingPreferenceItem* pItem = (CSeatingPreferenceItem*)m_wndListCtrl.GetItemData(nItem);
		for (int i=0; i<nCount; i++)
		{
			CSeatingPreferenceItem* pData = m_seatingPreferenceList.getItem(i);
			CString strType = _T("");
			strType = pData->getMobileElemCnt().GetStringForDatabase();
			if (!strType.Compare(strPaxType) && pItem != pData)
			{
				MessageBox(_T("The Passenger Type already exists!"));

				return;
			}
		}

		pItem->setMobileElemCnt(paxType);
		DisplayData();
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
}

LRESULT CDlgSeatingPreference::OnDBClick(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your control notification handler code here
	CPassengerType paxType(m_pInterm->inStrDict);
	CDlgNewPassengerType dlg(&paxType,m_pInterm);
	int nItem = (int)wParam;
	if (dlg.DoModal() == IDOK)
	{
		CString strPaxType = _T("");
		strPaxType = paxType.GetStringForDatabase();
		int nCount = m_seatingPreferenceList.getCount();
		CSeatingPreferenceItem* pItem = (CSeatingPreferenceItem*)m_wndListCtrl.GetItemData(nItem);
		for (int i=0; i<nCount; i++)
		{
			CSeatingPreferenceItem* pData =m_seatingPreferenceList.getItem(i);
			CString strType = _T("");
			strType = pData->getMobileElemCnt().GetStringForDatabase();
			if (!strType.Compare(strPaxType) && pItem != pData)
			{
				MessageBox(_T("The Passenger Type already exists!"));

				return FALSE;
			}
		}

		pItem->setMobileElemCnt(paxType);
		DisplayData();
	}
	return TRUE;
}

void CDlgSeatingPreference::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* dispinfo = (LV_DISPINFO * )pNMHDR;
	*pResult = 0;

	CSeatingPreferenceItem* pItem = (CSeatingPreferenceItem*)m_wndListCtrl.GetItemData(dispinfo->item.iItem);
	if(!pItem)return;
	CString strSel;
	strSel = dispinfo->item.pszText;
	SeatType emType;
	emType = getSeatTypeFromString(strSel);
	switch(dispinfo->item.iSubItem)
	{
	case 1:
		{
			pItem->setPriority1(emType);
		}
		break;
	case 2:
		{
			pItem->setPriority2(emType);
		}
		break;
	case 3:
		{
			pItem->setPriority3(emType);
		}
		break;
	case 4:
		{
			pItem->setPriority4(emType);
		}
		break;
	case 5:
		{
			pItem->setPriority5(emType);
		}
		break;
	case 6:
		{
			pItem->setPriority6(emType);
		}
		break;
	case 7:
		{
			pItem->setPriority7(emType);
		}
		break;
	case 8:
		{
			pItem->setPriority8(emType);
		}
		break;
	case 9:
		{
			pItem->setPriority9(emType);
		}
		break;
	case 10:
		{
			pItem->setPriority10(emType);
		}
	default:
		break;
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

SeatType CDlgSeatingPreference::getSeatTypeFromString(CString szData)
{
	//for (int i =0; i < 10; i++)
	for (int i =0; i < 12; i++)
	{
		if (!szData.CompareNoCase(szSeat[i]))
		{
			return (SeatType)i;
		}
	}
	return NO_TYPE;
}