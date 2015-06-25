// DlgWayPointStandFamily.cpp : implementation file
//

#include "stdafx.h"
//#include "AirsideGUI.h"
#include "resource.h"
#include "DlgWayPointStandFamily.h"
#include ".\dlgwaypointstandfamily.h"
#include "../InputAirside/ALTAirport.h"
#include "InputAirside\ALTObjectGroup.h"
#include "InputAirside/InputAirside.h"
#include "InputAirside/stand.h"
// CDlgWayPointStandFamily dialog

IMPLEMENT_DYNAMIC(CDlgWayPointStandFamily, CDialog)
CDlgWayPointStandFamily::CDlgWayPointStandFamily(int nProjID, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgWayPointStandFamily::IDD, pParent)
	,m_nProjID(nProjID)
{
}

CDlgWayPointStandFamily::~CDlgWayPointStandFamily()
{
}

void CDlgWayPointStandFamily::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_WAYPOINTANDSTANDFAMILY, m_TreeWayPointStandFamily);
}


BEGIN_MESSAGE_MAP(CDlgWayPointStandFamily, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_WAYPOINTANDSTANDFAMILY, OnTvnSelchangedTreeWaypointandstandfamily)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgWayPointStandFamily message handlers
void CDlgWayPointStandFamily::LoadTree()
{
	HTREEITEM hWayPoint = m_TreeWayPointStandFamily.InsertItem(_T("Way Point:"));

	//std::vector<int> vAirportIds;
	//InputAirside::GetAirportList(m_nProjID, vAirportIds);
	//for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	//{
	//	ALTAirport airport;
	//	airport.ReadAirport(*iterAirportID);
	//	airport.getName();
	//	HTREEITEM hAirport = m_TreeWayPointStandFamily.InsertItem(airport.getName(),hWayPoint);
	//	m_TreeWayPointStandFamily.SetItemData(hAirport, *iterAirportID);

		for (int i = 0; i < static_cast<int>(m_vWaypoint.size()); ++ i)
		{
			CString strWayPoint = m_vWaypoint[i].GetObjNameString();
			int nWayPointID = m_vWaypoint[i].getID();
			HTREEITEM hWayPointItem = m_TreeWayPointStandFamily.InsertItem(strWayPoint,hWayPoint);
			m_TreeWayPointStandFamily.SetItemData(hWayPointItem,nWayPointID);
		}
	//}
		m_TreeWayPointStandFamily.Expand(hWayPoint,TVE_EXPAND);

//	HTREEITEM hStandFamily = m_TreeWayPointStandFamily.InsertItem(_T("Stand Family:"));
	//iter = m_vStandFamily.begin();
	//for (; iter != m_vStandFamily.end(); ++iter)
	//{
	//	HTREEITEM hStandFamilyItem = m_TreeWayPointStandFamily.InsertItem(iter->first,hStandFamily);
	//	m_TreeWayPointStandFamily.SetItemData(hStandFamilyItem,iter->second);
	//}
//----------------------------------------------------------
// 	std::vector<int> vAirportIds;
// 	InputAirside::GetAirportList(m_nProjID, vAirportIds);
// 	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
// 	{
// 		ALTAirport airport;
// 		airport.ReadAirport(*iterAirportID);
// 		airport.getName();
// 		//HTREEITEM hAirport = m_TreeWayPointStandFamily.InsertItem(airport.getName(),hStandFamily);
// 
// 		//m_TreeWayPointStandFamily.SetItemData(hAirport, *iterAirportID);
// 
// 		ALTObjectList vStands;
// 		ALTAirport::GetStandList(*iterAirportID,vStands);
// 
// 		for(int i = 0;i< (int)vStands.size(); ++i)
// 		{
// 			Stand * pStand =(Stand*) vStands.at(i).get();
// 			AddObjectToTree(hStandFamily,pStand);
// 		}

		//std::vector<int> vStands;
		//ALTAirport::GetStandsIDs(*iterAirportID, vStands);
		//for (std::vector<int>::iterator iterStandID = vStands.begin(); iterStandID != vStands.end(); ++iterStandID)
		//{
		//	Stand stand;
		//	stand.ReadObject(*iterStandID);

		//	AddObjectToTree(hStandFamily, &stand);	
		//}

		//m_TreeWayPointStandFamily.Expand(hAirport, TVE_EXPAND);
//	}
//	m_TreeWayPointStandFamily.Expand(hStandFamily, TVE_EXPAND);
	//----------------------------------------------------------
}
void CDlgWayPointStandFamily::AddObjectToTree(HTREEITEM hObjRoot,ALTObject* pObject)
{
	ASSERT(hObjRoot);

	ALTObjectID objName;
	pObject->getObjName(objName);
	HTREEITEM hParentItem = hObjRoot;
	bool bObjNode = false;
	CString strNodeName = _T("");

	for (int nLevel =0; nLevel< OBJECT_STRING_LEVEL; ++nLevel)
	{
		if (nLevel != OBJECT_STRING_LEVEL -1 )
		{
			HTREEITEM hItem = FindObjNode(hParentItem,objName.m_val[nLevel].c_str());
			if (hItem != NULL)
			{
				hParentItem = hItem;
				continue;
			}
			if (objName.m_val[nLevel+1] != _T(""))
			{
				HTREEITEM hTreeItem = m_TreeWayPointStandFamily.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
//				m_TreeWayPointStandFamily.Expand(hParentItem, TVE_EXPAND);
				hParentItem = hTreeItem;

				continue;
			}
			else //objNode
			{
				HTREEITEM hTreeItem = m_TreeWayPointStandFamily.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
//				m_TreeWayPointStandFamily.Expand(hParentItem, TVE_EXPAND);
				break;
			}

		}
		else
		{
			HTREEITEM hTreeItem = m_TreeWayPointStandFamily.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
//			m_TreeWayPointStandFamily.Expand(hParentItem, TVE_EXPAND);
			break;
		}
	}
}
HTREEITEM CDlgWayPointStandFamily::FindObjNode(HTREEITEM hParentItem,const CString& strNodeText)
{
	HTREEITEM hRetItem = NULL;

	HTREEITEM hChildItem = m_TreeWayPointStandFamily.GetChildItem(hParentItem);

	//find
	while (hChildItem)
	{
		CString strItemText = m_TreeWayPointStandFamily.GetItemText(hChildItem);
		if(strItemText.CompareNoCase(strNodeText) == 0)
		{
			hRetItem = hChildItem;
			break;
		}
		hChildItem = m_TreeWayPointStandFamily.GetNextSiblingItem(hChildItem);
	}
	return hRetItem;
}

