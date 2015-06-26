// DlgConflictResolution.cpp : implementation file
//

#include "stdafx.h"
//#include "AirsideGUI.h"
#include "Resource.h"
#include "DlgConflictResolution.h"
#include ".\dlgconflictresolution.h"
#include "..\InputAirside\ConflictResolution.h"
#include "..\InputAirside\ALTObject.h"
#include "DlgConflictSelectTaxiway.h"
#include "..\InputAirside\ConflictAtIntersectionOnRight.h"
#include "..\InputAirside\ConflictFirstInATaxiway.h"
#include "..\InputAirside\ConflictOnSpecificTaxiways.h"
#include "..\InputAirside\ALTAirport.h"
#include "..\common\AirportDatabase.h"
#include "../Database//DBElementCollection_Impl.h"
#include "Common/CARCUnit.h"


static const UINT ID_NEW_LIST = 10;
static const UINT ID_DEL_ITEM = 11;
static const UINT ID_EDIT_ITEM = 12;

// CDlgConflictResolution dialog

IMPLEMENT_DYNAMIC(CDlgConflictResolution, CXTResizeDialog)
CDlgConflictResolution::CDlgConflictResolution(int nProjID, PFuncSelectFlightType pSelectFlightType, CAirportDatabase* pAirportDB, CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgConflictResolution::IDD, pParent)
	,m_nProjID(nProjID)
	,m_pSelectFlightType(pSelectFlightType)
	,m_pAirportDB(pAirportDB)
{
	m_pConflictResolution = new CConflictResolution();
	m_hRunwayCrossBuffer = NULL;
	m_hRadiusOfConcern = NULL;
	m_hAtIntersectionOnRight = NULL;
	m_hFirstInATaxiway = NULL;
	m_hOnSpecificTaxiways= NULL;
	m_hRunwayCrossBuffer = NULL;
}

CDlgConflictResolution::~CDlgConflictResolution()
{
	delete m_pConflictResolution;
}

void CDlgConflictResolution::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_CONFLICTRESOLUTION, m_wndConflictResolutionTree);
}


BEGIN_MESSAGE_MAP(CDlgConflictResolution, CXTResizeDialog)
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_BN_CLICKED(IDC_SAVE, OnBnClickedSave)
	ON_COMMAND(ID_NEW_MENULIST, OnNewItem)
	ON_COMMAND(ID_DELETE_MENUITEM, OnDelItem)
	ON_COMMAND(ID_EDIT_MENUITEM, OnEditItem)
	ON_COMMAND(ID_NEW_LIST, OnNewItem)
	ON_COMMAND(ID_DEL_ITEM, OnDelItem)
	ON_COMMAND(ID_EDIT_ITEM, OnEditItem)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_CONFLICTRESOLUTION, OnTvnSelchangedTreeConflictresolution)
END_MESSAGE_MAP()


// CDlgConflictResolution message handlers

void CDlgConflictResolution::OnBnClickedSave()
{
	// TODO: Add your control notification handler code here
	try
	{
		CADODatabase::BeginTransaction() ;
		m_pConflictResolution->SaveData(m_nProjID); 
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	
	//CXTResizeDialog::OnOK();
}


void CDlgConflictResolution::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	try
	{
		CADODatabase::BeginTransaction() ;
		m_pConflictResolution->SaveData(m_nProjID); 
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	CXTResizeDialog::OnOK();
}

BOOL CDlgConflictResolution::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();
	// TODO:  Add extra initialization here
	InitToolBar();
	SetResize(m_wndToolbar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_TREE_CONFLICTRESOLUTION, SZ_TOP_LEFT , SZ_BOTTOM_RIGHT); 	
	SetResize(IDC_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	try
	{
		m_pConflictResolution->SetAirportDB(m_pAirportDB);
		m_pConflictResolution->ReadData(m_nProjID);
	}	
	catch (CADOException&)
	{
		AfxMessageBox("Error Reading Conflict Resolution Data");
		return FALSE;
	}	

	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjID, vAirportIds);
	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	{
		ALTAirport airport;
		airport.ReadAirport(*iterAirportID);
	
		std::vector<ALTObject> vTaxiway;
		ALTObject::GetObjectList(ALT_TAXIWAY,*iterAirportID,vTaxiway);

		TaxiwayVector vTaxiwayNameID; 
		int i = 0;
		for (; i < static_cast<int>(vTaxiway.size()); ++ i)
		{
			vTaxiwayNameID.push_back(std::make_pair(vTaxiway[i].GetObjNameString(),vTaxiway[i].getID()));
		}
		if(i>0)
			m_TaxiwayVectorMap.insert(std::make_pair(airport.getName(),vTaxiwayNameID));
	}
	if (m_TaxiwayVectorMap.size() >0)
	{
		TaxiwayVectorMapIter iter = m_TaxiwayVectorMap.begin();
		TaxiwayVector& vec = iter->second;
		TaxiwayVectorIter it = vec.begin();
		m_strDefaultTaxiwayName = it->first;
		m_nDefaultTaxiwayID = it->second;
	}
	m_pTaxiwayVectorMap = &m_TaxiwayVectorMap;

	LoadTree();

	
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

int CDlgConflictResolution::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	if (!m_wndToolbar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_wndToolbar.LoadToolBar(IDR_TOOLBAR_ADD_DEL_EDIT))
	{
		return -1;
	}

	CToolBarCtrl& wndToolbarCtrl = m_wndToolbar.GetToolBarCtrl();
	wndToolbarCtrl.SetCmdID(0, ID_NEW_LIST);
	wndToolbarCtrl.SetCmdID(1, ID_DEL_ITEM);
	wndToolbarCtrl.SetCmdID(2, ID_EDIT_ITEM);

	return 0;
}
void CDlgConflictResolution::InitToolBar()
{
	// set the position of the runway toolbar
	CRect rectToolbar;
	m_wndConflictResolutionTree.GetWindowRect(&rectToolbar);
	ScreenToClient(&rectToolbar);
	rectToolbar.top -= 26;
	rectToolbar.bottom = rectToolbar.top + 22;
	rectToolbar.left += 4;
	m_wndToolbar.MoveWindow(&rectToolbar);

	CToolBarCtrl& wndToolbarCtrl = m_wndToolbar.GetToolBarCtrl();
	wndToolbarCtrl.EnableButton(ID_NEW_LIST,FALSE);
	wndToolbarCtrl.EnableButton(ID_DEL_ITEM,FALSE);
	wndToolbarCtrl.EnableButton(ID_EDIT_ITEM,FALSE);

}

