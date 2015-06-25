// DlgRunwayExitSelect.cpp : implementation file
//

#include "stdafx.h"
// #include "AirsideGUI.h"
#include "DlgParkingStandSelect.h"
#include "../InputAirside/ALTAirport.h"
#include "InputAirside/InputAirside.h"
#include "InputAirside\ALTObjectGroup.h"


// CDlgParkingStandSelect

IMPLEMENT_DYNAMIC(CDlgParkingStandSelect, CXTResizeDialog)
CDlgParkingStandSelect::CDlgParkingStandSelect(const UINT nID,int nProjID,const CString& strSelStands, CWnd* pParent)
:CXTResizeDialog(CDlgParkingStandSelect::IDD, pParent)
, m_nProjID(nProjID)
, m_bAllStand(FALSE)
{
	m_strStandGroups = strSelStands + ",";
}

CDlgParkingStandSelect::~CDlgParkingStandSelect()
{
}

BEGIN_MESSAGE_MAP(CDlgParkingStandSelect, CXTResizeDialog)
	ON_WM_SIZE()
//  	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_SELECT, OnTvnSelchangedTreeRunwayExit)
END_MESSAGE_MAP()
void CDlgParkingStandSelect::OnSize(UINT nType, int cx, int cy)
{

}
void CDlgParkingStandSelect::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_TREE_SELECT,m_TreeCtrlStandFamily);
}

BOOL CDlgParkingStandSelect::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();
	SetDlgItemText(IDC_STATIC,_T("Parking Stand"));
	SetWindowText("Select Parking Stand");
//	m_TreeCtrlStandFamily.EnableMultiSelect();
	SetStandFamilyTreeContent();
	return true;
}
int CDlgParkingStandSelect::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}
CString CDlgParkingStandSelect::GetStandFamilyName()
{
	return m_strStandGroups;
}
std::vector<int>& CDlgParkingStandSelect::GetStandGroupList()
{
	return m_vStandSelID;
}
BOOL CDlgParkingStandSelect::IsAllStand()
{
	return m_bAllStand;
}
void CDlgParkingStandSelect::SetStandFamilyTreeContent(void)
{
	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjID, vAirportIds);
	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	{
		ALTAirport airport;
		airport.ReadAirport(*iterAirportID);
		airport.getName();
		HTREEITEM hAirport = m_TreeCtrlStandFamily.InsertItem(airport.getName());
		m_TreeCtrlStandFamily.SetItemData(hAirport, *iterAirportID);

		HTREEITEM hAll = m_TreeCtrlStandFamily.InsertItem("All", hAirport);
		m_TreeCtrlStandFamily.SetItemData(hAll, -2);

		ALTObjectList vStands;
		ALTAirport::GetStandList(*iterAirportID,vStands);

		for(int i = 0;i< (int)vStands.size(); ++i)
		{
			Stand * pStand =(Stand*) vStands.at(i).get();
			AddObjectToTree(hAirport,pStand);
		}

		m_TreeCtrlStandFamily.Expand(hAirport, TVE_EXPAND);
	}
}

void CDlgParkingStandSelect::AddObjectToTree(HTREEITEM hObjRoot,ALTObject* pObject)
{
	ASSERT(hObjRoot);

	ALTObjectID objName;
	pObject->getObjName(objName);
	HTREEITEM hParentItem = hObjRoot;
	HTREEITEM hTreeItem = NULL;
	bool bObjNode = false;

	for (int nLevel =0; nLevel< OBJECT_STRING_LEVEL; ++nLevel)
	{
		if (nLevel < OBJECT_STRING_LEVEL -1 )
		{
			HTREEITEM hItem = FindObjNode(hParentItem,objName.m_val[nLevel].c_str());
			if (hItem != NULL)
			{
				if (IsStandSelected(pObject->GetObjNameString(nLevel)))
					m_TreeCtrlStandFamily.SelectItem(hItem);

				hParentItem = hItem;

				continue;
			}

			if (objName.m_val[nLevel+1] != _T(""))
			{
				hTreeItem = m_TreeCtrlStandFamily.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
				//m_TreeCtrlStandFamily.Expand(hParentItem, TVE_EXPAND);
				if (IsStandSelected(pObject->GetObjNameString(nLevel)))
					m_TreeCtrlStandFamily.SelectItem(hTreeItem);

				hParentItem = hTreeItem;

				continue;
			}
			else //objNode
			{
				hTreeItem = m_TreeCtrlStandFamily.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
				//m_TreeCtrlStandFamily.Expand(hParentItem, TVE_EXPAND);
				break;
			}

		}
		else
		{
			hTreeItem = m_TreeCtrlStandFamily.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
			//m_TreeCtrlStandFamily.Expand(hParentItem, TVE_EXPAND);
			break;
		}
	}

	if (IsStandSelected(pObject->GetObjNameString()))
	{
		m_TreeCtrlStandFamily.SelectItem(hTreeItem);
		m_TreeCtrlStandFamily.Expand(hParentItem, TVE_EXPAND);
	}
}
HTREEITEM CDlgParkingStandSelect::FindObjNode(HTREEITEM hParentItem,const CString& strNodeText)
{
	HTREEITEM hRetItem = NULL;

	HTREEITEM hChildItem = m_TreeCtrlStandFamily.GetChildItem(hParentItem);

	//find
	while (hChildItem)
	{
		CString strItemText = m_TreeCtrlStandFamily.GetItemText(hChildItem);
		if(strItemText.CompareNoCase(strNodeText) == 0)
		{
			hRetItem = hChildItem;
			break;
		}
		hChildItem = m_TreeCtrlStandFamily.GetNextSiblingItem(hChildItem);
	}
	return hRetItem;
}