BOOL CDlgWayPointStandFamily::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
//	ALTObject altObject;
//	altObject.ReadObject(m_nProjID);
//	int m_nAirportID = altObject.getAptID();

//	std::vector<ALTObject> vWaypoint;
//	ALTObject::GetObjectList(ALT_WAYPOINT,m_nAirportID,vWaypoint);

//	for (int i = 0; i < static_cast<int>(vWaypoint.size()); ++ i)
//	{
//		m_vWaypoint.push_back(std::make_pair(vWaypoint[i].GetObjNameString(),vWaypoint[i].getID()));
//	}

//	std::vector<ALTObject> m_vStandFamily;
//	ALTObject::GetObjectList(ALT_STAND,m_nAirportID,m_vStandFamily);

	//for (int i = 0; i < static_cast<int>(vStandFamily.size()); ++ i)
	//{
	//	m_vStandFamily.push_back(std::make_pair(vStandFamily[i].GetObjNameString(),vStandFamily[i].getID()));
	//}
	GetAllStand();
	LoadTree();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgWayPointStandFamily::OnTvnSelchangedTreeWaypointandstandfamily(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here


	*pResult = 0;
}
void CDlgWayPointStandFamily::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	HTREEITEM hSel = m_TreeWayPointStandFamily.GetSelectedItem();
//	HTREEITEM hParentItem = m_TreeWayPointStandFamily.GetParentItem(hSel);
	if (!hSel)
	{
		MessageBox("Please select a Way Point or a stand family!", "Error", MB_OK);
		return;
	}
	if(WayPointStandFamilyInTree())
		OnOK();
}

