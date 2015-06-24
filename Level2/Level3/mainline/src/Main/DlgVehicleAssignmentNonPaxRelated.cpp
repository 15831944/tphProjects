// VehicleAssignmentNonPaxRelated.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgVehicleAssignmentNonPaxRelated.h"
#include "Common/WinMsg.h"
#include "../Landside/NewVehicleAssignNonPaxRelated.h"
#include "DlgSelectLandsideVehicleType.h"
#include "DlgProbDist.h"
#include "landside/InputLandside.h"
#include "../Inputs/IN_TERM.H"
#include "../common/ProbDistEntry.h"
#include "../Inputs/PROBAB.H"
#include "../common/ProbDistManager.h"
#include "DlgSelectLandsideObject.h"


namespace
{
	const UINT ID_VEHICLE_NEW = 10;
	const UINT ID_VEHICLE_DEL = 11;

	const UINT ID_MENU_ADD = 15;
	const UINT ID_MENU_DEL = 16;
}
// CVehicleAssignmentNonPaxRelated dialog
static const char* strDailyBuffer[] = {"No","Yes"};

IMPLEMENT_DYNAMIC(CDlgVehicleAssignmentNonPaxRelated, CXTResizeDialog)
CDlgVehicleAssignmentNonPaxRelated::CDlgVehicleAssignmentNonPaxRelated(InputLandside* pInLandside, CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgVehicleAssignmentNonPaxRelated::IDD, pParent)
	,m_pInLandside(pInLandside)
{
	m_pAssignment = NULL;
}

CDlgVehicleAssignmentNonPaxRelated::~CDlgVehicleAssignmentNonPaxRelated()
{
}

void CDlgVehicleAssignmentNonPaxRelated::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_TREE_DATA,m_wndTreeCtrl);
}


BEGIN_MESSAGE_MAP(CDlgVehicleAssignmentNonPaxRelated, CXTResizeDialog)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_SAVE,OnBtnSave)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY(TVN_SELCHANGED,IDC_TREE_DATA,OnLvnItemchangedList)
	ON_COMMAND(ID_VEHICLE_NEW,OnCmdNewItem)
	ON_COMMAND(ID_VEHICLE_DEL,OnCmdDeleteItem)
	ON_COMMAND(ID_MENU_ADD,OnAddObject)
	ON_COMMAND(ID_MENU_DEL,OnDeleteObject)
END_MESSAGE_MAP()


// CVehicleAssignmentNonPaxRelated message handlers
int CDlgVehicleAssignmentNonPaxRelated::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP )
		|| !m_wndToolBar.LoadToolBar(IDR_ADDDELTOOLBAR))
	{
		return -1;
	}

	return 0;
}

BOOL CDlgVehicleAssignmentNonPaxRelated::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	//new assignment
	m_pAssignment = new NewVehicleAssignNonPaxRelated;
	m_pAssignment->ReadData(-1);

	SetTreeContent();
	OnInitToolbar();
	UpdateToolBarState();

	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_GROUP, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(m_wndToolBar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_RIGHT);
	SetResize(IDC_TREE_DATA, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	SetModified( FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
void CDlgVehicleAssignmentNonPaxRelated::OnInitToolbar()
{
	CRect rect;
	m_wndTreeCtrl.GetWindowRect( &rect );
	ScreenToClient( &rect );
	rect.top -= 26;
	rect.bottom = rect.top + 22;
	rect.left += 4;
	m_wndToolBar.MoveWindow(&rect);

	CToolBarCtrl& spotBarCtrl = m_wndToolBar.GetToolBarCtrl();
	spotBarCtrl.SetCmdID(0,ID_VEHICLE_NEW);
	spotBarCtrl.SetCmdID(1,ID_VEHICLE_DEL);

	m_wndToolBar.GetToolBarCtrl().EnableButton( ID_VEHICLE_NEW, TRUE);
	m_wndToolBar.GetToolBarCtrl().EnableButton( ID_VEHICLE_DEL, FALSE);
}
void CDlgVehicleAssignmentNonPaxRelated::UpdateToolBarState()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem == NULL)
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_VEHICLE_NEW,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_VEHICLE_DEL,FALSE);
	}
	else
	{
		TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
		if (pNodeData == NULL)
		{
			m_wndToolBar.GetToolBarCtrl().EnableButton(ID_VEHICLE_NEW,TRUE);
			m_wndToolBar.GetToolBarCtrl().EnableButton(ID_VEHICLE_DEL,FALSE);
		}
		else
		{
			if (pNodeData->m_emType == TreeNodeData::Vehicle_Node)
			{
				m_wndToolBar.GetToolBarCtrl().EnableButton(ID_VEHICLE_NEW,TRUE);
				m_wndToolBar.GetToolBarCtrl().EnableButton(ID_VEHICLE_DEL,TRUE);
			}
			else
			{
				m_wndToolBar.GetToolBarCtrl().EnableButton(ID_VEHICLE_NEW,TRUE);
				m_wndToolBar.GetToolBarCtrl().EnableButton(ID_VEHICLE_DEL,FALSE);
			}
		}
	}
}

