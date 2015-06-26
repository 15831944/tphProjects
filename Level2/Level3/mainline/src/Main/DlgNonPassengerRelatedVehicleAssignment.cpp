// VehicleAssignmentNonPaxRelated.cpp : implementation file

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgNonPassengerRelatedVehicleAssignment.h"
#include "Landside/NonPaxVehicleAssignment.h"
#include "DlgSelectLandsideVehicleType.h"
#include "DlgProbDist.h"
#include "landside/InputLandside.h"
#include "Inputs/IN_TERM.H"
#include "Inputs/PROBAB.H"
#include "common/ProbDistManager.h"
#include "DlgSelectLandsideObject.h"
#include "DlgTimeRange.h"


//CDlgNonPassengerRelatedVehicleAssignment Dialog
IMPLEMENT_DYNAMIC(CDlgNonPassengerRelatedVehicleAssignment, CXTResizeDialog)
CDlgNonPassengerRelatedVehicleAssignment::CDlgNonPassengerRelatedVehicleAssignment(InputLandside* pInLandside, CWnd* pParent /*=NULL*/)
: CXTResizeDialog(CDlgNonPassengerRelatedVehicleAssignment::IDD, pParent)
,m_pInLandside(pInLandside)
{
	m_pNonPaxVehicleAssignment = new NonPaxVehicleAssignment();
	m_pNonPaxVehicleAssignment->ReadData(-1);
}

CDlgNonPassengerRelatedVehicleAssignment::~CDlgNonPassengerRelatedVehicleAssignment()
{
	if (m_pNonPaxVehicleAssignment != NULL)
	{
		delete m_pNonPaxVehicleAssignment;
		m_pNonPaxVehicleAssignment = NULL;
	}
	DeleteTreeNodeData();
}

void CDlgNonPassengerRelatedVehicleAssignment::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_TREE_DATA,m_wndTreeCtrl);
}


BEGIN_MESSAGE_MAP(CDlgNonPassengerRelatedVehicleAssignment, CXTResizeDialog)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_SAVE,OnBtnSave)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY(TVN_SELCHANGED,IDC_TREE_DATA,OnLvnItemchangedList)
	ON_COMMAND(ID_ACTYPE_ALIAS_ADD,OnAddObject)
	ON_COMMAND(ID_ACTYPE_ALIAS_DEL,OnDeleteObject)
	ON_COMMAND(ID_ACTYPE_ALIAS_EDIT,OnEditObject)
END_MESSAGE_MAP()


// CVehicleAssignmentNonPaxRelated message handlers
int CDlgNonPassengerRelatedVehicleAssignment::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP )
		|| !m_wndToolBar.LoadToolBar(IDR_ACTYPE_ALIASNAME))
	{
		return -1;
	}

	return 0;
}

BOOL CDlgNonPassengerRelatedVehicleAssignment::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();
	
	OnInitToolbar();
	UpdateToolBarState();
	LoadTreeContent();

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
	Invalidate(TRUE);
	return TRUE;  
}
void CDlgNonPassengerRelatedVehicleAssignment::OnInitToolbar()
{
	CRect rect;
	m_wndTreeCtrl.GetWindowRect( &rect );
	ScreenToClient( &rect );
	rect.top -= 26;
	rect.bottom = rect.top + 22;
	rect.left += 4;
	m_wndToolBar.MoveWindow(&rect);

	CToolBarCtrl& spotBarCtrl = m_wndToolBar.GetToolBarCtrl();
}
void CDlgNonPassengerRelatedVehicleAssignment::UpdateToolBarState()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem == NULL)
		return;
	TreeNodeData* pSelNode = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	if (pSelNode == NULL)
		return;

	TreeNodeData::TreeNodeType NodeType = pSelNode->m_emType;
	if (NodeType == TreeNodeData::Node_Root || NodeType == TreeNodeData::Node_Destination \
		|| NodeType == TreeNodeData::Node_TimeRange || TreeNodeData::Node_ETA || TreeNodeData::Node_LotArrivalTime)
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_ADD,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_DEL,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_EDIT,FALSE);
	}
	if (NodeType == TreeNodeData::Node_Strategy || NodeType == TreeNodeData::Node_TimeRange \
		|| NodeType == TreeNodeData::Node_ProDist )
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_ADD,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_DEL,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_EDIT,TRUE);
	}
	if (NodeType == TreeNodeData::Node_VehicleName || NodeType == TreeNodeData::Node_VehicleNumber \
		|| NodeType == TreeNodeData::Node_EntryName || NodeType == TreeNodeData::Node_ProDist )
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_ADD,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_DEL,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_EDIT,TRUE);
	}
	if (NodeType == TreeNodeData::Node_Lot || NodeType == TreeNodeData::Node_TimeRange)
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_ADD,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_DEL,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_EDIT,TRUE);
	}
	if (NodeType == TreeNodeData::Node_Vehicle || NodeType == TreeNodeData::Node_Entry)
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_ADD,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_DEL,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_EDIT,FALSE);
	}
	if (NodeType == TreeNodeData::Node_Daily)
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_ADD,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_DEL,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_EDIT,FALSE);
	}
	if (NodeType == TreeNodeData::Node_Destination)
	{
		HTREEITEM childItem = m_wndTreeCtrl.GetChildItem(hItem);
		if (childItem)
		{
			m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_ADD,FALSE);
			m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_DEL,FALSE);
			m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_EDIT,FALSE);
		}
		
	}
}

void CDlgNonPassengerRelatedVehicleAssignment::LoadTreeContent()
{
	m_wndTreeCtrl.DeleteAllItems();

	//strategy, root item
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	TreeNodeData* pRootNode = new TreeNodeData();
	pRootNode->m_emType = TreeNodeData::Node_Root;

	HTREEITEM hRootItem = m_wndTreeCtrl.InsertItem("Strategy",cni,FALSE,FALSE,TVI_ROOT);
	m_wndTreeCtrl.SetItemData(hRootItem,(DWORD)pRootNode);
	m_vNodeData.push_back(pRootNode);	

	for (int i = 0; i < m_pNonPaxVehicleAssignment->GetItemCount(); i++)
	{
		NonPaxVehicleStrategy* pNonPaxVehicleStrategy = m_pNonPaxVehicleAssignment->GetItem(i);
		LoadSetStrategy(hRootItem,pNonPaxVehicleStrategy);
	}	
	m_wndTreeCtrl.SelectItem(hRootItem);
	m_wndTreeCtrl.Expand(hRootItem,TVE_EXPAND);
}