bool CDlgWayPointStandFamily::WayPointStandFamilyInTree()
{
	//--------------------------------------
	HTREEITEM hSelItem = m_TreeWayPointStandFamily.GetSelectedItem();
	HTREEITEM hParentItem = m_TreeWayPointStandFamily.GetParentItem(hSelItem);
	HTREEITEM hRootItem;
	if(!hParentItem)
		hRootItem = hSelItem;
	while(hParentItem)
	{
		hRootItem = hParentItem;
		hParentItem = m_TreeWayPointStandFamily.GetParentItem(hParentItem);
	}
	CString strRoot = m_TreeWayPointStandFamily.GetItemText(hRootItem);
	if(strcmp(strRoot,"Way Point:") == 0)
	{
		HTREEITEM hParentItemTemp = NULL;
		hParentItemTemp = m_TreeWayPointStandFamily.GetParentItem(hSelItem);
		if (!hParentItemTemp)
		{
			MessageBox("Please select a Way Point!", "Error", MB_OK);
			return 0;
		}
		m_nWayStandFlag = ALTOBJECT_TYPE(ALT_WAYPOINT);
//		CString strWayPoint = m_TreeWayPointStandFamily.GetItemText(hSelItem);
		m_strWayPoint = m_TreeWayPointStandFamily.GetItemText(hSelItem);
		m_nSelWayPointID = m_TreeWayPointStandFamily.GetItemData(hSelItem); 
//		CString strAirPort = m_TreeWayPointStandFamily.GetItemText(m_TreeWayPointStandFamily.GetParentItem(hSelItem));
//		m_strWayPoint.Format("%s:%s",strAirPort,strWayPoint);
	}

	//--------------------------------------
	else
	{
		HTREEITEM hParentItemTemp = NULL;
		hParentItemTemp = m_TreeWayPointStandFamily.GetParentItem(hSelItem);
		if (!hParentItemTemp)
		{
			MessageBox("Please select a Stand Family!", "Error", MB_OK);
			return 0;
		}
		m_nWayStandFlag = ALTOBJECT_TYPE(ALT_STAND);
		HTREEITEM hItem = m_TreeWayPointStandFamily.GetSelectedItem();

		ALTObjectID objName;

		switch(GetCurStandGroupNamePos(hItem))
		{
		case 0:
			{
				objName.at(0) = m_TreeWayPointStandFamily.GetItemText(hItem);			
				objName.at(3) = "";
				objName.at(2) = "";
				objName.at(1) = "";
				m_strStandFamily.Format("%s", objName.at(0).c_str());
			}
			break;

		case 1:
			{
				objName.at(1) = m_TreeWayPointStandFamily.GetItemText(hItem);
				hItem = m_TreeWayPointStandFamily.GetParentItem(hItem);
				objName.at(0) = m_TreeWayPointStandFamily.GetItemText(hItem);
				objName.at(3) = "";
				objName.at(2) = "";
				m_strStandFamily.Format("%s-%s", objName.at(0).c_str(), objName.at(1).c_str());
			}
			break;

		case 2:
			{			
				objName.at(2) = m_TreeWayPointStandFamily.GetItemText(hItem);
				hItem = m_TreeWayPointStandFamily.GetParentItem(hItem);
				objName.at(1) = m_TreeWayPointStandFamily.GetItemText(hItem);
				hItem = m_TreeWayPointStandFamily.GetParentItem(hItem);
				objName.at(0) = m_TreeWayPointStandFamily.GetItemText(hItem);
				objName.at(3) = "";
				m_strStandFamily.Format("%s-%s-%s", objName.at(0).c_str(), objName.at(1).c_str(), objName.at(2).c_str());
			}
			break;
		case 3:
			{
				objName.at(3) = m_TreeWayPointStandFamily.GetItemText(hItem);
				hItem = m_TreeWayPointStandFamily.GetParentItem(hItem);
				objName.at(2) = m_TreeWayPointStandFamily.GetItemText(hItem);
				hItem = m_TreeWayPointStandFamily.GetParentItem(hItem);
				objName.at(1) = m_TreeWayPointStandFamily.GetItemText(hItem);
				hItem = m_TreeWayPointStandFamily.GetParentItem(hItem);
				objName.at(0) = m_TreeWayPointStandFamily.GetItemText(hItem);
				//hItem = m_TreeWayPointStandFamily.GetParentItem(hItem);
				//CString strAirport = m_TreeWayPointStandFamily.GetItemText(hItem);
				m_strStandFamily.Format("%s-%s-%s-%s", objName.at(0).c_str(), objName.at(1).c_str(), objName.at(2).c_str(), objName.at(3).c_str());
			}
			break;

		default:
			{
				m_strStandFamily = "";
				MessageBox("Please select a Stand Family!", "Error", MB_OK);
				return 0;
			}		
			break;
		}

		ALTObjectGroup altObjGroup;	
		altObjGroup.setType(ALT_STAND);
		altObjGroup.setName(objName);

		try
		{
			CADODatabase::BeginTransaction();
			m_nSelStandFamilyID = altObjGroup.InsertData(m_nProjID);
			CADODatabase::CommitTransaction();

		}
		catch (CADOException &e)
		{
			CADODatabase::RollBackTransation();
			e.ErrorMessage();
		}
	}
	return 1;
}

int CDlgWayPointStandFamily::GetCurStandGroupNamePos(HTREEITEM hTreeItem)
{
	if (NULL == hTreeItem)
	{
		return -1;
	}

	HTREEITEM hParentItem = m_TreeWayPointStandFamily.GetParentItem(hTreeItem);

	//root item
	if (NULL == hParentItem)
	{
		return -1;
	}

	hParentItem = m_TreeWayPointStandFamily.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 0;
	}

	hParentItem = m_TreeWayPointStandFamily.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 1;
	}

	hParentItem = m_TreeWayPointStandFamily.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 2;
	}

	hParentItem = m_TreeWayPointStandFamily.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 3;
	}

	return -1;
}

void CDlgWayPointStandFamily::GetAllStand()
{
	//std::vector<int> vAirportIds;
	//InputAirside::GetAirportList(m_nProjID, vAirportIds);
	//for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	//{
	//	std::vector<ALTObject> vWaypoint;
	//	ALTObject::GetObjectList(ALT_WAYPOINT,*iterAirportID,vWaypoint);
	//	std::vector<ALTObject>::iterator iter = vWaypoint.begin();
	//	for (; iter != vWaypoint.end(); ++iter)	
	//		m_vWaypoint.push_back(*iter);
	//}

	ALTObject::GetObjectList(ALT_WAYPOINT,m_nProjID,m_vWaypoint);
}