void CDlgVehicleAssignmentNonPaxRelated::SetTreeContent()
{
	m_wndTreeCtrl.DeleteAllItems();
	m_wndTreeCtrl.SetImageList(m_wndTreeCtrl.GetImageList(TVSIL_NORMAL),TVSIL_NORMAL);
	for (int i = 0; i < m_pAssignment->GetItemCount(); i++)
	{
		VehicleNonPaxRelatedData* pVehicle = m_pAssignment->GetItem(i);
		InsertVehicleItem(TVI_ROOT,pVehicle);
	}

	HTREEITEM hItem = m_wndTreeCtrl.GetRootItem();
	if (hItem)
	{
		m_wndTreeCtrl.SelectSetFirstVisible(hItem);
	}
}
void CDlgVehicleAssignmentNonPaxRelated::OnCmdNewItem()
{
	CDlgSelectLandsideVehicleType dlg;
	if(dlg.DoModal() == IDOK)
	{
		CString strGroupName = dlg.GetName();
		if (!strGroupName.IsEmpty() && !m_pAssignment->ExistSameVehicleType(strGroupName))//non empty
		{
			VehicleNonPaxRelatedData* pVehicle = new VehicleNonPaxRelatedData();
			pVehicle->SetVehicleType(strGroupName);
			m_pAssignment->AddNewItem(pVehicle);
			InsertVehicleItem(TVI_ROOT,pVehicle);
			SetModified();
		}
	}
}


void CDlgVehicleAssignmentNonPaxRelated::OnBtnSave()
{
	try
	{
		CADODatabase::BeginTransaction();
		m_pAssignment->SaveData(-1);
		CADODatabase::CommitTransaction();
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
		CADODatabase::RollBackTransation();
		MessageBox(_T("Save database error"),_T("ERROR"),MB_OK);
		return;
	}
	SetModified(FALSE);
}

void CDlgVehicleAssignmentNonPaxRelated::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	try
	{
		CADODatabase::BeginTransaction();
		m_pAssignment->SaveData(-1);
		CADODatabase::CommitTransaction();
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
		CADODatabase::RollBackTransation();
		MessageBox(_T("Save database error"),_T("ERROR"),MB_OK);
		return;
	}
	CXTResizeDialog::OnOK();
}

void CDlgVehicleAssignmentNonPaxRelated::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CXTResizeDialog::OnCancel();
}


void CDlgVehicleAssignmentNonPaxRelated::SetModified( BOOL bModified /*= TRUE*/ )
{
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(bModified);
}