void CDlgConflictResolution::UpdateToolBar()
{
	HTREEITEM hSelItem;
	hSelItem =  m_wndConflictResolutionTree.GetSelectedItem();
	HTREEITEM hParentItem = m_wndConflictResolutionTree.GetParentItem(hSelItem);
	if(hSelItem == NULL || hSelItem == m_hRadiusOfConcern || hSelItem == m_hRunwayCrossBuffer)
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_NEW_LIST, FALSE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_DEL_ITEM, FALSE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_ITEM, FALSE);
	}
	else if(hParentItem == m_hAtIntersectionOnRight || hParentItem == m_hFirstInATaxiway || hParentItem == m_hOnSpecificTaxiways)
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_NEW_LIST, FALSE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_DEL_ITEM, TRUE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_ITEM, FALSE);
	}
	else
	{
		if(hSelItem == m_hAtIntersectionOnRight || hSelItem == m_hFirstInATaxiway
			|| hSelItem == m_hOnSpecificTaxiways)
		{
			CString strYesOrNo = m_wndConflictResolutionTree.GetItemText(hSelItem);
			strYesOrNo = strYesOrNo.Right(2);
			if(hSelItem == m_hAtIntersectionOnRight && strcmp(strYesOrNo,"No") == 0
				||hSelItem == m_hFirstInATaxiway && strcmp(strYesOrNo,"No") == 0
				||hSelItem == m_hOnSpecificTaxiways && strcmp(strYesOrNo,"es") == 0)
			{
				m_wndToolbar.GetToolBarCtrl().EnableButton(ID_NEW_LIST, TRUE);
				m_wndToolbar.GetToolBarCtrl().EnableButton(ID_DEL_ITEM, FALSE);
				m_wndToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_ITEM, FALSE);
			}
			else
			{
				m_wndToolbar.GetToolBarCtrl().EnableButton(ID_NEW_LIST, FALSE);
				m_wndToolbar.GetToolBarCtrl().EnableButton(ID_DEL_ITEM, FALSE);
				m_wndToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_ITEM, FALSE);
			}
		}
		else
		{
			m_wndToolbar.GetToolBarCtrl().EnableButton(ID_NEW_LIST, FALSE);
			m_wndToolbar.GetToolBarCtrl().EnableButton(ID_DEL_ITEM, FALSE);
			m_wndToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_ITEM, TRUE);
		}
	}
}
void CDlgConflictResolution::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: Add your message handler code here
	CRect rectTree;
	m_wndConflictResolutionTree.GetWindowRect(&rectTree);
	if (!rectTree.PtInRect(point)) 
		return;

	m_wndConflictResolutionTree.SetFocus();
	CPoint pt = point;
	m_wndConflictResolutionTree.ScreenToClient(&pt);
	UINT iRet;
	HTREEITEM hRClickItem = m_wndConflictResolutionTree.HitTest(pt, &iRet);
	if (iRet != TVHT_ONITEMLABEL)
	{
		hRClickItem = NULL;
		return;
	}

	m_wndConflictResolutionTree.SelectItem(hRClickItem);

	CMenu menuPopup;
	menuPopup.LoadMenu(IDR_MENU_CONFLICTRESOLUTION);
	CMenu* pSubMenu = NULL;

	HTREEITEM hParentItem = m_wndConflictResolutionTree.GetParentItem(hRClickItem);

	if (hParentItem == NULL && hRClickItem != m_hRadiusOfConcern)
	{
		CString strYesOrNo = m_wndConflictResolutionTree.GetItemText(hRClickItem);
		strYesOrNo = strYesOrNo.Right(2);
		if(hRClickItem == m_hAtIntersectionOnRight && strcmp(strYesOrNo,"No") == 0 || hRClickItem == m_hFirstInATaxiway && strcmp(strYesOrNo,"No") == 0
		   || hRClickItem == m_hOnSpecificTaxiways && strcmp(strYesOrNo,"es") == 0)
				pSubMenu = menuPopup.GetSubMenu(0);
	}	
	
	if(hRClickItem == m_hCaseLanding || hRClickItem == m_hCaseTakeoff)
	{
		pSubMenu = menuPopup.GetSubMenu(0);
	}
	else if(hParentItem == m_hCaseLanding || hParentItem == m_hCaseTakeoff)
	{
		pSubMenu = menuPopup.GetSubMenu(1);		
	}

	
	if(hParentItem ==m_hAtIntersectionOnRight || hParentItem== m_hFirstInATaxiway|| hParentItem==m_hOnSpecificTaxiways)
	{	
		pSubMenu = menuPopup.GetSubMenu(1);
		pSubMenu->DeleteMenu(ID_EDIT_MENUITEM ,MF_BYCOMMAND);		
	}
	else if(hParentItem)
	{
		HTREEITEM hParentParent = m_wndConflictResolutionTree.GetParentItem(hParentItem);		
		if(hParentParent ==m_hAtIntersectionOnRight || hParentParent== m_hFirstInATaxiway|| hParentParent==m_hOnSpecificTaxiways)
		{
			pSubMenu = menuPopup.GetSubMenu(1);
			pSubMenu->DeleteMenu(ID_DELETE_MENUITEM ,MF_BYCOMMAND);	
		}
	}

	if (pSubMenu != NULL)
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
}