void CDlgNonPassengerRelatedVehicleAssignment::OnBtnSave()
{
	try
	{
		CADODatabase::BeginTransaction();
		m_pNonPaxVehicleAssignment->SaveData(-1);
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
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
}

void CDlgNonPassengerRelatedVehicleAssignment::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	try
	{
		CADODatabase::BeginTransaction();
		m_pNonPaxVehicleAssignment->SaveData(-1);
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

void CDlgNonPassengerRelatedVehicleAssignment::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CXTResizeDialog::OnCancel();
}


void CDlgNonPassengerRelatedVehicleAssignment::SetModified( BOOL bModified /*= TRUE*/ )
{
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(bModified);
}

LRESULT CDlgNonPassengerRelatedVehicleAssignment::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message)
	{
	case UM_CEW_SHOW_DIALOGBOX_BEGIN:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
			
			if (pNodeData->m_emType == TreeNodeData::Node_TimeRange)
			{
				VehicleTimeEntry* pVehicleTime = (VehicleTimeEntry*)pNodeData->m_dwData;

				HTREEITEM hVehicleTime = m_wndTreeCtrl.GetParentItem(hItem);
				
				
				HTREEITEM hStratigy = m_wndTreeCtrl.GetParentItem(hVehicleTime);
				

				TreeNodeData* pStratigyNode = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hStratigy);
				
				NonPaxVehicleStrategy* pNonPaxVehicleStrategy = (NonPaxVehicleStrategy*)pStratigyNode->m_dwData;
				if (pNonPaxVehicleStrategy->IsDaily())
				{
					CDlgTimeRange dlg(pVehicleTime->GetTimeRange().GetStartTime(), pVehicleTime->GetTimeRange().GetEndTime(),TRUE, this);
					if(dlg.DoModal() == IDOK)
					{
						TimeRange Time(dlg.GetStartTime(), dlg.GetEndTime());						
						pVehicleTime->SetTimeRange(Time);
					}
					CString strTimeRange;					
					strTimeRange.Format(_T("Start - End Time: %s - %s"),pVehicleTime->GetTimeRange().GetStartTime().printTime(),pVehicleTime->GetTimeRange().GetEndTime().printTime());
					m_wndTreeCtrl.SetItemText(hItem,strTimeRange);
				} 
				else
				{
					CDlgTimeRange dlg(pVehicleTime->GetTimeRange().GetStartTime(), pVehicleTime->GetTimeRange().GetEndTime(),FALSE, this);
					if(dlg.DoModal() == IDOK)
					{
						TimeRange Time(dlg.GetStartTime(), dlg.GetEndTime());						
						pVehicleTime->SetTimeRange(Time);
					}
		
					CString strTimeRange;					
					strTimeRange.Format(_T("Start - End Time: %s - %s"),pVehicleTime->GetTimeRange().GetStartTime().PrintDateTime(),pVehicleTime->GetTimeRange().GetEndTime().PrintDateTime());
					m_wndTreeCtrl.SetItemText(hItem,strTimeRange);

				}
				GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
			}
			if (pNodeData->m_emType == TreeNodeData::Node_VehicleName)
			{
				HTREEITEM hVehicle = m_wndTreeCtrl.GetParentItem(hItem);
				
				TreeNodeData* pVehicleNode = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hVehicle);
				
				VehicleTypeEntry* pVehicleType = (VehicleTypeEntry*)pVehicleNode->m_dwData;

				HTREEITEM hVehicleTime = m_wndTreeCtrl.GetParentItem(hVehicle);
				
				TreeNodeData* pVehicleTimeNode = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hVehicleTime);
				
				VehicleTimeEntry* pVehicleTime = (VehicleTimeEntry*)pVehicleTimeNode->m_dwData;
				CDlgSelectLandsideVehicleType dlg;
				CString strGroupName;
				if(dlg.DoModal() == IDOK)
				{
					 strGroupName = dlg.GetName();					
				}
				if (!strGroupName.IsEmpty() && pVehicleType->GetVehicleType().Compare(strGroupName) != NULL)
				{
					if (pVehicleTime->FindVehicleType(strGroupName))
					{
						MessageBox(_T("The vehicle Item is exit!"),NULL,MB_OK);
					}
					else
					{
						pVehicleType->SetVehicleType(strGroupName);
					}
				}

				CString strVehicleName;
				strVehicleName.Format(_T("Vehicle Name: %s"),pVehicleType->GetVehicleType());
				m_wndTreeCtrl.SetItemText(hItem,strVehicleName);
				GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
			}
			if (pNodeData->m_emType == TreeNodeData::Node_EntryName)
			{
				HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
				
				TreeNodeData* pEntryNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
				

				CEntryOffSet* pEntry = (CEntryOffSet*)pEntryNodeData->m_dwData;
				
				
				HTREEITEM hNonPax = m_wndTreeCtrl.GetParentItem(m_wndTreeCtrl.GetParentItem(hParentItem));
				
				TreeNodeData* pNonPaxNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hNonPax);
				

				NonPaxVehicleStrategy* pNonPax = (NonPaxVehicleStrategy*)pNonPaxNodeData->m_dwData;
				
				ALTObjectID selectLot;
				CDlgSelectLandsideObject dlg(&m_pInLandside->getObjectList(),this);
				dlg.AddObjType(ALT_LEXT_NODE);
				if( dlg.DoModal() == IDOK)
				{
					selectLot = dlg.getSelectObject();
				}
				
				if (selectLot.GetIDString().IsEmpty())
					return NULL;				
				if (!selectLot.GetIDString().IsEmpty() && pEntry->GetEntry().GetIDString().Compare(selectLot.GetIDString()) != NULL)
				{
					if (pNonPax->IsEntryExisted(selectLot))
					{
						MessageBox(_T("The item is exit!"),NULL,MB_OK);
					}
					else
					{
						pEntry->SetEntry(selectLot);
					}
				}

				CString strEntryName;
				strEntryName.Format(_T("Entry Name: %s"),pEntry->GetEntry().GetIDString());
				m_wndTreeCtrl.SetItemText(hItem,strEntryName);

				GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
			}
		}
		break;
	case UM_CEW_SHOW_DIALOGBOX_END:
		{
			UpdateToolBarState();
		}
		break;
	case UM_CEW_LABLE_END:
		{
			HTREEITEM hItem = (HTREEITEM)wParam;
			CString strValue = *((CString*)lParam);
			TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
			NonPaxVehicleStrategy* pNonPaxVehicleStrategy = (NonPaxVehicleStrategy*)pNodeData->m_dwData;								
			if (pNodeData->m_emType == TreeNodeData::Node_Strategy)
			{
				if (strValue.IsEmpty() && pNonPaxVehicleStrategy->GetStrategyName().IsEmpty())
				{
					MessageBox(_T("Strategy name can not be empty"),_T("Warning"),MB_OK);
					m_wndTreeCtrl.DoEdit(hItem);										
				}
				if (!strValue.IsEmpty() && pNonPaxVehicleStrategy->GetStrategyName().CompareNoCase(strValue) != NULL)
				{
					if (m_pNonPaxVehicleAssignment->IsTheStrategyExit(strValue))
					{
						MessageBox(_T("The vehicle Item is exit!"),NULL,MB_OK);
					}
					else
					{
						pNonPaxVehicleStrategy->SetStrategyName(strValue);
					}
				}
				CString strStrategyName;
				CString strName = pNonPaxVehicleStrategy->GetStrategyName();
				strName.MakeUpper();
				strStrategyName.Format(_T("Name: %s"),strName);
				m_wndTreeCtrl.SetItemText(hItem,strStrategyName);

			}
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
		break;
	case UM_CEW_EDITSPIN_BEGIN:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			COOLTREE_NODE_INFO* pInfo = m_wndTreeCtrl.GetItemNodeInfo(hItem);
			pInfo->fMinValue = 0;
		}
		break;
	case UM_CEW_EDITSPIN_END:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			CString strValue=*((CString*)lParam);
			TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
			
			COOLTREE_NODE_INFO* pInfo = m_wndTreeCtrl.GetItemNodeInfo( hItem );
			if (pNodeData->m_emType == TreeNodeData::Node_VehicleNumber)
			{

				int nValue=static_cast<int>(atoi(strValue));
				HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
				
				TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
				
				VehicleTypeEntry* pVehicleTypeEntry = (VehicleTypeEntry*)pNodeData->m_dwData;
				
				pVehicleTypeEntry->SetVehicleNumber(nValue);
				CString strVehicleNumber;
				strVehicleNumber.Format(_T("Vehicle Number: %d"),pVehicleTypeEntry->GetVehicleNumber());
				m_wndTreeCtrl.SetItemText(hItem,strVehicleNumber);

				
				pInfo->fMinValue = static_cast<float>(pVehicleTypeEntry->GetVehicleNumber());
				GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
			}
	
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
			TreeNodeData* NodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
			
			if (NodeData->m_emType == TreeNodeData::Node_ProDist ||
				NodeData->m_emType == TreeNodeData::Node_Lot)
			{
				if(pCB->GetCount()!=0)
				{
					pCB->ResetContent();
				}

				CString s;
				s.LoadString(IDS_STRING_NEWDIST);
				pCB->AddString(s);

				int nCount = _g_GetActiveProbMan(  m_pInLandside->getInputTerminal() )->getCount();
				for( int m=0; m<nCount; m++ )
				{
					CProbDistEntry* pPBEntry = _g_GetActiveProbMan( m_pInLandside->getInputTerminal() )->getItem( m );		
					pCB->AddString(pPBEntry->m_csName);
				}
			}
			if(NodeData->m_emType == TreeNodeData::Node_Daily)
			{
				if(pCB->GetCount()!=0)
				{
					pCB->ResetContent();
				}
				pCB->AddString(_T("Yes"));
				pCB->AddString(_T("No"));
			}
			
		}
		break;
	case UM_CEW_COMBOBOX_SELCHANGE:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;

			int iSelCombox = m_wndTreeCtrl.GetCmbBoxCurSel(hItem);
			ASSERT(iSelCombox >= 0);
			TreeNodeData* NodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
			
			
			if(NodeData->m_emType == TreeNodeData::Node_ProDist)
			{
				CProDistrubution* pProDist = (CProDistrubution*)NodeData->m_dwData;
				
				if(iSelCombox == 0)
				{
					CDlgProbDist dlg(  m_pInLandside->getInputTerminal()->m_pAirportDB, true );
					if(dlg.DoModal()==IDOK) 
					{
						int idxPD = dlg.GetSelectedPDIdx();
						ASSERT(idxPD!=-1);
						CProbDistEntry* pde = _g_GetActiveProbMan( m_pInLandside->getInputTerminal() )->getItem(idxPD);
						pProDist->SetProDistrubution(pde);

						TreeNodeData* pProDistNode = new TreeNodeData();
						pProDistNode->m_emType = TreeNodeData::Node_ProDist;
						pProDistNode->m_dwData = (DWORD)pProDist;
						CString strProDist;
						strProDist.Format(_T("Distrubution(mins): %s"),pProDist->getDistName());
						m_wndTreeCtrl.SetItemText(hItem,strProDist);
						m_wndTreeCtrl.SetItemData(hItem,(DWORD)pProDistNode);
						SetModified();
					}
				}
				else
				{
					ASSERT( iSelCombox-1 >= 0 && iSelCombox-1 < static_cast<int>(_g_GetActiveProbMan( m_pInLandside->getInputTerminal()  )->getCount()) );
					CProbDistEntry* pPBEntry = _g_GetActiveProbMan( m_pInLandside->getInputTerminal()  )->getItem( iSelCombox-1 );
					pProDist->SetProDistrubution(pPBEntry);
					TreeNodeData* pProDistNode = new TreeNodeData();
					pProDistNode->m_emType = TreeNodeData::Node_ProDist;
					pProDistNode->m_dwData = (DWORD)pProDist;
					CString strProDist;
					strProDist.Format(_T("Distrubution(mins): %s"),pProDist->getDistName());
					m_wndTreeCtrl.SetItemText(hItem,strProDist);
					m_wndTreeCtrl.SetItemData(hItem,(DWORD)pProDistNode);
					SetModified();
				}
				GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
			}
			if(NodeData->m_emType == TreeNodeData::Node_Lot)
			{
				if(iSelCombox == 0)
				{
					CDlgProbDist dlg(  m_pInLandside->getInputTerminal()->m_pAirportDB, true );
					if(dlg.DoModal()==IDOK) 
					{
						int idxPD = dlg.GetSelectedPDIdx();
						ASSERT(idxPD!=-1);
						CProbDistEntry* pde = _g_GetActiveProbMan( m_pInLandside->getInputTerminal() )->getItem(idxPD);
				
						ParkingLotRoute* pParkingLotRoute = (ParkingLotRoute*)NodeData->m_dwData;
						pParkingLotRoute->SetProDist(pde);

						CString strLot;
						strLot.Format(_T("%s,  TA Time(mins): %s"),pParkingLotRoute->GetParkingLot().GetIDString(),pParkingLotRoute->GetProDist().getPrintDist());

						m_wndTreeCtrl.SetItemText(hItem,strLot);
						SetModified();
					}
				}
				else
				{
					ASSERT( iSelCombox-1 >= 0 && iSelCombox-1 < static_cast<int>(_g_GetActiveProbMan( m_pInLandside->getInputTerminal()  )->getCount()) );
					CProbDistEntry* pPBEntry = _g_GetActiveProbMan( m_pInLandside->getInputTerminal()  )->getItem( iSelCombox-1 );

					ParkingLotRoute* pParkingLotRoute = (ParkingLotRoute*)NodeData->m_dwData;
					pParkingLotRoute->SetProDist(pPBEntry);

					CString strLot;
					strLot.Format(_T("%s,  TA Time(mins): %s"),pParkingLotRoute->GetParkingLot().GetIDString(), pParkingLotRoute->GetProDist().getPrintDist());
					
					m_wndTreeCtrl.SetItemText(hItem,strLot);

					SetModified();
				}
				GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
			}

			if(NodeData->m_emType == TreeNodeData::Node_Daily)
			{
				HTREEITEM hLotItem = m_wndTreeCtrl.GetParentItem(hItem);

				HTREEITEM hparentItem = m_wndTreeCtrl.GetParentItem(hLotItem);
				
				TreeNodeData* pParentNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hparentItem);
				
				NonPaxVehicleStrategy* pNonPaxVehicleStrategy = (NonPaxVehicleStrategy*)pParentNodeData->m_dwData;
								
				if (iSelCombox == 0)
				{
					pNonPaxVehicleStrategy->SetDaily(true);				
				}
				if (iSelCombox == 1)
				{
					pNonPaxVehicleStrategy->SetDaily(false);		
				}
				ChangeTheTime(pNonPaxVehicleStrategy);
				LoadVehicleTimeEntry(hLotItem,pNonPaxVehicleStrategy);
				GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
			}
		
		}
		break;
	case UM_CEW_COMBOBOX_END:
		{

		}
		break;
	default:
		break;
	}
	return CXTResizeDialog::DefWindowProc(message, wParam, lParam);
}