LRESULT CDlgVehicleAssignmentNonPaxRelated::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message)
	{
	case UM_CEW_SHOW_DIALOGBOX_BEGIN:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			COOLTREE_NODE_INFO* pCNI = (COOLTREE_NODE_INFO*)m_wndTreeCtrl.GetItemNodeInfo(hItem);
			TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
			if (pNodeData)
			{
				if (pNodeData->m_emType == TreeNodeData::Entry_Node)
				{
					VehicleNonPaxRelatedEntry* pEntry= (VehicleNonPaxRelatedEntry*)(pNodeData->m_dwData);
					if (pEntry)
					{
						CDlgSelectLandsideObject* pDlgSelectLandsideObject = new CDlgSelectLandsideObject(&m_pInLandside->getObjectList(),this);
						pDlgSelectLandsideObject->AddObjType(ALT_LEXT_NODE);
						pCNI->pEditWnd = pDlgSelectLandsideObject;
					}
				}
				else if (pNodeData->m_emType == TreeNodeData::Vehicle_Node)
				{
					VehicleNonPaxRelatedData* pVehicle = (VehicleNonPaxRelatedData*)(pNodeData->m_dwData);
					if (pVehicle)
					{
						CDlgSelectLandsideVehicleType* pDlgVehicleType = new CDlgSelectLandsideVehicleType();
						pCNI->pEditWnd = pDlgVehicleType;
					}
				}
				else if (pNodeData->m_emType == TreeNodeData::Position_Node)
				{
					VehicleNonPaxRelatedPosition* pPosition= (VehicleNonPaxRelatedPosition*)(pNodeData->m_dwData);
					if (pPosition)
					{
						CDlgSelectLandsideObject* pDlgSelectLandsideObject = new CDlgSelectLandsideObject(&m_pInLandside->getObjectList(),this);
						pDlgSelectLandsideObject->AddObjType(ALT_LPARKINGLOT);
						pCNI->pEditWnd = pDlgSelectLandsideObject;
					}
				}
			}

		}
		break;
	case UM_CEW_SHOW_DIALOGBOX_END:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			COOLTREE_NODE_INFO* pCNI = (COOLTREE_NODE_INFO*)lParam;
			TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
			if (pNodeData)
			{
				if (pNodeData->m_emType == TreeNodeData::Entry_Node)
				{
					CDlgSelectLandsideObject* pDlg = (CDlgSelectLandsideObject*)pCNI->pEditWnd;
					VehicleNonPaxRelatedEntry* pEntry = (VehicleNonPaxRelatedEntry*)(pNodeData->m_dwData);
					if (pEntry && !(pEntry->GetEntry() == pDlg->getSelectObject()))
					{
						HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
						if (hParentItem)
						{
							TreeNodeData* pSuperNode = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
							if (pSuperNode && pSuperNode->m_emType == TreeNodeData::EntryRoot_Node)
							{
								VehicleNonPaxRelatedPosition* pPosition = (VehicleNonPaxRelatedPosition*)(pSuperNode->m_dwData);

								if (!pPosition->ExistSameEntry(pDlg->getSelectObject().GetIDString()))
								{
									LandsideFacilityLayoutObjectList* pLayoutList = &m_pInLandside->getObjectList();
									if (pLayoutList)
									{
										if (pLayoutList->getObjectByName(pDlg->getSelectObject()) == NULL)
										{
											MessageBox(_T("Can't select group of entry or exit node!!."),_T("Warning"),MB_OK|MB_ICONINFORMATION);
										}
										else
										{
											pEntry->SetEntry(pDlg->getSelectObject());
											m_wndTreeCtrl.SetItemText(hItem,pEntry->GetEntry().GetIDString());
											SetModified();
										}
									}
								}
								else
								{
									MessageBox(_T("Exist the same Entry or Exit node.!!"),_T("Warning"),MB_OK);
								}
							}
						}
						
					}
					
				}
				else if (pNodeData->m_emType == TreeNodeData::Vehicle_Node)
				{
					CDlgSelectLandsideVehicleType* pDlg = (CDlgSelectLandsideVehicleType*)pCNI->pEditWnd;
					VehicleNonPaxRelatedData* pVehicle = (VehicleNonPaxRelatedData*)(pNodeData->m_dwData);
					if (pVehicle && pVehicle->GetVehicleType().Compare(pDlg->GetName()) != 0)
					{
						if (m_pAssignment && !m_pAssignment->ExistSameVehicleType(pDlg->GetName()))
						{
							pVehicle->SetVehicleType(pDlg->GetName());
							CString strVehicleType;
							strVehicleType.Format(_T("Vehicle Type: %s"),pDlg->GetName());
							m_wndTreeCtrl.SetItemText(hItem,strVehicleType);
							SetModified();
						}
						else
						{
							MessageBox(_T("Exist the same vehicle type!!."),_T("Warning"),MB_OK|MB_ICONINFORMATION);
						}
					}
				}
				else if (pNodeData->m_emType == TreeNodeData::Position_Node)
				{
					CDlgSelectLandsideObject* pDlgSelectLandsideObject = (CDlgSelectLandsideObject*)pCNI->pEditWnd;
					VehicleNonPaxRelatedPosition* pPosition= (VehicleNonPaxRelatedPosition*)(pNodeData->m_dwData);
					if (pPosition && pPosition->GetParkingLt().GetIDString().Compare(pDlgSelectLandsideObject->getSelectObject().GetIDString())!= 0)
					{
						HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
						if (hParentItem)
						{
							TreeNodeData* pSuperNode = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
							if (pSuperNode && pSuperNode->m_emType == TreeNodeData::Vehicle_Node)
							{
								VehicleNonPaxRelatedData* pVehicle = (VehicleNonPaxRelatedData*)(pSuperNode->m_dwData);
								if (pVehicle && !pVehicle->ExistSamePosition(pDlgSelectLandsideObject->getSelectObject().GetIDString()))
								{
									LandsideFacilityLayoutObjectList* pLayoutList = &m_pInLandside->getObjectList();
									if (pLayoutList)
									{
										if (pLayoutList->getObjectByName(pDlgSelectLandsideObject->getSelectObject()) == NULL)
										{
											MessageBox(_T("Can't select group of position!!."),_T("Warning"),MB_OK|MB_ICONINFORMATION);
										}
										else
										{
											pPosition->SetParkingLot(pDlgSelectLandsideObject->getSelectObject());
											CString strObject;
											strObject.Format(_T("Position: %s"),pPosition->GetParkingLt().GetIDString());
											m_wndTreeCtrl.SetItemText(hItem,strObject);
											SetModified();
										}
									}
								}
								else
								{
									MessageBox(_T("Exist the same position!!."),_T("Warning"),MB_OK|MB_ICONINFORMATION);
								}
							}
						}
					}
				}
			}
			if (pCNI->pEditWnd)
			{
				delete pCNI->pEditWnd;
				pCNI->pEditWnd = NULL;
			}
		}
		break;
	case UM_CEW_COMBOBOX_BEGIN:
		{
			CRect rectWnd;
			HTREEITEM hItem = (HTREEITEM)wParam;
			CWnd* pWnd=m_wndTreeCtrl.GetEditWnd(hItem);
			m_wndTreeCtrl.GetItemRect(hItem,rectWnd,TRUE);
			pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
			CComboBox* pCB=(CComboBox*)pWnd;
			pCB->ResetContent();

			TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
			if (pNodeData->m_emType == TreeNodeData::Daily_Node)
			{
				pCB->AddString(strDailyBuffer[0]);
				pCB->AddString(strDailyBuffer[1]);
			}
			else
			{
				int nCount = _g_GetActiveProbMan(  m_pInLandside->getInputTerminal() )->getCount();
				for( int m=0; m<nCount; m++ )
				{
					CProbDistEntry* pPBEntry = _g_GetActiveProbMan( m_pInLandside->getInputTerminal() )->getItem( m );			
					pCB->AddString(pPBEntry->m_csName);
				}
			}
		}
		break;
	case UM_CEW_COMBOBOX_SELCHANGE:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			CString strValue = *((CString*)lParam);
			TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
			if (pNodeData->m_emType == TreeNodeData::Arrival_Node)
			{
				VehicleNonPaxRelatedPosition* pPosition= (VehicleNonPaxRelatedPosition*)(pNodeData->m_dwData);
				pPosition->SetDistArr(ChangeProbDist(hItem,strValue));
				CString strArrDist;
				strArrDist.Format(_T("Arrival time distribution(mins): %s"),pPosition->GetDistArr()->getPrintDist());
				m_wndTreeCtrl.SetItemText(hItem,strArrDist);
				SetModified();
			}
			else if (pNodeData->m_emType == TreeNodeData::Stop_Node)
			{
				VehicleNonPaxRelatedPosition* pPosition= (VehicleNonPaxRelatedPosition*)(pNodeData->m_dwData);
				pPosition->SetDistStop(ChangeProbDist(hItem,strValue));
				CString strStop;
				strStop.Format(_T("Stop period: %s"),pPosition->GetDistStop()->getPrintDist());
				m_wndTreeCtrl.SetItemText(hItem,strStop);
				SetModified();
			}
			else if (pNodeData->m_emType == TreeNodeData::Offset_Node)
			{
				VehicleNonPaxRelatedEntry* pEntry = (VehicleNonPaxRelatedEntry*)(pNodeData->m_dwData);
				pEntry->SetDistOffset(ChangeProbDist(hItem,strValue));
				CString strOffset;
				strOffset.Format(_T("Offset(mins): %s"),pEntry->GetDistOffset()->getPrintDist());
				m_wndTreeCtrl.SetItemText(hItem,strOffset);
				SetModified();
			}
			else if (pNodeData->m_emType == TreeNodeData::Daily_Node)
			{
				CWnd* pWnd=m_wndTreeCtrl.GetEditWnd(hItem);
				CComboBox* pCB=(CComboBox*)pWnd;
				int nSel = pCB->GetCurSel();
				VehicleNonPaxRelatedPosition* pPosition= (VehicleNonPaxRelatedPosition*)(pNodeData->m_dwData);
				if (nSel != LB_ERR)
				{
					pPosition->SetDaily(nSel);
					CString strDaily;
					strDaily.Format(_T("Daily: %s"),strDailyBuffer[nSel]);
					m_wndTreeCtrl.SetItemText(hItem,strDaily);
					SetModified();
				}
			}
		}
		break;
	case UM_CEW_DATETIME_END:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			long lSecondTime = (long)lParam;
			CString strText(_T(""));

			ElapsedTime estTime(lSecondTime);

			HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
			TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
			if (pNodeData && pNodeData->m_emType == TreeNodeData::Position_Node)
			{
				VehicleNonPaxRelatedPosition* pPosition= (VehicleNonPaxRelatedPosition*)(pNodeData->m_dwData);
				if (pPosition)
				{
					pPosition->SetStartTime(estTime);
					strText.Format(_T("Start Time:	%02d:%02d:%02d"), estTime.GetHour(), estTime.GetMinute(), estTime.GetSecond());
					m_wndTreeCtrl.SetItemText(hItem, strText);
					m_wndTreeCtrl.SetItemData(hItem,(DWORD)estTime.asSeconds());
					SetModified();
				}
			}
		}
		break;
	default:
		break;
	}
	return CXTResizeDialog::DefWindowProc(message, wParam, lParam);
}