LRESULT CDlgConflictResolution::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

	switch(message)
	{	
		case UM_CEW_EDITSPIN_END:
			{
				HTREEITEM hItem=(HTREEITEM)wParam;
				CString strValue = *((CString*)lParam);
				int ntemp = (int)atoi( strValue.GetBuffer() );
				
				CString strtemp;	
				if (hItem == m_hRadiusOfConcern)
				{
					m_pConflictResolution->SetRadiusOfConcern(ntemp);
					strtemp.Format("%s:%s","Radius of concern(ft)",strValue);
				}
				else if(hItem == m_hRunwayCrossBuffer)
				{
					m_pConflictResolution->SetRunwayCrossBuffer(ntemp);
					strtemp.Format("%s:%s","Runway Cross Buffer(secs)",strValue);
				}
				if(isBuffeTime(hItem))
				{
					HTREEITEM hItemParent  = m_wndConflictResolutionTree.GetParentItem(hItem);
					RunwayCrossBuffer* buffer =  (RunwayCrossBuffer*)m_wndConflictResolutionTree.GetItemData(hItemParent);
					buffer->setTime( ElapsedTime((double)ntemp) );
					strtemp.Format(_T("Buffer Time(secs):%d"), ntemp);
				}

				if(isBuffeDist(hItem))
				{
					HTREEITEM hItemParent  = m_wndConflictResolutionTree.GetParentItem(hItem);
					RunwayCrossBuffer* buffer =  (RunwayCrossBuffer*)m_wndConflictResolutionTree.GetItemData(hItemParent);
				
					buffer->setDistanceNM(ntemp);
					strtemp.Format(_T("Buffer Distance(nm):%d"), ntemp);

				}
				if(hItem == m_hApproachDistance)
				{
					m_pConflictResolution->setRunwayAsTaxiwayApproachDistNM(ntemp);
					strtemp.Format(_T("Distance(nm): %d"),ntemp);
				}
				if(hItem == m_hApproachTime)
				{
					m_pConflictResolution->setRunwayAsTaxiwayApproachTime(ElapsedTime((double)ntemp));
					strtemp.Format(_T("Time(secs): %d"),ntemp);
				}
				if(hItem == m_hTakeoffSeconds)
				{
					m_pConflictResolution->setRunwayAsTaxiwayTakeoffTime(ElapsedTime((double)ntemp));
					strtemp.Format(_T("No takeoff scheduled in the next(secs): %d"),ntemp);
				}
				

				m_wndConflictResolutionTree.SetItemText(hItem,strtemp);
				m_wndConflictResolutionTree.SetItemData(hItem,ntemp);
			}
			break;
		case UM_CEW_COMBOBOX_BEGIN:
			{
				CWnd* pWnd=m_wndConflictResolutionTree.GetEditWnd((HTREEITEM)wParam);
				CRect rectWnd;
				m_wndConflictResolutionTree.GetItemRect((HTREEITEM)wParam,rectWnd,TRUE);
				pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
				CComboBox* pCB=(CComboBox*)pWnd;
				if(pCB->GetCount()==0)
				{
					pCB->AddString(_T("Yes"));
					pCB->AddString(_T("No"));
				}
			}
			break;
		case UM_CEW_COMBOBOX_SELCHANGE:
			{
				HTREEITEM hItem=(HTREEITEM)wParam;
				CString strText=*(CString*)lParam;
				CString strTextOld = m_wndConflictResolutionTree.GetItemText(hItem);
				if(strcmp(strText,strTextOld) != 0)
				{
					if(hItem == m_hAtIntersectionOnRight)
					{
						CString strAtIntersectionOnRight;
						strAtIntersectionOnRight = "At intersection Aircraft on right has right of way: ";
						if(strcmp(strText,"Yes") == 0)
						{
							m_pConflictResolution->SetAtIntersectionOnRight(Yes);
							strAtIntersectionOnRight += "Yes";
							CAtIntersectionOnRightList* pAtIntersectionOnRightList = m_pConflictResolution->GetAtIntersectionOnRightList();
							pAtIntersectionOnRightList->DeleteData();
							if (m_wndConflictResolutionTree.ItemHasChildren(m_hAtIntersectionOnRight))
							{
								HTREEITEM hNextItem;
								HTREEITEM hChildItem = m_wndConflictResolutionTree.GetChildItem(m_hAtIntersectionOnRight);

								while (hChildItem != NULL)
								{
									hNextItem = m_wndConflictResolutionTree.GetNextItem(hChildItem, TVGN_NEXT);
									m_wndConflictResolutionTree.DeleteItem(hChildItem);
									hChildItem = hNextItem;
								}
							}
						}
						else
						{
							m_pConflictResolution->SetAtIntersectionOnRight(No);
							strAtIntersectionOnRight += "No";
						}
						m_wndConflictResolutionTree.SetItemText(m_hAtIntersectionOnRight,strAtIntersectionOnRight);
					}
					else if(hItem == m_hFirstInATaxiway)
					{
						CString strFirstInATaxiway;
						strFirstInATaxiway = "Aircraft first in a taxiway has right of way: ";
						if(strcmp(strText,"Yes") == 0)
						{
							m_pConflictResolution->SetFirstInATaxiway(Yes);
							strFirstInATaxiway += "Yes";
							CFirstInATaxiwayList* pFirstInATaxiwayList = m_pConflictResolution->GetFirstInATaxiwayList();
							pFirstInATaxiwayList->DeleteData();
							if (m_wndConflictResolutionTree.ItemHasChildren(m_hFirstInATaxiway))
							{
								HTREEITEM hNextItem;
								HTREEITEM hChildItem = m_wndConflictResolutionTree.GetChildItem(m_hFirstInATaxiway);

								while (hChildItem != NULL)
								{
									hNextItem = m_wndConflictResolutionTree.GetNextItem(hChildItem, TVGN_NEXT);
									m_wndConflictResolutionTree.DeleteItem(hChildItem);
									hChildItem = hNextItem;
								}
							}
						}
						else
						{
							m_pConflictResolution->SetFirstInATaxiway(No);
							strFirstInATaxiway += "No";
						}
						m_wndConflictResolutionTree.SetItemText(m_hFirstInATaxiway,strFirstInATaxiway);						
					}
					else if(hItem == m_hOnSpecificTaxiways)
					{
						CString strOnSpecificTaxiways;
						strOnSpecificTaxiways = "Aircraft on specific taxiways have right of way: ";
						if(strcmp(strText,"Yes") == 0)
						{
							m_pConflictResolution->SetOnSpecificTaxiways(Yes);
							strOnSpecificTaxiways += "Yes";
						}
						else
						{
							m_pConflictResolution->SetOnSpecificTaxiways(No);
							strOnSpecificTaxiways += "No";
							COnSpecificTaxiwaysList* pOnSpecificTaxiwaysList = m_pConflictResolution->GetOnSpecificTaxiwaysList();
							pOnSpecificTaxiwaysList->DeleteData();
							if (m_wndConflictResolutionTree.ItemHasChildren(m_hOnSpecificTaxiways))
							{
								HTREEITEM hNextItem;
								HTREEITEM hChildItem = m_wndConflictResolutionTree.GetChildItem(m_hOnSpecificTaxiways);

								while (hChildItem != NULL)
								{
									hNextItem = m_wndConflictResolutionTree.GetNextItem(hChildItem, TVGN_NEXT);
									m_wndConflictResolutionTree.DeleteItem(hChildItem);
									hChildItem = hNextItem;
								}
							}
						}
						m_wndConflictResolutionTree.SetItemText(m_hOnSpecificTaxiways,strOnSpecificTaxiways);
					}
				}
			}
			break;
		case UM_CEW_COMBOBOX_SETWIDTH:
			{
				m_wndConflictResolutionTree.SetWidth(80);
			}
			break;

		default:
			break;
	}

	return CXTResizeDialog::DefWindowProc(message, wParam, lParam);
}