void CDlgNonPassengerRelatedVehicleAssignment::DeleteTreeNodeData()
{
	std::vector<TreeNodeData*>::iterator iter = m_vNodeData.begin();
	for(; iter != m_vNodeData.end(); iter++)
	{
		delete *iter;
	}
	m_vNodeData.clear();
}

void CDlgNonPassengerRelatedVehicleAssignment::OnAddObject()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem == NULL)
		return;

	TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	if (pNodeData == NULL)
		return;

	
	if (pNodeData->m_emType == TreeNodeData::Node_Root)
	{
		NonPaxVehicleStrategy* pNonPaxVehicleStrategy = new NonPaxVehicleStrategy();
		//strategy name
		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this,cni);
		cni.nt = NT_NORMAL;
		cni.net = NET_LABLE;

		TreeNodeData* pStrategyNode = new TreeNodeData();
		pStrategyNode->m_emType = TreeNodeData::Node_Strategy;
		pStrategyNode->m_dwData = (DWORD)pNonPaxVehicleStrategy;

		CString strStrategyName;
		strStrategyName.Format(_T("Name:"));
		HTREEITEM hStrategyNameItem = m_wndTreeCtrl.InsertItem(strStrategyName,cni,FALSE,FALSE,hItem);
		m_wndTreeCtrl.SetItemData(hStrategyNameItem,(DWORD)pStrategyNode);
		m_vNodeData.push_back(pStrategyNode);
		m_pNonPaxVehicleAssignment->AddNewItem(pNonPaxVehicleStrategy);

		//destination
		cni.net = NET_NORMAL;

		TreeNodeData* pDestinationNode = new TreeNodeData();
		pDestinationNode->m_emType = TreeNodeData::Node_Destination;

		CString strDestination;
		strDestination.Format(_T("Destination"));
		HTREEITEM hDestinationItem = m_wndTreeCtrl.InsertItem(strDestination,cni,FALSE,FALSE,hStrategyNameItem);
		m_wndTreeCtrl.SetItemData(hDestinationItem,(DWORD)pDestinationNode);
		m_vNodeData.push_back(pDestinationNode);

		//lot a arrival time range distribution daily(Y/N)
		cni.net = NET_NORMAL;

		TreeNodeData* pLotArrivalTimeNode = new TreeNodeData();
		pLotArrivalTimeNode->m_emType = TreeNodeData::Node_LotArrivalTime;
	
		CString strLotArrivalTime;
		strLotArrivalTime.Format(_T("Lot: arrival time range distribution:"));
		HTREEITEM hLotArrivalTime = m_wndTreeCtrl.InsertItem(strLotArrivalTime,cni,FALSE,FALSE,hStrategyNameItem);
		m_wndTreeCtrl.SetItemData(hLotArrivalTime,(DWORD)pLotArrivalTimeNode);
		m_vNodeData.push_back(pLotArrivalTimeNode);	

		//daily
		cni.net = NET_COMBOBOX;
		TreeNodeData* pLotDailyNode = new TreeNodeData();
		pLotDailyNode->m_emType = TreeNodeData::Node_Daily;
		CString strDaily;
		strDaily.Format(_T("Daily: No"));
		HTREEITEM hLotDaily = m_wndTreeCtrl.InsertItem(strDaily,cni,FALSE,FALSE,hLotArrivalTime);
		m_wndTreeCtrl.SetItemData(hLotDaily,(DWORD)pLotDailyNode);
		m_vNodeData.push_back(pLotDailyNode);	

		//ETA Offset LOT A
		TreeNodeData* pETANode = new TreeNodeData();
		pETANode->m_emType = TreeNodeData::Node_ETA;
		cni.net = NET_STATIC;
		CString strEta;
		strEta.Format(_T("ETA Offset Lot:"));
		HTREEITEM hETAItem = m_wndTreeCtrl.InsertItem(strEta,cni,FALSE,FALSE,hStrategyNameItem);
		m_wndTreeCtrl.SetItemData(hETAItem,(DWORD)pETANode);
		m_vNodeData.push_back(pETANode);
		m_wndTreeCtrl.Expand(hLotArrivalTime,TVE_EXPAND);
		m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
		m_wndTreeCtrl.Expand(hStrategyNameItem,TVE_EXPAND);

		m_wndTreeCtrl.SelectItem(hStrategyNameItem);
		m_wndTreeCtrl.DoEdit(hStrategyNameItem);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		
	}
	if (pNodeData->m_emType == TreeNodeData::Node_Destination)
	{
		HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
		if (hParentItem == NULL)
			return;

		TreeNodeData* pNonPaxNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
		if (pNodeData == NULL)
			return;

		NonPaxVehicleStrategy* pNonPax = (NonPaxVehicleStrategy*)pNonPaxNodeData->m_dwData;
		if (pNonPax == NULL)
			return;

		CDlgSelectLandsideObject dlg(&m_pInLandside->getObjectList(),this);
		dlg.AddObjType(ALT_LPARKINGLOT);
		dlg.AddObjType(ALT_LCURBSIDE);
		dlg.AddObjType(ALT_LEXT_NODE);
		if( dlg.DoModal() == IDCANCEL )
			return;

		ALTObjectID selectLot = dlg.getSelectObject();

		//check if the lot existed
		if(IsTheLotItemExit(hItem,selectLot))
		{
			MessageBox("The selected object has been existed.",NULL, MB_OK);
			return;
		}
		ParkingLotRoute* pParkingLotRoute = new ParkingLotRoute();
		pParkingLotRoute->SetParkingLot(selectLot);	
		pNonPax->AddLotRouteItem(pParkingLotRoute);

		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this,cni);
		cni.nt = NT_NORMAL;
		cni.net = NET_COMBOBOX;
		TreeNodeData* pLotNode = new TreeNodeData();
		pLotNode->m_emType = TreeNodeData::Node_Lot;
		pLotNode->m_dwData = (DWORD)pParkingLotRoute;
		m_vNodeData.push_back(pLotNode);
	
		CString strLot;
		strLot.Format(_T("%s,  TA Time(mins): %s"),selectLot.GetIDString(),pParkingLotRoute->GetProDist().getPrintDist());

		HTREEITEM hLotItem = m_wndTreeCtrl.InsertItem(strLot,cni,FALSE,FALSE,hItem);
		m_wndTreeCtrl.SetItemData(hLotItem,(DWORD_PTR)pLotNode);
		m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
		m_wndTreeCtrl.SelectItem(hLotItem);

		LoadArrivalAndETAOffset(hItem,pNonPax);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
	if (pNodeData->m_emType == TreeNodeData::Node_Lot)
	{
		ParkingLotRoute* pLoPax = (ParkingLotRoute*)pNodeData->m_dwData;
		if (pLoPax == NULL)
			return;

		CDlgSelectLandsideObject dlg(&m_pInLandside->getObjectList(),this);
		dlg.AddObjType(ALT_LPARKINGLOT);
		dlg.AddObjType(ALT_LCURBSIDE);
		dlg.AddObjType(ALT_LEXT_NODE);
		if( dlg.DoModal() == IDCANCEL )
			return;

		ALTObjectID selectLot = dlg.getSelectObject();

		//check if the Facility existed
		if(IsTheLotItemExit(hItem,selectLot))
		{
			MessageBox("The selected object has been existed.",NULL, MB_OK);
			return;
		}
		ParkingLotRoute* pParkingLotRoute = new ParkingLotRoute();
		pParkingLotRoute->SetParkingLot(selectLot);
		pLoPax->AddItem(pParkingLotRoute);

		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this,cni);
		cni.nt = NT_NORMAL;
		cni.net = NET_COMBOBOX;
		TreeNodeData* pLotNode = new TreeNodeData();
		pLotNode->m_emType = TreeNodeData::Node_Lot;
		pLotNode->m_dwData = (DWORD)pParkingLotRoute;
		m_vNodeData.push_back(pLotNode);

		CString strLot;
		strLot.Format(_T("%s,  TA Time(mins): %s"),selectLot.GetIDString(),pParkingLotRoute->GetProDist().getPrintDist());


		HTREEITEM hLotItem = m_wndTreeCtrl.InsertItem(strLot,cni,FALSE,FALSE,hItem);
		m_wndTreeCtrl.SetItemData(hLotItem,(DWORD_PTR)pLotNode);
		m_wndTreeCtrl.SelectItem(hLotItem);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
	if (pNodeData->m_emType == TreeNodeData::Node_LotArrivalTime)
	{
		HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
		if (hParentItem == NULL)
			return;
		TreeNodeData* pNonPaxNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
		if (pNodeData == NULL)
			return;
		NonPaxVehicleStrategy* pNonPax = (NonPaxVehicleStrategy*)pNonPaxNodeData->m_dwData;
		if (pNonPax == NULL)
			return;
		VehicleTimeEntry* pVehicleTime = new VehicleTimeEntry();
		ElapsedTime elatime;
		CDlgTimeRange dlg(elatime, elatime, pNonPax->IsDaily(), this);
		if(dlg.DoModal() == IDCANCEL)
			return;
		TimeRange Time(dlg.GetStartTime(), dlg.GetEndTime());						
		pVehicleTime->SetTimeRange(Time);
		pNonPax->AddTimeEntryItem(pVehicleTime);
		
		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this,cni);

		TreeNodeData* pTimeRangeNode = new TreeNodeData();
		pTimeRangeNode->m_emType = TreeNodeData::Node_TimeRange;
		pTimeRangeNode->m_dwData = (DWORD)pVehicleTime;
		cni.net = NET_SHOW_DIALOGBOX;
		CString strTimeRange;
		if (pNonPax->IsDaily())
		{
			strTimeRange.Format(_T("Start - End Time: %s - %s"),pVehicleTime->GetTimeRange().GetStartTime().printTime(),pVehicleTime->GetTimeRange().GetEndTime().printTime());
		} 
		else
		{
			strTimeRange.Format(_T("Start - End Time: %s - %s"),pVehicleTime->GetTimeRange().GetStartTime().PrintDateTime(),pVehicleTime->GetTimeRange().GetEndTime().PrintDateTime());
		} 
		
		HTREEITEM hTimeRangeItem = m_wndTreeCtrl.InsertItem(strTimeRange,cni,FALSE,FALSE,hItem);
		m_wndTreeCtrl.SetItemData(hTimeRangeItem,(DWORD)pTimeRangeNode);
		m_vNodeData.push_back(pTimeRangeNode);
		m_wndTreeCtrl.SelectItem(hTimeRangeItem);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
	if (pNodeData->m_emType == TreeNodeData::Node_TimeRange)
	{
		TreeNodeData* pNonPaxNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
		if (pNodeData == NULL)
			return;
		VehicleTimeEntry* pVehicleTime = (VehicleTimeEntry*)pNonPaxNodeData->m_dwData;
		if (pVehicleTime == NULL)
			return;
		VehicleTypeEntry* pVehicleType = new VehicleTypeEntry();

		CDlgSelectLandsideVehicleType dlg;
		CString strGroupName;
		if(dlg.DoModal() == IDCANCEL)
			return;
		strGroupName = dlg.GetName();
		if (!strGroupName.IsEmpty())
		{
			if (pVehicleTime->FindVehicleType(strGroupName))
			{
				MessageBox(_T("The vehicle Item is exit!"),NULL,MB_OK);
				return ;
			}
			pVehicleType->SetVehicleType(strGroupName);
		}
		
		pVehicleTime->AddvehicleTypeItem(pVehicleType);
		LoadVehicle(hItem,pVehicleTime);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
	if (pNodeData->m_emType == TreeNodeData::Node_ETA)
	{
		HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
		if (hParentItem == NULL)
			return;
		TreeNodeData* pNonPaxNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
		if (pNodeData == NULL)
			return;
		
		NonPaxVehicleStrategy* pNonPax = (NonPaxVehicleStrategy*)pNonPaxNodeData->m_dwData;
		if (pNonPax == NULL)
			return;


		CDlgSelectLandsideObject dlg(&m_pInLandside->getObjectList(),this);
		dlg.AddObjType(ALT_LEXT_NODE);
		if( dlg.DoModal() == IDCANCEL )
			return;

		ALTObjectID selectLot = dlg.getSelectObject();

		CEntryOffSet* pEntryOffSet = new CEntryOffSet();
		if (pNonPax->IsEntryExisted(selectLot))
		{
			MessageBox(_T("The item is exit!"),NULL,MB_OK);
			return;
		}
		pEntryOffSet->SetEntry(selectLot);
		CProDistrubution* pProDist = new CProDistrubution();
		pEntryOffSet->SetProDist(pProDist);

		pNonPax->AddEntryOffSetItem(pEntryOffSet);
		
		LoadEntryOffSet(hItem,pNonPax);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
	m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
	
}

void CDlgNonPassengerRelatedVehicleAssignment::OnDeleteObject()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem == NULL)
		return;

	HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
	if (hParentItem == NULL)
		return;

	TreeNodeData* pDelNode = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	if (pDelNode == NULL)
		return;

	TreeNodeData* pParentNode = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
	if (pParentNode == NULL)
		return;

	TreeNodeData::TreeNodeType DelNodeType = pDelNode->m_emType;
	if (DelNodeType == TreeNodeData::Node_Strategy)
	{
		HTREEITEM hNextSelItem = m_wndTreeCtrl.GetNextSiblingItem(hItem);
		if(hNextSelItem == NULL)
			hNextSelItem = m_wndTreeCtrl.GetPrevSiblingItem(hItem);
		if(hNextSelItem == NULL)
			hNextSelItem = m_wndTreeCtrl.GetParentItem(hItem);

		NonPaxVehicleStrategy* pDelNonPaxVehicleStrategy = (NonPaxVehicleStrategy*)pDelNode->m_dwData;
		m_pNonPaxVehicleAssignment->DeleteItem(pDelNonPaxVehicleStrategy);
	
		m_wndTreeCtrl.DeleteItem(hItem);

		if(hNextSelItem)
			m_wndTreeCtrl.SelectItem(hNextSelItem);
		

		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		UpdateToolBarState();
		return;
	}
	if (DelNodeType == TreeNodeData::Node_TimeRange)
	{	
		HTREEITEM hNextSelItem = m_wndTreeCtrl.GetNextSiblingItem(hItem);
		if(hNextSelItem == NULL)
			hNextSelItem = m_wndTreeCtrl.GetPrevSiblingItem(hItem);
		if(hNextSelItem == NULL)
			hNextSelItem = m_wndTreeCtrl.GetParentItem(hItem);

		HTREEITEM hNonPaxVehicleStrategy = m_wndTreeCtrl.GetParentItem(hParentItem);
		TreeNodeData* pStrategy = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hNonPaxVehicleStrategy);
		NonPaxVehicleStrategy* pNonPaxVehicleStrategy = (NonPaxVehicleStrategy*)pStrategy->m_dwData;

		VehicleTimeEntry* pDelVehicleTimeEntry = (VehicleTimeEntry*)pDelNode->m_dwData;

		pNonPaxVehicleStrategy->DeleteTimeEntryItem(pDelVehicleTimeEntry);
		m_wndTreeCtrl.DeleteItem(hItem);

		if(hNextSelItem)
			m_wndTreeCtrl.SelectItem(hNextSelItem);

		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		UpdateToolBarState();
		return;
	}
	if (DelNodeType == TreeNodeData::Node_Vehicle)
	{
		HTREEITEM hNextSelItem = m_wndTreeCtrl.GetNextSiblingItem(hItem);
		if(hNextSelItem == NULL)
			hNextSelItem = m_wndTreeCtrl.GetPrevSiblingItem(hItem);
		if(hNextSelItem == NULL)
			hNextSelItem = m_wndTreeCtrl.GetParentItem(hItem);

		VehicleTimeEntry* pVehicleTime = (VehicleTimeEntry*)pParentNode->m_dwData;
		VehicleTypeEntry* pVehicleType = (VehicleTypeEntry*)pDelNode->m_dwData;

		pVehicleTime->DeletevehicleTypeItem(pVehicleType);
		m_wndTreeCtrl.DeleteItem(hItem);

		LoadVehicle(hParentItem,pVehicleTime);
		if(hNextSelItem)
			m_wndTreeCtrl.SelectItem(hNextSelItem);

		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		UpdateToolBarState();
		return;
	}
	if (DelNodeType == TreeNodeData::Node_Entry)
	{
		HTREEITEM hNextSelItem = m_wndTreeCtrl.GetNextSiblingItem(hItem);
		if(hNextSelItem == NULL)
			hNextSelItem = m_wndTreeCtrl.GetPrevSiblingItem(hItem);
		if(hNextSelItem == NULL)
			hNextSelItem = m_wndTreeCtrl.GetParentItem(hItem);

		HTREEITEM hNonPaxVehicleStrategy = m_wndTreeCtrl.GetParentItem(hParentItem);
		TreeNodeData* pStrategy = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hNonPaxVehicleStrategy);
		NonPaxVehicleStrategy* pNonPaxVehicleStrategy = (NonPaxVehicleStrategy*)pStrategy->m_dwData;

		CEntryOffSet* pProDist = (CEntryOffSet*)pDelNode->m_dwData;

		pNonPaxVehicleStrategy->DeleteEntryOffSetItem(pProDist);
		m_wndTreeCtrl.DeleteItem(hItem);

		if(hNextSelItem)
			m_wndTreeCtrl.SelectItem(hNextSelItem);
		LoadEntryOffSet(hParentItem,pNonPaxVehicleStrategy);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		UpdateToolBarState();
		return;
	}
	if (DelNodeType == TreeNodeData::Node_Lot && pParentNode->m_emType == TreeNodeData::Node_Lot)
	{
		HTREEITEM hNextSelItem = m_wndTreeCtrl.GetNextSiblingItem(hItem);
		if(hNextSelItem == NULL)
			hNextSelItem = m_wndTreeCtrl.GetPrevSiblingItem(hItem);
		if(hNextSelItem == NULL)
			hNextSelItem = m_wndTreeCtrl.GetParentItem(hItem);

		ParkingLotRoute* pParentParkingLotRoute = (ParkingLotRoute*)pParentNode->m_dwData;
		ParkingLotRoute* pDelParkingLotRoute = (ParkingLotRoute*)pDelNode->m_dwData;

		pParentParkingLotRoute->DeleteItem(pDelParkingLotRoute);

		m_wndTreeCtrl.DeleteItem(hItem);

		if(hNextSelItem)
			m_wndTreeCtrl.SelectItem(hNextSelItem);

		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		UpdateToolBarState();
		return;

	}
	if (DelNodeType == TreeNodeData::Node_Lot)
	{
		HTREEITEM hNextSelItem = m_wndTreeCtrl.GetNextSiblingItem(hItem);
		if(hNextSelItem == NULL)
			hNextSelItem = m_wndTreeCtrl.GetPrevSiblingItem(hItem);
		if(hNextSelItem == NULL)
			hNextSelItem = m_wndTreeCtrl.GetParentItem(hItem);

		HTREEITEM hNonPaxVehicleStrategy = m_wndTreeCtrl.GetParentItem(hParentItem);
		TreeNodeData* pStrategy = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hNonPaxVehicleStrategy);
		NonPaxVehicleStrategy* pNonPaxVehicleStrategy = (NonPaxVehicleStrategy*)pStrategy->m_dwData;

		ParkingLotRoute* pDelParkingLotRoute = (ParkingLotRoute*)pDelNode->m_dwData;

		pNonPaxVehicleStrategy->DeleteLotRouteItem(pDelParkingLotRoute);
		m_wndTreeCtrl.DeleteItem(hItem);

		if(hNextSelItem)
			m_wndTreeCtrl.SelectItem(hNextSelItem);
		LoadArrivalAndETAOffset(hParentItem,pNonPaxVehicleStrategy);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		UpdateToolBarState();
		return;
	}
}