CProbDistEntry* CDlgVehicleAssignmentNonPaxRelated::ChangeProbDist( HTREEITEM hItem,const CString& strDist)
{
	int nIndexSeled=m_wndTreeCtrl.GetCmbBoxCurSel(hItem);
	CString strValue = _T("New Probability Distribution...");

	CProbDistEntry* pPDEntry = NULL;
	if( strcmp( strValue, strDist ) == 0 )
	{
		CDlgProbDist dlg(  m_pInLandside->getInputTerminal()->m_pAirportDB, true );
		if(dlg.DoModal()==IDOK) 
		{
			int idxPD = dlg.GetSelectedPDIdx();
			ASSERT(idxPD!=-1);
			pPDEntry = _g_GetActiveProbMan( m_pInLandside->getInputTerminal() )->getItem(idxPD);
		}
	}
	else
	{
		int nCount = _g_GetActiveProbMan( m_pInLandside->getInputTerminal() )->getCount();
		for( int i=0; i<nCount; i++ )
		{
			pPDEntry = _g_GetActiveProbMan( m_pInLandside->getInputTerminal() )->getItem( i );
			if( strcmp( pPDEntry->m_csName, strDist ) == 0 )
				break;
		}
	}

	return pPDEntry;
}

void CDlgVehicleAssignmentNonPaxRelated::GetProbDropdownList( CStringList& strList )
{
	CString s;
	s.LoadString(IDS_STRING_NEWDIST);
	strList.InsertAfter( strList.GetTailPosition(), s );

	int nCount = _g_GetActiveProbMan(  m_pInLandside->getInputTerminal() )->getCount();
	for( int m=0; m<nCount; m++ )
	{
		CProbDistEntry* pPBEntry = _g_GetActiveProbMan( m_pInLandside->getInputTerminal() )->getItem( m );			
		strList.InsertAfter( strList.GetTailPosition(), pPBEntry->m_csName );
	}
}