bool CDlgParkingStandSelect::IsStandSelected(const CString& strStandName)
{
	CString strName = strStandName + ",";
	if (m_strStandGroups.Find(strName) >= 0)
		return true;

	return false;
}
void CDlgParkingStandSelect::GetStandFamilyAltID(HTREEITEM hItem, ALTObjectID& objName, CString& strStandFamily)
{
	switch(GetCurStandGroupNamePos(hItem))
	{
	case 0:
		{
			objName.at(0) = m_TreeCtrlStandFamily.GetItemText(hItem);

			strStandFamily.Format("%s", objName.at(0).c_str());
		}
		break;

	case 1:
		{
			objName.at(1) = m_TreeCtrlStandFamily.GetItemText(hItem);
			hItem = m_TreeCtrlStandFamily.GetParentItem(hItem);
			objName.at(0) = m_TreeCtrlStandFamily.GetItemText(hItem);
			objName.at(2) = "";
			objName.at(3) = "";

			strStandFamily.Format("%s-%s", objName.at(0).c_str(), objName.at(1).c_str());
		}
		break;

	case 2:
		{
			objName.at(2) = m_TreeCtrlStandFamily.GetItemText(hItem);
			hItem = m_TreeCtrlStandFamily.GetParentItem(hItem);
			objName.at(1) = m_TreeCtrlStandFamily.GetItemText(hItem);
			hItem = m_TreeCtrlStandFamily.GetParentItem(hItem);
			objName.at(0) = m_TreeCtrlStandFamily.GetItemText(hItem);
			objName.at(3) = "";

			strStandFamily.Format("%s-%s-%s", objName.at(0).c_str(), objName.at(1).c_str(), objName.at(2).c_str());
		}
		break;

	case 3:
		{
			objName.at(3) = m_TreeCtrlStandFamily.GetItemText(hItem);
			hItem = m_TreeCtrlStandFamily.GetParentItem(hItem);
			objName.at(2) = m_TreeCtrlStandFamily.GetItemText(hItem);
			hItem = m_TreeCtrlStandFamily.GetParentItem(hItem);
			objName.at(1) = m_TreeCtrlStandFamily.GetItemText(hItem);
			hItem = m_TreeCtrlStandFamily.GetParentItem(hItem);
			objName.at(0) = m_TreeCtrlStandFamily.GetItemText(hItem);

			strStandFamily.Format("%s-%s-%s-%s", objName.at(0).c_str(), objName.at(1).c_str(), objName.at(2).c_str(), objName.at(3).c_str());
		}
		break;

	default:
		strStandFamily = "";
		break;
	}
}
int CDlgParkingStandSelect::GetCurStandGroupNamePos(HTREEITEM hTreeItem)
{
	if (NULL == hTreeItem)
	{
		return -1;
	}

	HTREEITEM hParentItem = m_TreeCtrlStandFamily.GetParentItem(hTreeItem);

	//root item
	if (NULL == hParentItem)
	{
		return -1;
	}

	hParentItem = m_TreeCtrlStandFamily.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 0;
	}

	hParentItem = m_TreeCtrlStandFamily.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 1;
	}

	hParentItem = m_TreeCtrlStandFamily.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 2;
	}

	hParentItem = m_TreeCtrlStandFamily.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 3;
	}

	return -1;
}