void CDlgNonPassengerRelatedVehicleAssignment::OnLvnItemchangedList( NMHDR *pNMHDR, LRESULT *pResult )
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	UpdateToolBarState();
	*pResult = 0;
}

void CDlgNonPassengerRelatedVehicleAssignment::OnContextMenu( CWnd* pWnd, CPoint point )
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
	case TreeNodeData::Node_Root:
		menuPopup.AppendMenu(MF_POPUP,ID_ACTYPE_ALIAS_ADD,_T("Add Strategy"));
		menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		break;
	case TreeNodeData::Node_Strategy:
		menuPopup.AppendMenu(MF_POPUP,ID_ACTYPE_ALIAS_DEL,_T("Delete Strategy"));
		menuPopup.AppendMenu(MF_POPUP,ID_ACTYPE_ALIAS_EDIT,_T("Edit Strategy"));
		menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		break;
	case TreeNodeData::Node_Destination:
		if(!m_wndTreeCtrl.ItemHasChildren(hRClickItem))
		{
			menuPopup.AppendMenu(MF_POPUP,ID_ACTYPE_ALIAS_ADD,_T("Add Destination"));
			menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		}
		break;
	case TreeNodeData::Node_Lot:
		menuPopup.AppendMenu(MF_POPUP,ID_ACTYPE_ALIAS_ADD,_T("Add Destination"));
		menuPopup.AppendMenu(MF_POPUP,ID_ACTYPE_ALIAS_EDIT,_T("Edit Destination"));
		menuPopup.AppendMenu(MF_POPUP,ID_ACTYPE_ALIAS_DEL,_T("Delete Destination"));
		menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		break;
	case TreeNodeData::Node_LotArrivalTime:
		menuPopup.AppendMenu(MF_POPUP,ID_ACTYPE_ALIAS_ADD,_T("Add TimeRange"));
		menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		break;
	case TreeNodeData::Node_TimeRange:
		menuPopup.AppendMenu(MF_POPUP,ID_ACTYPE_ALIAS_ADD,_T("Add VehicleType"));
		menuPopup.AppendMenu(MF_POPUP,ID_ACTYPE_ALIAS_EDIT,_T("Edit TimeRange"));
		menuPopup.AppendMenu(MF_POPUP,ID_ACTYPE_ALIAS_DEL,_T("Delete TimeRange"));
		menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		break;
	case TreeNodeData::Node_Vehicle:
		menuPopup.AppendMenu(MF_POPUP,ID_ACTYPE_ALIAS_DEL,_T("Delete VehicleType"));
		menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		break;
	case TreeNodeData::Node_VehicleName:
		menuPopup.AppendMenu(MF_POPUP,ID_ACTYPE_ALIAS_EDIT,_T("Edit VehicleType"));
		menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		break;
	case TreeNodeData::Node_VehicleNumber:
		menuPopup.AppendMenu(MF_POPUP,ID_ACTYPE_ALIAS_EDIT,_T("Edit VehicleType Number"));
		menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		break;
	case TreeNodeData::Node_ETA:
		menuPopup.AppendMenu(MF_POPUP,ID_ACTYPE_ALIAS_ADD,_T("Add Entry"));
		menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		break;
	case TreeNodeData::Node_Entry:
		menuPopup.AppendMenu(MF_POPUP,ID_ACTYPE_ALIAS_DEL,_T("Delete Entry"));
		menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		break;
	case TreeNodeData::Node_EntryName:
		menuPopup.AppendMenu(MF_POPUP,ID_ACTYPE_ALIAS_EDIT,_T("Edit Entry Name"));
		menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		break;
	case TreeNodeData::Node_ProDist:
		menuPopup.AppendMenu(MF_POPUP,ID_ACTYPE_ALIAS_EDIT,_T("Edit Entry Distribution"));
		menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		break;
	default:
		break;
	}
	menuPopup.DestroyMenu();
}