void CDlgVehicleAssignmentNonPaxRelated::InsertVehicleItem( HTREEITEM hItem,VehicleNonPaxRelatedData* pVehicle )
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	TreeNodeData* pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::Vehicle_Node;
	pNodeData->m_dwData = (DWORD)pVehicle;
	cni.nt = NT_NORMAL;
	cni.net = NET_SHOW_DIALOGBOX;
	CString strVehicleType;
	strVehicleType.Format(_T("Vehicle Type: %s"),pVehicle->GetVehicleType());
	HTREEITEM hVehicleItem = m_wndTreeCtrl.InsertItem(strVehicleType,cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hVehicleItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);
	for (int i = 0; i < pVehicle->GetCount(); i++)
	{
		VehicleNonPaxRelatedPosition* pPosition = pVehicle->GetItem(i);
		InsertPositionItem(hVehicleItem,pPosition);
	}

	m_wndTreeCtrl.Expand(hVehicleItem,TVE_EXPAND);
}

void CDlgVehicleAssignmentNonPaxRelated::InsertPositionItem( HTREEITEM hItem,VehicleNonPaxRelatedPosition* pPosition )
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	TreeNodeData* pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::Position_Node;
	pNodeData->m_dwData = (DWORD)pPosition;
	cni.nt = NT_NORMAL;
	cni.net = NET_SHOW_DIALOGBOX;
	CString strParkingLot;
	strParkingLot.Format(_T("Position: %s"),pPosition->GetParkingLt().GetIDString());
	HTREEITEM hParkingLotItem = m_wndTreeCtrl.InsertItem(strParkingLot,cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hParkingLotItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);

	pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::Daily_Node;
	pNodeData->m_dwData = (DWORD)pPosition;
	cni.nt = NT_NORMAL;
	cni.net = NET_COMBOBOX;
	CString strDaily;
	strDaily.Format(_T("Daily: %s"),strDailyBuffer[pPosition->GetDaily()]);
	HTREEITEM hDailyItem = m_wndTreeCtrl.InsertItem(strDaily,cni,FALSE,FALSE,hParkingLotItem);
	m_wndTreeCtrl.SetItemData(hDailyItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);
	//Start Time