/*
void CDlgParkingStandSelect::OnTvnSelchangedTreeRunwayExit(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	HTREEITEM hItem = m_TreeCtrlStandFamily.GetSelectedItem();
	if(hItem != NULL)
	{
		int nId = (int)m_TreeCtrlStandFamily.GetItemData(hItem);
		if (nId == -2)
			m_bAllRunwayExit = TRUE;
	}

	UpdateData(FALSE);

	*pResult = 0;
}
CDlgParkingStandSelect::RUNWAYEXITTYPE CDlgParkingStandSelect::GetRunwayExitType(HTREEITEM hRunwayTreeItem)
{
	if (NULL == hRunwayTreeItem)
	{
		return RUNWAYEXIT_DEFAULT;
	}

	HTREEITEM hParent = m_TreeCtrlStandFamily.GetParentItem(hRunwayTreeItem);

	if (NULL == hParent)
	{
		return RUNWAYEXIT_DEFAULT;
	}
	else
	{
		HTREEITEM hParentParent = m_TreeCtrlStandFamily.GetParentItem(hParent);

		if (NULL == hParentParent)
		{
			return RUNWAYEXIT_ALLRUNWAY;
		}
		else
		{
			HTREEITEM hParentParentParent = m_TreeCtrlStandFamily.GetParentItem(hParentParent);

			if (NULL == hParentParentParent)
			{
				return RUNWAYEXIT_DEFAULT;
			}
			else
			{
				HTREEITEM hParentParentParentParent = m_TreeCtrlStandFamily.GetParentItem(hParentParentParent);

				if (NULL == hParentParentParentParent)
				{
					return RUNWAYEXIT_LOGICRUNWAY;
				}
				else
				{
					HTREEITEM hItem = m_TreeCtrlStandFamily.GetParentItem(hParentParentParentParent);
					if (NULL == hItem)
					{
						return RUNWAYEXIT_ITEM;
					}
				}

			}
		}
	}

	return RUNWAYEXIT_DEFAULT;
}

*/
void CDlgParkingStandSelect::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	m_strStandGroups = "";
	m_vStandSelID.clear();


	if(!m_bAllStand)
	{
//		int nSelCount = m_TreeCtrlStandFamily.GetSelectedCount();

		HTREEITEM hSelItem = m_TreeCtrlStandFamily.GetSelectedItem();

//		for (int i = 0; i < nSelCount;++i) 
		{		
			if (hSelItem != NULL )
			{	
				ALTObjectGroup altObjGroup;
				ALTObjectID objName;
				CString strName;

				GetStandFamilyAltID(hSelItem,objName,strName);

				altObjGroup.setType(ALT_STAND);
				altObjGroup.setName(objName);
				int nSelStandFamilyID = altObjGroup.InsertData(m_nProjID);

				m_vStandSelID.push_back(nSelStandFamilyID);

				if (m_strStandGroups =="")
				{
					m_strStandGroups = strName;
				}
				else
				{
					m_strStandGroups += ", ";
					m_strStandGroups += strName;
				}
			}
//			hSelItem = m_TreeCtrlStandFamily.GetNextSelectedItem(hSelItem);
		}
	}
	else
		m_strStandGroups = "All";

	if (!m_strStandGroups.Compare(""))
	{
		MessageBox("Please select stand family!", "Error", MB_OK|MB_ICONEXCLAMATION );
		return;
	}

	
	CXTResizeDialog::OnOK();
}




// CDlgParkingStandSelect message handlers