void CDlgNonPassengerRelatedVehicleAssignment::OnEditObject()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem == NULL)
		return;

	TreeNodeData* EditItemNode = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	if (EditItemNode == NULL)
		return;

	if (EditItemNode->m_emType ==TreeNodeData::Node_Strategy || EditItemNode->m_emType ==TreeNodeData::Node_Lot \
		|| EditItemNode->m_emType ==TreeNodeData::Node_LotArrivalTime || EditItemNode->m_emType ==TreeNodeData::Node_TimeRange\
		|| EditItemNode->m_emType ==TreeNodeData::Node_VehicleName || EditItemNode->m_emType ==TreeNodeData::Node_ProDist 
		|| EditItemNode->m_emType ==TreeNodeData::Node_VehicleNumber || EditItemNode->m_emType == TreeNodeData::Node_EntryName)
	{
		m_wndTreeCtrl.DoEdit(hItem);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
}

void CDlgNonPassengerRelatedVehicleAssignment::LoadSetStrategy( HTREEITEM hItem, NonPaxVehicleStrategy* pNonPaxVehicleStrategy )
{
	//strategy name
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	TreeNodeData* pStrategyNode = new TreeNodeData();
	pStrategyNode->m_emType = TreeNodeData::Node_Strategy;
	pStrategyNode->m_dwData = (DWORD)pNonPaxVehicleStrategy;

	cni.nt = NT_NORMAL;
	cni.net = NET_LABLE;

	CString strStrategyName;
	strStrategyName.Format(_T("Name: %s"),pNonPaxVehicleStrategy->GetStrategyName());
	HTREEITEM hStrategyNameItem = m_wndTreeCtrl.InsertItem(strStrategyName,cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hStrategyNameItem,(DWORD)pStrategyNode);
	m_vNodeData.push_back(pStrategyNode);
	
	//destination
	TreeNodeData* pDestinationNode = new TreeNodeData();
	pDestinationNode->m_emType = TreeNodeData::Node_Destination;

	cni.net = NET_NORMAL;

	CString strDestination;
	strDestination.Format(_T("Destination"));
	HTREEITEM hDestinationItem = m_wndTreeCtrl.InsertItem(strDestination,cni,FALSE,FALSE,hStrategyNameItem);
	m_wndTreeCtrl.SetItemData(hDestinationItem,(DWORD)pDestinationNode);
	m_vNodeData.push_back(pDestinationNode);

	//Lot Route 
	int nCount = pNonPaxVehicleStrategy->GetLotRouteCount();
	for (int i = 0; i < nCount; i++)
	{
		ParkingLotRoute* pParkingLotRoute = pNonPaxVehicleStrategy->GetLotRouteItem(i);
		LoadParkingLotRoute(hDestinationItem,pParkingLotRoute);
	}

	//lot a arrival time range distribution daily(Y/N)
	TreeNodeData* pLotArrivalTimeNode = new TreeNodeData();
	pLotArrivalTimeNode->m_emType = TreeNodeData::Node_LotArrivalTime;
	pLotArrivalTimeNode->m_dwData = (DWORD)pNonPaxVehicleStrategy->IsDaily();
	
	cni.net = NET_NORMAL;

	CString strLotArrivalTime;
	if (nCount > 0)
	{
		ParkingLotRoute* pParkingLot = pNonPaxVehicleStrategy->GetLotRouteItem(0);
		if (pNonPaxVehicleStrategy->IsDaily())
		{
			strLotArrivalTime.Format(_T("Lot: %s arrival time range distribution:"),pParkingLot->GetParkingLot().GetIDString());
		} 
		else
		{
			strLotArrivalTime.Format(_T("Lot: %s arrival time range distribution:"),pParkingLot->GetParkingLot().GetIDString());
		}	
	}
	else
	{
		if (pNonPaxVehicleStrategy->IsDaily())
		{
			strLotArrivalTime.Format(_T("Lot: arrival time range distribution:"));
		} 
		else
		{
			strLotArrivalTime.Format(_T("Lot: arrival time range distribution:"));
		}	
	}
	HTREEITEM hLotArrivalTime = m_wndTreeCtrl.InsertItem(strLotArrivalTime,cni,FALSE,FALSE,hStrategyNameItem);
	m_wndTreeCtrl.SetItemData(hLotArrivalTime,(DWORD)pLotArrivalTimeNode);
	m_vNodeData.push_back(pLotArrivalTimeNode);
	
	//time range
	LoadVehicleTimeEntry(hLotArrivalTime,pNonPaxVehicleStrategy);
		
	m_wndTreeCtrl.Expand(hLotArrivalTime,TVE_EXPAND);
	//ETA Offset LOT A
	TreeNodeData* pETANode = new TreeNodeData();
	pETANode->m_emType = TreeNodeData::Node_ETA;
	cni.net = NET_STATIC;
	CString strEta;
	if (nCount > 0)
	{
		ParkingLotRoute* pParkingLot = pNonPaxVehicleStrategy->GetLotRouteItem(0);
		strEta.Format(_T("ETA offset Lot: %s"), pParkingLot->GetParkingLot().GetIDString());
	}
	else
	{
		strEta.Format(_T("ETA Offset Lot:"));
	}
	HTREEITEM hETAItem = m_wndTreeCtrl.InsertItem(strEta,cni,FALSE,FALSE,hStrategyNameItem);
	m_wndTreeCtrl.SetItemData(hETAItem,(DWORD)pETANode);
	m_vNodeData.push_back(pETANode);
	
	LoadEntryOffSet(hETAItem,pNonPaxVehicleStrategy);
	m_wndTreeCtrl.Expand(hETAItem,TVE_EXPAND);
	m_wndTreeCtrl.Expand(hStrategyNameItem,TVE_EXPAND);
	m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
}