// 	pNodeData = new TreeNodeData();
// 	pNodeData->m_emType = TreeNodeData::Start_Node;
// 	pNodeData->m_dwData = (DWORD)pPosition;
	cni.nt = NT_NORMAL;
	cni.net = NET_DATETIMEPICKER;
	CString strStart;
	strStart.Format(_T("Start Time: %02d:%02d:%02d"),pPosition->GetStartTime().GetHour(),pPosition->GetStartTime().GetMinute(),pPosition->GetStartTime().GetSecond());
	HTREEITEM hStartItem = m_wndTreeCtrl.InsertItem(strStart,cni,FALSE,FALSE,hParkingLotItem);
	m_wndTreeCtrl.SetItemData(hStartItem,(DWORD)pPosition->GetStartTime().asSeconds());
//	m_vNodeData.push_back(pNodeData);

	//Arrival time
	pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::Arrival_Node;
	pNodeData->m_dwData = (DWORD)pPosition;
	cni.nt = NT_NORMAL;
	cni.net = NET_COMBOBOX;
	CString strArrDist;
	strArrDist.Format(_T("Arrival time distribution(mins): %s"),pPosition->GetDistArr()->getPrintDist());
	HTREEITEM hArrItem = m_wndTreeCtrl.InsertItem(strArrDist,cni,FALSE,FALSE,hParkingLotItem);
	m_wndTreeCtrl.SetItemData(hArrItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);

	//stop perior
	pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::Stop_Node;
	pNodeData->m_dwData = (DWORD)pPosition;
	cni.nt = NT_NORMAL;
	cni.net = NET_COMBOBOX;
	CString strStop;
	strStop.Format(_T("Stop period: %s"),pPosition->GetDistStop()->getPrintDist());
	HTREEITEM hStopItem = m_wndTreeCtrl.InsertItem(strStop,cni,FALSE,FALSE,hParkingLotItem);
	m_wndTreeCtrl.SetItemData(hStopItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);

	//entry root
	pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::EntryRoot_Node;
	pNodeData->m_dwData = (DWORD)pPosition;
	cni.nt = NT_NORMAL;
	cni.net = NET_NORMAL;
	HTREEITEM hEntryItem = m_wndTreeCtrl.InsertItem("Entry",cni,FALSE,FALSE,hParkingLotItem);
	m_wndTreeCtrl.SetItemData(hEntryItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);
	for (int i = 0; i < pPosition->GetCount(); i++)
	{
		VehicleNonPaxRelatedEntry* pEntry = pPosition->GetItem(i);
		InsertEntryItem(hEntryItem,pEntry);
	}
	m_wndTreeCtrl.Expand(hEntryItem,TVE_EXPAND);
	m_wndTreeCtrl.Expand(hParkingLotItem,TVE_EXPAND);
}