void CDlgConflictResolution::OnTvnSelchangedTreeConflictresolution(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	UpdateToolBar();
	*pResult = 0;
}

void CDlgConflictResolution::addRunwayCrossBufferToTree(HTREEITEM hParent, RunwayCrossBuffer* buffer)
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	

	CString sfltType;
	buffer->getFlightType().screenPrint(sfltType);
	CString str; 
	str.Format(_T("Flight Type: %s"), sfltType.GetString() );
	HTREEITEM hItemFlight = m_wndConflictResolutionTree.InsertItem(str, cni, FALSE,0, hParent);


	cni.net =  NET_EDITSPIN_WITH_VALUE;
	str.Format(_T("Buffer Time(secs): %d"), buffer->getTime().asSeconds() );
	HTREEITEM hItem = m_wndConflictResolutionTree.InsertItem(str, cni, FALSE,0, hItemFlight);
	m_hBufferTimes.push_back(hItem);
	m_wndConflictResolutionTree.SetItemData(hItem,buffer->getTime().asSeconds());

	int nNM = (int)(buffer->getDistanceNM()+0.5);
	str.Format(_T("Buffer Distance(nm): %d"), nNM );
	hItem = m_wndConflictResolutionTree.InsertItem(str, cni, FALSE,0, hItemFlight);
	m_hBufferDists.push_back(hItem);
	m_wndConflictResolutionTree.SetItemData(hItem,nNM);

	m_wndConflictResolutionTree.SetItemData(hItemFlight, (DWORD)buffer);
	m_wndConflictResolutionTree.Expand(hItemFlight, TVE_EXPAND);
	m_wndConflictResolutionTree.Expand(hParent, TVE_EXPAND);
}