/*
bool CDlgParkingStandSelect::IsExitSelected( const CString& strExitName )
{
	if (m_strRunwayExits.Find(strExitName) >= 0)
		return true;

	return false;
}
void CDlgParkingStandSelect::GetRunwayExitNameString(RunwayExit *pRunwayExit, RunwayExitList *pRunwayExitList, CString& strRunwayExitName)
{
	ASSERT(pRunwayExit);
	ASSERT(pRunwayExitList);

	int nTaxiwayID = pRunwayExit->GetTaxiwayID();
	Taxiway taxiway;
	taxiway.ReadObject(nTaxiwayID);

	BOOL bDisplayLeftRight = FALSE;
	BOOL bDisplayIndex     = FALSE;

	RunwayExitList::iterator iter = pRunwayExitList->begin();
	for (; iter!=pRunwayExitList->end(); iter++)
	{
		RunwayExit *pOtherRunwayExit = &(*iter);
		ASSERT(pOtherRunwayExit);

		//skip self
		if (pOtherRunwayExit->GetID() == pRunwayExit->GetID())
		{
			continue;
		}

		//have more intersect node with a taxiway
		if (pOtherRunwayExit->GetTaxiwayID() == pRunwayExit->GetTaxiwayID())
		{
			bDisplayLeftRight = TRUE;

			if (pOtherRunwayExit->GetSideType() == pRunwayExit->GetSideType())
			{
				bDisplayIndex = TRUE;
			}
		}
	}

	CString strExitName;
	strExitName.Format(_T("%s"), pRunwayExit->GetName());

	if (bDisplayLeftRight)
	{
		if (bDisplayIndex)
		{
			CString strIndex;
			strIndex.Format(_T("%d"), pRunwayExit->GetIntesectionNode().GetIndex());
			strExitName += "&";
			strExitName += strIndex;
		}
	}

	strRunwayExitName.Format(_T("%s"), strExitName);
}

void CDlgParkingStandSelect::SetRunwayExitTreeContent(void)
{
	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjID, vAirportIds);
	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	{
		ALTAirport airport;
		airport.ReadAirport(*iterAirportID);

		HTREEITEM hAirport = m_TreeCtrlStandFamily.InsertItem(airport.getName());
		m_TreeCtrlStandFamily.SetItemData(hAirport, *iterAirportID);

		HTREEITEM hAll = m_TreeCtrlStandFamily.InsertItem("All",hAirport);
		m_TreeCtrlStandFamily.SetItemData(hAll,-2);

		std::vector<int> vRunways;
		ALTAirport::GetRunwaysIDs(*iterAirportID, vRunways);
		for (std::vector<int>::iterator iterRunwayID = vRunways.begin(); iterRunwayID != vRunways.end(); ++iterRunwayID)
		{
			Runway runway;
			runway.ReadObject(*iterRunwayID);

			RunwayExitList runwayExitList;
			int nRunwayExitCount = runway.GetExitList(RUNWAYMARK_FIRST, runwayExitList);

			//if the runway haven't exit
			if (runwayExitList.size() < 1)
			{
				continue;
			}

			HTREEITEM hRunwayItem = m_TreeCtrlStandFamily.InsertItem(runway.GetObjNameString(), hAll);
			m_TreeCtrlStandFamily.SetItemData(hRunwayItem, *iterRunwayID);

			//marking 1
			HTREEITEM hMarking1Item = m_TreeCtrlStandFamily.InsertItem(runway.GetMarking1().c_str(), hRunwayItem);
			m_TreeCtrlStandFamily.SetItemData(hMarking1Item, (int)RUNWAYMARK_FIRST);

			std::vector<RunwayExit>::iterator citer;
			CString runwayExitName;
			for (citer=runwayExitList.begin(); citer!=runwayExitList.end(); citer++)
			{
				RunwayExit runwayExit = *citer;
				//runwayExitName = runwayExit.GetName();
				GetRunwayExitNameString(&runwayExit, &runwayExitList, runwayExitName);

				HTREEITEM hPositionItem = m_TreeCtrlStandFamily.InsertItem(runwayExitName, hMarking1Item);
				m_TreeCtrlStandFamily.SetItemData(hPositionItem, (DWORD_PTR)runwayExit.GetID());

				if (IsExitSelected(runwayExitName))
					m_TreeCtrlStandFamily.SelectItem(hPositionItem);				
			}
			m_TreeCtrlStandFamily.Expand(hMarking1Item, TVE_EXPAND);

			//marking 2
			HTREEITEM hMarking2Item = m_TreeCtrlStandFamily.InsertItem(runway.GetMarking2().c_str(), hRunwayItem);
			m_TreeCtrlStandFamily.SetItemData(hMarking2Item, (int)RUNWAYMARK_SECOND);

			RunwayExitList runwayExitListMarkTwo;
			nRunwayExitCount = runway.GetExitList(RUNWAYMARK_SECOND, runwayExitListMarkTwo);
			for (citer=runwayExitListMarkTwo.begin(); citer!=runwayExitListMarkTwo.end(); citer++)
			{
				RunwayExit runwayExit = *citer;

				//runwayExitName = runwayExit.m_strName;	
				GetRunwayExitNameString(&runwayExit, &runwayExitListMarkTwo, runwayExitName);
				//runwayExitName = runwayExit.GetName();

				HTREEITEM hPositionItem = m_TreeCtrlStandFamily.InsertItem(runwayExitName, hMarking2Item);
				m_TreeCtrlStandFamily.SetItemData(hPositionItem, (DWORD_PTR)runwayExit.GetID());

				if (IsExitSelected(runwayExitName))
					m_TreeCtrlStandFamily.SelectItem(hPositionItem);
			}
			m_TreeCtrlStandFamily.Expand(hMarking2Item, TVE_EXPAND);
			m_TreeCtrlStandFamily.Expand(hRunwayItem, TVE_EXPAND);

		}

		m_TreeCtrlStandFamily.Expand(hAll, TVE_EXPAND);
		m_TreeCtrlStandFamily.Expand(hAirport, TVE_EXPAND);
	}
}
std::vector<int>& CDlgParkingStandSelect::GetRunwayExitList()
{
	return m_vExitSelID;
}
BOOL CDlgParkingStandSelect::IsAllRunwayExit()
{
	return m_bAllRunwayExit;
}
CString CDlgParkingStandSelect::GetRunwayExitName()
{
	return m_strRunwayExits;
}

*/