void CDlgVehicleAssignmentNonPaxRelated::InsertEntryItem( HTREEITEM hItem, VehicleNonPaxRelatedEntry* pEntry )
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	TreeNodeData* pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::Entry_Node;
	pNodeData->m_dwData = (DWORD)pEntry;
	cni.nt = NT_NORMAL;
	cni.net = NET_SHOW_DIALOGBOX;
	HTREEITEM hEntryItem = m_wndTreeCtrl.InsertItem(pEntry->GetEntry().GetIDString(),cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hEntryItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);

	//offset 
	pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::Offset_Node;
	pNodeData->m_dwData = (DWORD)pEntry;
	cni.nt = NT_NORMAL;
	cni.net = NET_COMBOBOX;
	CString strOffset;
	strOffset.Format(_T("Offset(mins): %s"),pEntry->GetDistOffset()->getPrintDist());
	HTREEITEM hOffsetItem = m_wndTreeCtrl.InsertItem(strOffset,cni,FALSE,FALSE,hEntryItem);
	m_wndTreeCtrl.SetItemData(hOffsetItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);

	m_wndTreeCtrl.Expand(hEntryItem,TVE_EXPAND);
}

void CDlgVehicleAssignmentNonPaxRelated::DeleteTreeNodeData( TreeNodeData* pNodeData )
{
	std::vector<TreeNodeData*>::iterator iter = std::find(m_vNodeData.begin(),m_vNodeData.end(),pNodeData);
	if (iter != m_vNodeData.end())
	{
		m_vNodeData.erase(iter);
		delete pNodeData;
	}
}

void CDlgVehicleAssignmentNonPaxRelated::OnCmdDeleteItem()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem == NULL)
		return;

	TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	if (pNodeData == NULL)
		return;

	VehicleNonPaxRelatedData* pVehicle = (VehicleNonPaxRelatedData*)(pNodeData->m_dwData);
	m_pAssignment->DeleteItem(pVehicle);
	DeleteTreeNodeData(pNodeData);

	m_wndTreeCtrl.DeleteItem(hItem);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgVehicleAssignmentNonPaxRelated::OnAddObject()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem == NULL)
		return;

	TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	if (pNodeData == NULL)
		return;

	switch (pNodeData->m_emType)
	{
	case TreeNodeData::Vehicle_Node:
		{
			VehicleNonPaxRelatedData* pVehicle = (VehicleNonPaxRelatedData*)(pNodeData->m_dwData);
			LandsideFacilityLayoutObjectList* pLayoutList = &m_pInLandside->getObjectList();
			if (pLayoutList)
			{
				CDlgSelectLandsideObject dlg(pLayoutList,this);
				dlg.AddObjType(ALT_LPARKINGLOT);
				if(dlg.DoModal() == IDOK)
				{
					if (pLayoutList->getObjectByName(dlg.getSelectObject()))
					{
						CString strObject = dlg.getSelectObject().GetIDString();
						if (!pVehicle->ExistSamePosition(strObject))
						{
							VehicleNonPaxRelatedPosition* pPosition = new VehicleNonPaxRelatedPosition();
							ALTObjectID objID;
							objID.FromString(strObject);
							pPosition->SetParkingLot(objID);
							pVehicle->AddItem(pPosition);

							InsertPositionItem(hItem,pPosition);
							m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
							SetModified();
						}
						else
						{
							MessageBox(_T("Exist the same position!!."),_T("Warning"),MB_OK | MB_ICONINFORMATION);
						}
					}
					else
					{
						MessageBox(_T("Can't select group of poisiton!!."),_T("Warning"),MB_OK | MB_ICONINFORMATION);
					}
				}	
			}


		}
		break;
	case TreeNodeData::EntryRoot_Node:
		{
			VehicleNonPaxRelatedPosition* pPosition = (VehicleNonPaxRelatedPosition*)(pNodeData->m_dwData);
			LandsideFacilityLayoutObjectList* pLayoutList = &m_pInLandside->getObjectList();
			if (pLayoutList)
			{
				CDlgSelectLandsideObject dlg(pLayoutList,this);
				dlg.AddObjType(ALT_LEXT_NODE);
				if(dlg.DoModal() == IDOK)
				{
					if (pLayoutList->getObjectByName(dlg.getSelectObject()))
					{
						CString strObject = dlg.getSelectObject().GetIDString();
						if (!pPosition->ExistSameEntry(strObject))
						{
							VehicleNonPaxRelatedEntry* pEntry = new VehicleNonPaxRelatedEntry();
							ALTObjectID objID;
							objID.FromString(strObject);
							pEntry->SetEntry(objID);
							pPosition->AddItem(pEntry);

							InsertEntryItem(hItem,pEntry);
							m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
							SetModified();
						}
						else
						{
							MessageBox(_T("Exist the same Entry or Exit node!!."),_T("Warning"),MB_OK | MB_ICONINFORMATION);
						}
					}
					else
					{
						MessageBox(_T("Can't select group of Entry or Exit !!."),_T("Warning"),MB_OK | MB_ICONINFORMATION);
					}
				}	
			}

		}
		break;
	default:
		break;
	}
}