void CDlgConflictResolution::LoadTree()
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	m_hBufferDists.clear();
	m_hBufferTimes.clear();

	//radius concern
	int nRadiusOfConcern = m_pConflictResolution->GetRadiusOfConcern();
	CString strRadiusOfConcern;
	strRadiusOfConcern.Format(_T("Radius of concern(ft):%d"), nRadiusOfConcern);
	cni.nt=NT_NORMAL; 
	cni.net=NET_EDITSPIN_WITH_VALUE;
	cni.fMaxValue = 10000;
	m_hRadiusOfConcern = m_wndConflictResolutionTree.InsertItem(strRadiusOfConcern,cni,FALSE);
	m_wndConflictResolutionTree.SetItemData(m_hRadiusOfConcern,nRadiusOfConcern);
	
	//runway cross buffer (seconds)
	int nRunwayCrossBuffer = m_pConflictResolution->GetRunwayCrossBuffer();
	CString strRunwayCrossBuffer;
	strRunwayCrossBuffer.Format(_T("Runway Cross Buffer(secs): %d"),nRunwayCrossBuffer);
	cni.nt=NT_NORMAL; 
	cni.net=NET_EDITSPIN_WITH_VALUE;
	cni.fMaxValue = 10000;
	m_hRunwayCrossBuffer = m_wndConflictResolutionTree.InsertItem(strRunwayCrossBuffer,cni,FALSE);
	m_wndConflictResolutionTree.SetItemData(m_hRunwayCrossBuffer,nRunwayCrossBuffer);
	
	CString strCaseTakeoff = _T("Case Takeoff:");
	cni.net =  NET_NORMAL;
	m_hCaseTakeoff = m_wndConflictResolutionTree.InsertItem(strCaseTakeoff, cni, FALSE,0,m_hRunwayCrossBuffer);
	for(size_t inserite=0;inserite<m_pConflictResolution->m_vCrossBuffers.GetElementCount(); inserite++)
	{
		RunwayCrossBuffer* buffer = m_pConflictResolution->m_vCrossBuffers.GetItem(inserite);
		if(buffer->getCase()==RunwayCrossBuffer::_Takeoff)
		{
			addRunwayCrossBufferToTree(m_hCaseTakeoff, buffer);
		}
	}
	m_wndConflictResolutionTree.Expand(m_hCaseTakeoff,TVE_EXPAND);

	//add Cross buffers 
	CString strCaseLanding = _T("Case Landing:");
	m_hCaseLanding = m_wndConflictResolutionTree.InsertItem(strCaseLanding, cni, FALSE, 0,m_hRunwayCrossBuffer);
	for(size_t inserite=0;inserite<m_pConflictResolution->m_vCrossBuffers.GetElementCount(); inserite++)
	{
		RunwayCrossBuffer* buffer = m_pConflictResolution->m_vCrossBuffers.GetItem(inserite);
		if(buffer->getCase()==RunwayCrossBuffer::_Landing)
		{
			addRunwayCrossBufferToTree(m_hCaseLanding, buffer);
		}
	}
	m_wndConflictResolutionTree.Expand(m_hCaseLanding,TVE_EXPAND);

	m_wndConflictResolutionTree.Expand(m_hRunwayCrossBuffer,TVE_EXPAND);


	CString strRunwayusTaxiway = _T("Runway use as Taxiway Criteria");
	HTREEITEM hItemRuwnayasTaxiway = m_wndConflictResolutionTree.InsertItem(strRunwayusTaxiway, cni,FALSE);
	{
		CString strAppraoch  = _T("No Aircraft on approach threshold within ");
		HTREEITEM hApproach = m_wndConflictResolutionTree.InsertItem(strAppraoch, cni, FALSE, 0 ,hItemRuwnayasTaxiway);
		{
			cni.net = NET_EDITSPIN_WITH_VALUE;
			CString strNM;
			int nm = (int)(m_pConflictResolution->getRunwayAsTaxiwayApproachDistNM()+0.5);
			strNM.Format(_T("Distance(nm): %d"), nm );
			m_hApproachDistance = m_wndConflictResolutionTree.InsertItem(strNM, cni,FALSE,0, hApproach);
			m_wndConflictResolutionTree.SetItemData(m_hApproachDistance,nm);
			CString strTime;
			int nTime = m_pConflictResolution->getRunwayAsTaxiwayApproachTime().asSeconds();
			strTime.Format(_T("Time(secs): %d"), nTime);
			m_hApproachTime = m_wndConflictResolutionTree.InsertItem(strTime, cni, FALSE, 0, hApproach);
			m_wndConflictResolutionTree.SetItemData(m_hApproachTime,nTime);
			m_wndConflictResolutionTree.Expand(hApproach,TVE_EXPAND);

			//
			CString sTakeoff;
			nTime = m_pConflictResolution->getRunwayAsTaxiwayTakeoffTime().asSeconds();
			sTakeoff.Format(_T("No takeoff scheduled in the next(secs): %d"),  nTime);
			m_hTakeoffSeconds = m_wndConflictResolutionTree.InsertItem(sTakeoff, cni, FALSE, 0 , hItemRuwnayasTaxiway);
			m_wndConflictResolutionTree.SetItemData(m_hTakeoffSeconds,nTime);
		}
	}
	m_wndConflictResolutionTree.Expand(hItemRuwnayasTaxiway,TVE_EXPAND);

	
	//At intersection Aircraft on right has right of way
	CString strAtIntersectionOnRight;
	strAtIntersectionOnRight = "At intersection Aircraft on right has right of way: ";
	if(m_pConflictResolution->GetAtIntersectionOnRight() == Yes)
		strAtIntersectionOnRight += "Yes";
	else
		strAtIntersectionOnRight += "No";
	cni.nt=NT_NORMAL;
	cni.net=NET_COMBOBOX;
	cni.bAutoSetItemText = false;
	m_hAtIntersectionOnRight = m_wndConflictResolutionTree.InsertItem(strAtIntersectionOnRight,cni,FALSE);

	if(m_pConflictResolution->GetAtIntersectionOnRight() == No)
	{
		CAtIntersectionOnRightList* pAtIntersectionOnRightList = m_pConflictResolution->GetAtIntersectionOnRightList();
		size_t nAtIntersectionOnRightCount = pAtIntersectionOnRightList->GetElementCount();

		CString strTempValue;
		for (size_t i = 0; i < nAtIntersectionOnRightCount; i++)
		{
			CAtIntersectionOnRight* pAtIntersectionOnRight = pAtIntersectionOnRightList->GetItem(i);
			strTempValue = _T("");
			//CString strFltTypeA = pAtIntersectionOnRight->GetFltTypeA();
			//CString strFltTypeB = pAtIntersectionOnRight->GetFltTypeB();
			FlightConstraint& fltTypeA = pAtIntersectionOnRight->GetFltTypeA();
			FlightConstraint& fltTypeB = pAtIntersectionOnRight->GetFltTypeB();
			CString strFltTypeA;
			fltTypeA.screenPrint(strFltTypeA);
			CString strFltTypeB;
			fltTypeB.screenPrint(strFltTypeB);
			strTempValue.Format(_T("%s gives way to %s"),strFltTypeA,strFltTypeB);
			CCoolTree::InitNodeInfo(this,cni);
			HTREEITEM hAtIntersectionOnRightItem = m_wndConflictResolutionTree.InsertItem(strTempValue,cni,FALSE,FALSE, m_hAtIntersectionOnRight);
			m_wndConflictResolutionTree.SetItemData(hAtIntersectionOnRightItem,(DWORD_PTR)pAtIntersectionOnRight);
			strTempValue.Format(_T("Flight Type1: %s"),strFltTypeA);
			m_wndConflictResolutionTree.InsertItem(strTempValue,cni,FALSE,FALSE, hAtIntersectionOnRightItem);
			strTempValue.Format(_T("Flight Type2: %s"),strFltTypeB);
			m_wndConflictResolutionTree.InsertItem(strTempValue,cni,FALSE,FALSE, hAtIntersectionOnRightItem);
			m_wndConflictResolutionTree.Expand(hAtIntersectionOnRightItem,TVE_EXPAND);
		}
	m_wndConflictResolutionTree.Expand(m_hAtIntersectionOnRight,TVE_EXPAND);
	}

	
	//Aircraft first in a taxiway has right of way
	CString strFirstInATaxiway;
	strFirstInATaxiway = "Aircraft first in a taxiway has right of way: ";
	if(m_pConflictResolution->GetFirstInATaxiway() == Yes)
		 strFirstInATaxiway += "Yes";
	else
		 strFirstInATaxiway += "No";
	cni.nt=NT_NORMAL;
	cni.net=NET_COMBOBOX;
	cni.bAutoSetItemText = false;
	m_hFirstInATaxiway = m_wndConflictResolutionTree.InsertItem(strFirstInATaxiway,cni,FALSE);
	if(m_pConflictResolution->GetFirstInATaxiway() == No)
	{
		CFirstInATaxiwayList* pFirstInATaxiwayList = m_pConflictResolution->GetFirstInATaxiwayList();
		size_t nFirstInATaxiwayCount = pFirstInATaxiwayList->GetElementCount();

		CString strTempValue;
		for (size_t i = 0; i < nFirstInATaxiwayCount; i++)
		{
			CFirstInATaxiway* pFirstInATaxiway = pFirstInATaxiwayList->GetItem(i);
			strTempValue = _T("");
			//CString strFltTypeA = pFirstInATaxiway->GetFltTypeA();
			//CString strFltTypeB = pFirstInATaxiway->GetFltTypeB();
			FlightConstraint& fltTypeA = pFirstInATaxiway->GetFltTypeA();
			FlightConstraint& fltTypeB = pFirstInATaxiway->GetFltTypeB();
			CString strFltTypeA;
			fltTypeA.screenPrint(strFltTypeA);
			CString strFltTypeB;
			fltTypeB.screenPrint(strFltTypeB);
			strTempValue.Format(_T("%s gives way to %s"),strFltTypeA,strFltTypeB);
			CCoolTree::InitNodeInfo(this,cni);
			HTREEITEM hFirstInATaxiwayItem = m_wndConflictResolutionTree.InsertItem(strTempValue,cni,FALSE,FALSE, m_hFirstInATaxiway);
			m_wndConflictResolutionTree.SetItemData(hFirstInATaxiwayItem,(DWORD_PTR)pFirstInATaxiway);
			strTempValue.Format(_T("Flight Type1: %s"),strFltTypeA);
			m_wndConflictResolutionTree.InsertItem(strTempValue,cni,FALSE,FALSE, hFirstInATaxiwayItem);
			strTempValue.Format(_T("Flight Type2: %s"),strFltTypeB);
			m_wndConflictResolutionTree.InsertItem(strTempValue,cni,FALSE,FALSE, hFirstInATaxiwayItem);
			m_wndConflictResolutionTree.Expand(hFirstInATaxiwayItem,TVE_EXPAND);
		}
		m_wndConflictResolutionTree.Expand(m_hFirstInATaxiway,TVE_EXPAND);
	}

	
	
	
	//Aircraft on specific taxiways have right of way 
	CString strOnSpecificTaxiways;
	strOnSpecificTaxiways = "Aircraft on specific taxiways have right of way: ";
	if(m_pConflictResolution->GetOnSpecificTaxiways() == Yes)
		strOnSpecificTaxiways += "Yes";
	else
		strOnSpecificTaxiways += "No";
	cni.nt=NT_NORMAL;
	cni.net=NET_COMBOBOX;
	cni.bAutoSetItemText = false;
	m_hOnSpecificTaxiways = m_wndConflictResolutionTree.InsertItem(strOnSpecificTaxiways,cni,FALSE);
	if(m_pConflictResolution->GetOnSpecificTaxiways() == Yes)
	{
		COnSpecificTaxiwaysList* pOnSpecificTaxiwaysList = m_pConflictResolution->GetOnSpecificTaxiwaysList();
		size_t nOnSpecificTaxiwaysCount = pOnSpecificTaxiwaysList->GetElementCount();

		CString strTempValue;
		for (size_t i = 0; i < nOnSpecificTaxiwaysCount; i++)
		{
			COnSpecificTaxiways* pOnSpecificTaxiways = pOnSpecificTaxiwaysList->GetItem(i);
			strTempValue = _T("");
			//CString strFltType = pOnSpecificTaxiways->GetFltType();
			FlightConstraint& fltType = pOnSpecificTaxiways->GetFltType();
			CString strFltType;
			fltType.screenPrint(strFltType);

			int nTaxiwayID = pOnSpecificTaxiways->GetTaxiwayID();
			CString strTaxiwayName;
			int nFlag = 0;
			TaxiwayVectorMapIter iter = m_TaxiwayVectorMap.begin();
			for(; iter != m_TaxiwayVectorMap.end(); iter++)
			{
				TaxiwayVector& vec = iter->second;
				for(TaxiwayVectorIter it = vec.begin();
					it != vec.end(); it++)
				{
					if(it->second == nTaxiwayID)
					{
						strTaxiwayName = it->first;
						nFlag = 1;
						break;
					}
				}
				if(nFlag == 1)
					break;
			}
			strTempValue.Format(_T("%s on Taxiway %s has right of way"),strFltType,strTaxiwayName);
			CCoolTree::InitNodeInfo(this,cni);
			HTREEITEM hOnSpecificTaxiwaysItem = m_wndConflictResolutionTree.InsertItem(strTempValue,cni,FALSE,FALSE, m_hOnSpecificTaxiways);
			m_wndConflictResolutionTree.SetItemData(hOnSpecificTaxiwaysItem,(DWORD_PTR)pOnSpecificTaxiways);
			strTempValue.Format(_T("Flight Type: %s"),strFltType);
			m_wndConflictResolutionTree.InsertItem(strTempValue,cni,FALSE,FALSE, hOnSpecificTaxiwaysItem);
			strTempValue.Format(_T("Taxiway: %s"),strTaxiwayName);
			m_wndConflictResolutionTree.InsertItem(strTempValue,cni,FALSE,FALSE, hOnSpecificTaxiwaysItem);
			m_wndConflictResolutionTree.Expand(hOnSpecificTaxiwaysItem,TVE_EXPAND);
		}
		m_wndConflictResolutionTree.Expand(m_hOnSpecificTaxiways,TVE_EXPAND);
	}
}