void CDlgNonPassengerRelatedVehicleAssignment::LoadParkingLotRoute( HTREEITEM hItem,ParkingLotRoute* pParkingLotRoute )
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt = NT_NORMAL;
	cni.net = NET_COMBOBOX;
	TreeNodeData* pLotNode = new TreeNodeData();
	pLotNode->m_emType = TreeNodeData::Node_Lot;
	pLotNode->m_dwData = (DWORD)pParkingLotRoute;
	m_vNodeData.push_back(pLotNode);

	CString strLot;
	strLot.Format(_T("%s,  TA Time(mins): %s"),pParkingLotRoute->GetParkingLot().GetIDString(),pParkingLotRoute->GetProDist().getPrintDist());
	HTREEITEM hLotItem = m_wndTreeCtrl.InsertItem(strLot,cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hLotItem,(DWORD_PTR)pLotNode);
	m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
	m_wndTreeCtrl.SelectItem(hLotItem);


	for (int i = 0; i < pParkingLotRoute->GetItemCount(); i++)
	{
		ParkingLotRoute* pParkLot = (ParkingLotRoute*)pParkingLotRoute->GetItem(i);
		LoadParkingLotRoute(hLotItem,pParkLot);
	}
	m_wndTreeCtrl.Expand(hLotItem,TVE_EXPAND);
}