void CDlgVehicleAssignmentNonPaxRelated::OnDeleteObject()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem == NULL)
		return;

	HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
	if (hParentItem == NULL)
		return;

	TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	if (pNodeData == NULL)
		return;

	TreeNodeData* pSuperData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
	if (pSuperData == NULL)
		return;

	switch (pNodeData->m_emType)
	{
	case TreeNodeData::Position_Node:
		{
			VehicleNonPaxRelatedData* pVehicle = (VehicleNonPaxRelatedData*)(pSuperData->m_dwData);
			VehicleNonPaxRelatedPosition* pPosition = (VehicleNonPaxRelatedPosition*)(pNodeData->m_dwData);
			pVehicle->DeleteItem(pPosition);

			m_wndTreeCtrl.DeleteItem(hItem);
			DeleteTreeNodeData(pNodeData);
			SetModified();
		}
		break;
	case TreeNodeData::Entry_Node:
		{
			VehicleNonPaxRelatedPosition* pPosition = (VehicleNonPaxRelatedPosition*)(pSuperData->m_dwData);
			VehicleNonPaxRelatedEntry* pEntry = (VehicleNonPaxRelatedEntry*)(pNodeData->m_dwData);
			pPosition->DeleteItem(pEntry);

			m_wndTreeCtrl.DeleteItem(hItem);
			DeleteTreeNodeData(pNodeData);
			SetModified();
		}
		break;
	default:
		break;
	}
}

void CDlgVehicleAssignmentNonPaxRelated::OnLvnItemchangedList( NMHDR *pNMHDR, LRESULT *pResult )
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;

	if (pNMTreeView->itemNew.hItem)
	{
		if (pNMTreeView->itemNew.hItem != pNMTreeView->itemOld.hItem)
		{
			COOLTREE_NODE_INFO* pCNI = (COOLTREE_NODE_INFO*)m_wndTreeCtrl.GetItemNodeInfo(pNMTreeView->itemNew.hItem);
			if (pCNI->net != NET_DATETIMEPICKER)
			{
				TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(pNMTreeView->itemNew.hItem);
				if (pNodeData && pNodeData->m_emType == TreeNodeData::Vehicle_Node) 
				{
					m_wndToolBar.GetToolBarCtrl().EnableButton(ID_VEHICLE_NEW,TRUE);
					m_wndToolBar.GetToolBarCtrl().EnableButton(ID_VEHICLE_DEL,TRUE);
					return;
				}
			}
		}
	}

	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_VEHICLE_NEW,TRUE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_VEHICLE_DEL,FALSE);
}

void CDlgVehicleAssignmentNonPaxRelated::OnContextMenu( CWnd* pWnd, CPoint point )
{
	CRect rectTree;
	m_wndTreeCtrl.GetWindowRect(&rectTree);
	if (!rectTree.PtInRect(point)) 
		return;

	m_wndTreeCtrl.SetFocus();
	CPoint pt = point;
	m_wndTreeCtrl.ScreenToClient(&pt);
	UINT iRet;
	HTREEITEM hRClickItem = m_wndTreeCtrl.HitTest(pt, &iRet);
	if (iRet != TVHT_ONITEMLABEL)
	{
		hRClickItem = NULL;
		return;
	}

	m_wndTreeCtrl.SelectItem(hRClickItem);
	TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hRClickItem);
	if (pNodeData == NULL)
		return;

	CMenu menuPopup; 
	menuPopup.CreatePopupMenu();
	switch (pNodeData->m_emType)
	{
	case TreeNodeData::Vehicle_Node:
		{
			menuPopup.AppendMenu(MF_POPUP,ID_MENU_ADD,_T("Select Position"));
			menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		}
		break;
	case TreeNodeData::Position_Node:
		{
			menuPopup.AppendMenu(MF_POPUP,ID_MENU_DEL,_T("Delete Position"));
			menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		}
		break;
	case TreeNodeData::EntryRoot_Node:
		{
			menuPopup.AppendMenu(MF_POPUP,ID_MENU_ADD,_T("Add Entry"));
			menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		}
	case TreeNodeData::Entry_Node:
		{
			menuPopup.AppendMenu(MF_POPUP,ID_MENU_DEL,_T("Delete Entry"));
			menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		}
		break;
	default:
		break;
	}
	menuPopup.DestroyMenu();
}

