void CDlgConflictResolution::OnNewItem()
{
	if (m_pSelectFlightType == NULL)
		return;
	HTREEITEM hSelItem = m_wndConflictResolutionTree.GetSelectedItem();
	if(hSelItem == m_hAtIntersectionOnRight)
	{
		//FlightConstraint fltTypeA = (*m_pSelectFlightType)(NULL);
		//CString strFltTypeA;
		//fltTypeA.screenPrint(strFltTypeA);

		//FlightConstraint fltTypeB = (*m_pSelectFlightType)(NULL);
		//CString strFltTypeB;
		//fltTypeB.screenPrint(strFltTypeB);
		
		CAtIntersectionOnRight* pNewItem = new CAtIntersectionOnRight;
		//pNewItem->SetFltTypeA(strFltTypeA);
		//pNewItem->SetFltTypeB(strFltTypeB);
		CAtIntersectionOnRightList* pAtIntersectionOnRightList = m_pConflictResolution->GetAtIntersectionOnRightList();
		pAtIntersectionOnRightList->AddNewItem(pNewItem);
		CString strTempValue;
		strTempValue = _T("");
		strTempValue.Format(_T("%s gives way to %s"),"DEFAULT","DEFAULT");
		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this,cni);
		HTREEITEM hAtIntersectionOnRightItem = m_wndConflictResolutionTree.InsertItem(strTempValue,cni,FALSE,FALSE, m_hAtIntersectionOnRight);
		m_wndConflictResolutionTree.SetItemData(hAtIntersectionOnRightItem,(DWORD_PTR)pNewItem);
		strTempValue.Format(_T("Flight Type1: %s"),"DEFAULT");
		m_wndConflictResolutionTree.InsertItem(strTempValue,cni,FALSE,FALSE, hAtIntersectionOnRightItem);
		strTempValue.Format(_T("Flight Type2: %s"),"DEFAULT");
		m_wndConflictResolutionTree.InsertItem(strTempValue,cni,FALSE,FALSE, hAtIntersectionOnRightItem);
		m_wndConflictResolutionTree.Expand(hAtIntersectionOnRightItem,TVE_EXPAND);
		m_wndConflictResolutionTree.Expand(m_hAtIntersectionOnRight,TVE_EXPAND);
	}
	else if(hSelItem == m_hFirstInATaxiway)
	{ 
		//FlightConstraint fltTypeA = (*m_pSelectFlightType)(NULL);
		//CString strFltTypeA;
		//fltTypeA.screenPrint(strFltTypeA);

		//FlightConstraint fltTypeB = (*m_pSelectFlightType)(NULL);
		//CString strFltTypeB;
		//fltTypeB.screenPrint(strFltTypeB);

		CFirstInATaxiway* pNewItem = new CFirstInATaxiway;
		//pNewItem->SetFltTypeA(strFltTypeA);
		//pNewItem->SetFltTypeB(strFltTypeB);
		CFirstInATaxiwayList* pFirstInATaxiwayList = m_pConflictResolution->GetFirstInATaxiwayList();
		pFirstInATaxiwayList->AddNewItem(pNewItem);
		CString strTempValue;
		strTempValue = _T("");
		strTempValue.Format(_T("%s gives way to %s"),"DEFAULT","DEFAULT");
		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this,cni);
		HTREEITEM hFirstInATaxiwayItem = m_wndConflictResolutionTree.InsertItem(strTempValue,cni,FALSE,FALSE, m_hFirstInATaxiway);
		m_wndConflictResolutionTree.SetItemData(hFirstInATaxiwayItem,(DWORD_PTR)pNewItem);
		strTempValue.Format(_T("Flight Type1: %s"),"DEFAULT");
		m_wndConflictResolutionTree.InsertItem(strTempValue,cni,FALSE,FALSE, hFirstInATaxiwayItem);
		strTempValue.Format(_T("Flight Type2: %s"),"DEFAULT");
		m_wndConflictResolutionTree.InsertItem(strTempValue,cni,FALSE,FALSE, hFirstInATaxiwayItem);
		m_wndConflictResolutionTree.Expand(hFirstInATaxiwayItem,TVE_EXPAND);
		m_wndConflictResolutionTree.Expand(m_hFirstInATaxiway,TVE_EXPAND);
	}
	else if(hSelItem == m_hOnSpecificTaxiways)
	{
		//FlightConstraint fltType = (*m_pSelectFlightType)(NULL);
		//CString strFltType;
		//fltType.screenPrint(strFltType);

		//CDlgConflictSelectTaxiway dlg(m_pTaxiwayVectorMap);
		//if(IDOK != dlg.DoModal())
		//	return;

		//int nTaxiwayID = dlg.GetTaxiwayID();
		//CString strTaxiwayName = dlg.GetTaxiwayName();

		COnSpecificTaxiways* pNewItem = new COnSpecificTaxiways;
		//pNewItem->SetFltType(strFltType);
		//pNewItem->SetTaxiwayID(nTaxiwayID);
		COnSpecificTaxiwaysList* pOnSpecificTaxiwaysList = m_pConflictResolution->GetOnSpecificTaxiwaysList();
		pOnSpecificTaxiwaysList->AddNewItem(pNewItem);
		CString strTempValue;
		strTempValue = _T("");
		strTempValue.Format(_T("%s on Taxiway %s has right of way"),"DEFAULT","DEFAULT");
		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this,cni);
		HTREEITEM hOnSpecificTaxiwaysItem = m_wndConflictResolutionTree.InsertItem(strTempValue,cni,FALSE,FALSE, m_hOnSpecificTaxiways);
		m_wndConflictResolutionTree.SetItemData(hOnSpecificTaxiwaysItem,(DWORD_PTR)pNewItem);
		strTempValue.Format(_T("Flight Type: %s"),"DEFAULT");
		m_wndConflictResolutionTree.InsertItem(strTempValue,cni,FALSE,FALSE, hOnSpecificTaxiwaysItem);
		strTempValue.Format(_T("Taxiway: %s"),m_strDefaultTaxiwayName);
		pNewItem->SetTaxiwayID(m_nDefaultTaxiwayID);
		m_wndConflictResolutionTree.InsertItem(strTempValue,cni,FALSE,FALSE, hOnSpecificTaxiwaysItem);
		m_wndConflictResolutionTree.Expand(hOnSpecificTaxiwaysItem,TVE_EXPAND);
		m_wndConflictResolutionTree.Expand(m_hOnSpecificTaxiways,TVE_EXPAND);
	}
	if(m_hCaseLanding == hSelItem)
	{
		FlightConstraint fltType;
		if( (*m_pSelectFlightType)(NULL, fltType))
		{
			RunwayCrossBuffer* newBuffer = new RunwayCrossBuffer;
			newBuffer->setCase(RunwayCrossBuffer::_Landing);
			newBuffer->setFlightType(fltType);
			m_pConflictResolution->m_vCrossBuffers.AddNewItem(newBuffer);
			addRunwayCrossBufferToTree(m_hCaseLanding, newBuffer);
		}
	}
	if(m_hCaseTakeoff == hSelItem)
	{
		FlightConstraint fltType;
		if( (*m_pSelectFlightType)(NULL, fltType))
		{
			RunwayCrossBuffer* newBuffer = new RunwayCrossBuffer;
			newBuffer->setCase(RunwayCrossBuffer::_Takeoff);
			newBuffer->setFlightType(fltType);
			m_pConflictResolution->m_vCrossBuffers.AddNewItem(newBuffer);
			addRunwayCrossBufferToTree(m_hCaseTakeoff, newBuffer);
		}
	}
	
}
void CDlgConflictResolution::OnDelItem()
{
	HTREEITEM hSelItem = m_wndConflictResolutionTree.GetSelectedItem();
	HTREEITEM hParentItem = m_wndConflictResolutionTree.GetParentItem(hSelItem);
	if(hParentItem == m_hAtIntersectionOnRight)
	{
		CAtIntersectionOnRight* pSelItem = (CAtIntersectionOnRight*)m_wndConflictResolutionTree.GetItemData(hSelItem);
		CAtIntersectionOnRightList* pAtIntersectionOnRightList = (CAtIntersectionOnRightList*)(m_pConflictResolution->GetAtIntersectionOnRightList());
		pAtIntersectionOnRightList->DeleteItem(pSelItem);
	}
	else if(hParentItem == m_hFirstInATaxiway)
	{
		CFirstInATaxiway* pSelItem = (CFirstInATaxiway*)m_wndConflictResolutionTree.GetItemData(hSelItem);
		CFirstInATaxiwayList* pFirstInATaxiwayList = (CFirstInATaxiwayList*)(m_pConflictResolution->GetFirstInATaxiwayList());
		pFirstInATaxiwayList->DeleteItem(pSelItem);
	}
	else if(hParentItem == m_hOnSpecificTaxiways)
	{
		COnSpecificTaxiways* pSelItem = (COnSpecificTaxiways*)m_wndConflictResolutionTree.GetItemData(hSelItem);
		COnSpecificTaxiwaysList* pOnSpecificTaxiwaysList = (COnSpecificTaxiwaysList*)m_pConflictResolution->GetOnSpecificTaxiwaysList();
		pOnSpecificTaxiwaysList->DeleteItem(pSelItem);
	}
	else if (hParentItem == m_hCaseLanding || hParentItem == m_hCaseTakeoff)
	{
		RunwayCrossBuffer* buffer = (RunwayCrossBuffer*)m_wndConflictResolutionTree.GetItemData(hSelItem);
		m_pConflictResolution->m_vCrossBuffers.DeleteItem(buffer);
	}
	m_wndConflictResolutionTree.DeleteItem(hSelItem);
}
void CDlgConflictResolution::OnEditItem()
{
	if (m_pSelectFlightType == NULL)
		return;
	HTREEITEM hSelItem = m_wndConflictResolutionTree.GetSelectedItem();
	HTREEITEM hParentItem = m_wndConflictResolutionTree.GetParentItem(hSelItem);
//	HTREEITEM hNextVisibleItem = m_wndConflictResolutionTree.GetNextVisibleItem( hSelItem );
	CString strSelItem = m_wndConflictResolutionTree.GetItemText(hSelItem);
	int npos = strSelItem.Find(":");
	strSelItem = strSelItem.Left(npos);
	strSelItem = strSelItem.Right(1);
	if(m_wndConflictResolutionTree.GetParentItem(hParentItem) == m_hAtIntersectionOnRight)
	{
		CAtIntersectionOnRight* pSelItem = (CAtIntersectionOnRight*)m_wndConflictResolutionTree.GetItemData(hParentItem);
		CString strTempValue;
		strTempValue = _T("");
		CString strFltTypeA,strFltTypeB;

		if(strSelItem == _T("1"))
		{
			FlightConstraint fltTypeA;
			(*m_pSelectFlightType)(NULL, fltTypeA);
			fltTypeA.screenPrint(strFltTypeA);
			pSelItem->SetFltTypeA(fltTypeA);
			strTempValue.Format(_T("Flight Type1: %s"),strFltTypeA);
			(pSelItem->GetFltTypeB()).screenPrint(strFltTypeB);
		}
		else
		{
			FlightConstraint fltTypeB ;
			(*m_pSelectFlightType)(NULL, fltTypeB);
			fltTypeB.screenPrint(strFltTypeB);
			pSelItem->SetFltTypeB(fltTypeB);
			strTempValue.Format(_T("Flight Type2: %s"),strFltTypeB);
			(pSelItem->GetFltTypeA()).screenPrint(strFltTypeA);
		}	
		m_wndConflictResolutionTree.SetItemText(hSelItem,strTempValue);			
		strTempValue.Format(_T("%s gives way to %s"),strFltTypeA,strFltTypeB);
		m_wndConflictResolutionTree.SetItemText(hParentItem,strTempValue);
	}
	else if(m_wndConflictResolutionTree.GetParentItem(hParentItem) == m_hFirstInATaxiway)
	{
		CFirstInATaxiway* pSelItem = (CFirstInATaxiway*)m_wndConflictResolutionTree.GetItemData(hParentItem);
		CString strTempValue;
		strTempValue = _T("");
		CString strFltTypeA,strFltTypeB;
		if(strSelItem == _T("1"))
		{
			FlightConstraint fltTypeA;
			(*m_pSelectFlightType)(NULL, fltTypeA);
			fltTypeA.screenPrint(strFltTypeA);
			pSelItem->SetFltTypeA(fltTypeA);
			strTempValue.Format(_T("Flight Type1: %s"),strFltTypeA);
			(pSelItem->GetFltTypeB()).screenPrint(strFltTypeB);
		}
		else
		{
			FlightConstraint fltTypeB;// = (*m_pSelectFlightType)(NULL);
			(*m_pSelectFlightType)(NULL, fltTypeB);
			pSelItem->SetFltTypeB(fltTypeB);
			strTempValue.Format(_T("Flight Type2: %s"),strFltTypeB);
			(pSelItem->GetFltTypeA()).screenPrint(strFltTypeA);
		}
		m_wndConflictResolutionTree.SetItemText(hSelItem,strTempValue);		
		strTempValue.Format(_T("%s gives way to %s"),strFltTypeA,strFltTypeB);
		m_wndConflictResolutionTree.SetItemText(hParentItem,strTempValue);
	}
	else if(m_wndConflictResolutionTree.GetParentItem(hParentItem) == m_hOnSpecificTaxiways)
	{		
		COnSpecificTaxiways* pSelItem = (COnSpecificTaxiways*)m_wndConflictResolutionTree.GetItemData(hParentItem);
		CString strTempValue;
		strTempValue = _T("");
		CString strFltType,strTaxiwayName;
		if(strSelItem == _T("e"))
		{
			FlightConstraint fltType;
			(*m_pSelectFlightType)(NULL,fltType);
			fltType.screenPrint(strFltType);
			pSelItem->SetFltType(fltType);
			strTempValue.Format(_T("Flight Type: %s"),strFltType);
			int nTaxiwayID = pSelItem->GetTaxiwayID();
			int nFlag = 0;
			TaxiwayVectorMapIter iter = m_TaxiwayVectorMap.begin();
			for(; iter != m_TaxiwayVectorMap.end(); iter++)
			{
				TaxiwayVector& vec = iter->second;
				for(TaxiwayVectorIter it = vec.begin();
					it != vec.end(); it++)
				{
					if(it->second == nTaxiwayID)
					{
						strTaxiwayName = it->first;
						nFlag = 1;
						break;
					}
				}
				if(nFlag == 1)
					break;
			}
		}
		else
		{
			CDlgConflictSelectTaxiway dlg(m_pTaxiwayVectorMap);
			if(IDOK != dlg.DoModal())
				return;
			int nTaxiwayID = dlg.GetTaxiwayID();
			strTaxiwayName = dlg.GetTaxiwayName();
			pSelItem->SetTaxiwayID(nTaxiwayID);
			strTempValue.Format(_T("Taxiway: %s"),strTaxiwayName);
			(pSelItem->GetFltType()).screenPrint(strFltType);
		}
		m_wndConflictResolutionTree.SetItemText(hSelItem,strTempValue);		
		strTempValue.Format(_T("%s on Taxiway %s has right of way"),strFltType,strTaxiwayName);
		m_wndConflictResolutionTree.SetItemText(hParentItem,strTempValue);
	}
	
	if(hParentItem == m_hCaseLanding || hParentItem == m_hCaseTakeoff) //edit flight type
	{
		RunwayCrossBuffer* buffer  = (RunwayCrossBuffer*)m_wndConflictResolutionTree.GetItemData(hSelItem);
		FlightConstraint fltType;
		if( (*m_pSelectFlightType)(NULL, fltType) )
		{
			buffer->setFlightType(fltType);
			CString sFltType;
			fltType.screenPrint(sFltType);
			CString str; 
			str.Format(_T("Flight Type: %s"), sFltType.GetString() );;
			m_wndConflictResolutionTree.SetItemText(hSelItem, str);
		}
	}

}