void CDlgNonPassengerRelatedVehicleAssignment::LoadVehicleTimeEntry( HTREEITEM hItem, NonPaxVehicleStrategy* pNonPaxVehicleStrategy )
{
	HTREEITEM hChildItem = m_wndTreeCtrl.GetChildItem(hItem);
	while(hChildItem)
	{
		m_wndTreeCtrl.DeleteItem(hChildItem);
		hChildItem = m_wndTreeCtrl.GetChildItem(hItem);
	}

	COOLTREE_NODE_INFO cn;
	CCoolTree::InitNodeInfo(this,cn);

	TreeNodeData* pDailyNode = new TreeNodeData();
	pDailyNode->m_emType = TreeNodeData::Node_Daily;
	cn.net = NET_COMBOBOX;
	bool daily = pNonPaxVehicleStrategy->IsDaily();
	CString strDaliy;
	if (daily)
	{
		strDaliy.Format(_T("Daily: %s"),_T("Yes"));
	}
	else
	{
		strDaliy.Format(_T("Daily: %s"),_T("No"));			
	}
	HTREEITEM hDailyItem = m_wndTreeCtrl.InsertItem(strDaliy,cn,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hDailyItem,(DWORD)pDailyNode);
	m_vNodeData.push_back(pDailyNode);

	for (int i = 0; i < pNonPaxVehicleStrategy->GetTimeEntryCount(); i++)
	{
		VehicleTimeEntry* pTimeEntry = pNonPaxVehicleStrategy->GetTimeEntryItem(i);
		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this,cni);

		TreeNodeData* pTimeRangeNode = new TreeNodeData();
		pTimeRangeNode->m_emType = TreeNodeData::Node_TimeRange;
		pTimeRangeNode->m_dwData = (DWORD)pTimeEntry;
		cni.net = NET_SHOW_DIALOGBOX;
		CString strTimeRange;
		int nDay = pTimeEntry->GetTimeRange().GetEndTime().GetDay();
		if (nDay >= 1 && !daily)
		{
			strTimeRange.Format(_T("Start - End Time: %s - %s"),pTimeEntry->GetTimeRange().GetStartTime().PrintDateTime(),pTimeEntry->GetTimeRange().GetEndTime().PrintDateTime());
		} 
		else
		{
			strTimeRange.Format(_T("Start - End Time: %s - %s"),pTimeEntry->GetTimeRange().GetStartTime().printTime(),pTimeEntry->GetTimeRange().GetEndTime().printTime());
		}

		HTREEITEM hTimeRangeItem = m_wndTreeCtrl.InsertItem(strTimeRange,cni,FALSE,FALSE,hItem);
		m_wndTreeCtrl.SetItemData(hTimeRangeItem,(DWORD)pTimeRangeNode);
		m_vNodeData.push_back(pTimeRangeNode);

		//vehicle list
		LoadVehicle(hTimeRangeItem,pTimeEntry);
		m_wndTreeCtrl.Expand(hTimeRangeItem,TVE_EXPAND);
	}
	m_wndTreeCtrl.SelectItem(hItem);
	m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
	
}

void CDlgNonPassengerRelatedVehicleAssignment::LoadEntryOffSet( HTREEITEM hItem, NonPaxVehicleStrategy* pNonPaxVehicleStrategy )
{
	HTREEITEM hChildItem = m_wndTreeCtrl.GetChildItem(hItem);
	while(hChildItem)
	{
		m_wndTreeCtrl.DeleteItem(hChildItem);
		hChildItem = m_wndTreeCtrl.GetChildItem(hItem);
	}

	for (int i = 0; i < pNonPaxVehicleStrategy->GetEntryOffSetCount(); i++)
	{	
		CEntryOffSet* pEntryOffSet = pNonPaxVehicleStrategy->GetEntryOffSetItem(i);

		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this,cni);
		//entry
		TreeNodeData* pEntryNode = new TreeNodeData();
		pEntryNode->m_emType = TreeNodeData::Node_Entry;
		pEntryNode->m_dwData = (DWORD)pEntryOffSet;
		CString strEntry;
		strEntry.Format(_T("Entry %d"),i+1);
		HTREEITEM hEntry = m_wndTreeCtrl.InsertItem(strEntry,cni,FALSE,FALSE,hItem);
		m_wndTreeCtrl.SetItemData(hEntry,(DWORD)pEntryNode);
		m_vNodeData.push_back(pEntryNode);

		//entry name
		cni.net = NET_SHOW_DIALOGBOX;
		TreeNodeData* pEntryNameNode = new TreeNodeData();
		pEntryNameNode->m_emType = TreeNodeData::Node_EntryName;
		CString strEntryName;
		strEntryName.Format(_T("Entry Name: %s"),pEntryOffSet->GetEntry().GetIDString());
		HTREEITEM hEntryName = m_wndTreeCtrl.InsertItem(strEntryName,cni,FALSE,FALSE,hEntry);
		m_wndTreeCtrl.SetItemData(hEntryName,(DWORD)pEntryNameNode);
		m_vNodeData.push_back(pEntryNameNode);

		//entry ProDistrubution
		cni.net = NET_COMBOBOX;
		TreeNodeData* pProDistNode = new TreeNodeData();
		pProDistNode->m_emType = TreeNodeData::Node_ProDist;
		pProDistNode->m_dwData = (DWORD)pEntryOffSet->GetProDist();
		CString strProDist;
		strProDist.Format(_T("Distrubution(mins): %s"),pEntryOffSet->GetProDist()->getDistName());
		HTREEITEM hProDist = m_wndTreeCtrl.InsertItem(strProDist,cni,FALSE,FALSE,hEntry);
		m_wndTreeCtrl.SetItemData(hProDist,(DWORD)pProDistNode);
		m_vNodeData.push_back(pProDistNode);
		m_wndTreeCtrl.Expand(hEntry,TVE_EXPAND);
		m_wndTreeCtrl.SelectItem(hProDist);
	}
	m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
}

void CDlgNonPassengerRelatedVehicleAssignment::LoadVehicle( HTREEITEM hItem, VehicleTimeEntry* pTimeEntry )
{
	HTREEITEM hChildItem = m_wndTreeCtrl.GetChildItem(hItem);
	while(hChildItem)
	{
		m_wndTreeCtrl.DeleteItem(hChildItem);
		hChildItem = m_wndTreeCtrl.GetChildItem(hItem);
	}
	for (int i = 0; i < pTimeEntry->GetvehicleTypeCount(); i++)
	{
		VehicleTypeEntry* pVehicleType = pTimeEntry->GetvehicleTypeItem(i);
		
		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this,cni);

		//vehicle
		TreeNodeData* pVehicleNode = new TreeNodeData();
		pVehicleNode->m_emType = TreeNodeData::Node_Vehicle;
		pVehicleNode->m_dwData = (DWORD)pVehicleType;

		cni.net = NET_NORMAL;
		CString strVehicle;
		strVehicle.Format(_T("Vehicle %d"),i+1);
		HTREEITEM hVehicleItem = m_wndTreeCtrl.InsertItem(strVehicle,cni,FALSE,FALSE,hItem);
		m_wndTreeCtrl.SetItemData(hVehicleItem,(DWORD)pVehicleNode);
		m_vNodeData.push_back(pVehicleNode);

		//vehicle name
		TreeNodeData* pVehicleNameNode = new TreeNodeData();
		pVehicleNameNode->m_emType = TreeNodeData::Node_VehicleName;

		cni.net = NET_SHOW_DIALOGBOX;
		CString strVehicleName;
		strVehicleName.Format(_T("Vehicle Name: %s"),pVehicleType->GetVehicleType());
		HTREEITEM hVehicleNameItem = m_wndTreeCtrl.InsertItem(strVehicleName,cni,FALSE,FALSE,hVehicleItem);
		m_wndTreeCtrl.SetItemData(hVehicleNameItem,(DWORD)pVehicleNameNode);
		m_vNodeData.push_back(pVehicleNameNode);

		//vehicle number
		TreeNodeData* pVehicleNumberNode = new TreeNodeData();
		pVehicleNumberNode->m_emType = TreeNodeData::Node_VehicleNumber;

		cni.net = NET_EDIT_INT;
		cni.fMinValue = static_cast<float>(pVehicleType->GetVehicleNumber());
		CString strVehicleNumber;
		strVehicleNumber.Format(_T("Vehicle Number: %d"),pVehicleType->GetVehicleNumber());
		HTREEITEM hVehicleNumberItem = m_wndTreeCtrl.InsertItem(strVehicleNumber,cni,FALSE,FALSE,hVehicleItem);
		m_wndTreeCtrl.SetItemData(hVehicleNumberItem,(DWORD)pVehicleNumberNode);
		m_vNodeData.push_back(pVehicleNumberNode);
		m_wndTreeCtrl.Expand(hVehicleItem,TVE_EXPAND);
		m_wndTreeCtrl.SelectItem(hVehicleNumberItem);
	}
}

void CDlgNonPassengerRelatedVehicleAssignment::ChangeTheTime( NonPaxVehicleStrategy* pNonPaxVehicleStrategy )
{
	bool daliy = pNonPaxVehicleStrategy->IsDaily();
	for (int i = 0; i < pNonPaxVehicleStrategy->GetTimeEntryCount(); i++)
	{
		VehicleTimeEntry* pTimeEntry = pNonPaxVehicleStrategy->GetTimeEntryItem(i);
		int nStartDay = pTimeEntry->GetTimeRange().GetStartTime().GetDay();
		int nEndDay = pTimeEntry->GetTimeRange().GetEndTime().GetDay();
		if (daliy && nEndDay > 1)
		{
			ElapsedTime StartTime = pTimeEntry->GetTimeRange().GetStartTime();
			StartTime.SetDay(1);
			
			ElapsedTime EndTime = pTimeEntry->GetTimeRange().GetEndTime();
			EndTime.SetDay(1);
			TimeRange TimeRange(StartTime,EndTime);

			pTimeEntry->SetTimeRange(TimeRange);
		}
		if (!daliy && nEndDay <2)
		{
			ElapsedTime StartTime = pTimeEntry->GetTimeRange().GetStartTime();
			StartTime.SetDay(1);

			ElapsedTime EndTime = pTimeEntry->GetTimeRange().GetEndTime();
			EndTime.SetDay(1);
			TimeRange TimeRange(StartTime,EndTime);

			pTimeEntry->SetTimeRange(TimeRange);
		}
	}
}

bool CDlgNonPassengerRelatedVehicleAssignment::IsTheLotItemExit( HTREEITEM hItem, ALTObjectID& objectID )
{	
	TreeNodeData* pParentNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	while(hItem && (pParentNodeData->m_emType == TreeNodeData::Node_Lot))
	{	
		ParkingLotRoute* pParkingLot = (ParkingLotRoute*)pParentNodeData->m_dwData;
		if (pParkingLot->GetParkingLot().GetIDString().Compare(objectID.GetIDString()) == NULL)
		{
			return true;
		}
		hItem = m_wndTreeCtrl.GetParentItem(hItem);
		pParentNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	}
	return false;
}

void CDlgNonPassengerRelatedVehicleAssignment::LoadArrivalAndETAOffset( HTREEITEM hItem, NonPaxVehicleStrategy* pNonPaxVehicleStrategy )
{
	HTREEITEM hArrivalItem = m_wndTreeCtrl.GetNextSiblingItem(hItem);
	if (hArrivalItem == NULL)
		return;
	TreeNodeData* pArrivalNode = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hArrivalItem);
	if (pArrivalNode == NULL)
		return;
	int nCount = pNonPaxVehicleStrategy->GetLotRouteCount();
	if (pArrivalNode->m_emType == TreeNodeData::Node_LotArrivalTime)
	{
		//Arrival
		TreeNodeData* pLotArrivalTimeNode = new TreeNodeData();
		pLotArrivalTimeNode->m_emType = TreeNodeData::Node_LotArrivalTime;

		
		CString strLotArrivalTime;
		if (nCount > 0)
		{
			ParkingLotRoute* pParkingLot = pNonPaxVehicleStrategy->GetLotRouteItem(0);
			if (pNonPaxVehicleStrategy->IsDaily())
			{
				strLotArrivalTime.Format(_T("Lot: %s arrival time range distribution:"),pParkingLot->GetParkingLot().GetIDString());
			} 
			else
			{
				strLotArrivalTime.Format(_T("Lot: %s arrival time range distribution:"),pParkingLot->GetParkingLot().GetIDString());
			}	
		}
		else
		{
			if (pNonPaxVehicleStrategy->IsDaily())
			{
				strLotArrivalTime.Format(_T("Lot: arrival time range distribution:"));
			} 
			else
			{
				strLotArrivalTime.Format(_T("Lot: arrival time range distribution:"));
			}	
		}
		m_wndTreeCtrl.SetItemText(hArrivalItem,strLotArrivalTime);
		m_wndTreeCtrl.SetItemData(hArrivalItem,(DWORD)pLotArrivalTimeNode);
		m_vNodeData.push_back(pLotArrivalTimeNode);	
	}

	HTREEITEM hETAOffset = m_wndTreeCtrl.GetNextSiblingItem(hArrivalItem);
	if (hETAOffset == NULL)
		return;
	TreeNodeData* pETAOffset = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hETAOffset);
	if (pETAOffset == NULL)
		return;
	
	if (pETAOffset->m_emType == TreeNodeData::Node_ETA)
	{
		//ETA Offset LOT A
		TreeNodeData* pETANode = new TreeNodeData();
		pETANode->m_emType = TreeNodeData::Node_ETA;
		CString strEta;
		if (nCount > 0)
		{
			ParkingLotRoute* pParkingLot = pNonPaxVehicleStrategy->GetLotRouteItem(0);
			strEta.Format(_T("ETA Offset Lot: %s"),pParkingLot->GetParkingLot().GetIDString());
		}
		else
		{
			strEta.Format(_T("ETA Offset Lot:"));
		}
		m_wndTreeCtrl.SetItemText(hETAOffset,strEta);
		m_wndTreeCtrl.SetItemData(hETAOffset,(DWORD)pETANode);
		m_vNodeData.push_back(pETANode);
	}
			
}
