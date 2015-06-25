// DlgTowingRouteStandfamilyAndTemporaryParking.cpp : implementation file
//

#include "stdafx.h"
//#include "AirsideGUI.h"
#include "Resource.h"
#include "DlgTowingRouteStandfamilyAndTemporaryParking.h"
#include ".\dlgtowingroutestandfamilyandtemporaryparking.h"
#include "..\InputAirside\ALTAirport.h"
#include "..\InputAirside\ALTObjectGroup.h"
#include "..\InputAirside\InputAirside.h"
#include "..\InputAirside\stand.h"
#include "..\InputAirside\Taxiway.h"

// CDlgTowingRouteStandfamilyAndTemporaryParking dialog

IMPLEMENT_DYNAMIC(CDlgTowingRouteStandfamilyAndTemporaryParking, CDialog)
CDlgTowingRouteStandfamilyAndTemporaryParking::CDlgTowingRouteStandfamilyAndTemporaryParking(int nProjID, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTowingRouteStandfamilyAndTemporaryParking::IDD, pParent)
	,m_nProjID(nProjID)
{
}

CDlgTowingRouteStandfamilyAndTemporaryParking::~CDlgTowingRouteStandfamilyAndTemporaryParking()
{
}

void CDlgTowingRouteStandfamilyAndTemporaryParking::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_STANDFAMILYANDTEMPORARYPARKING, m_wndTree);
}


BEGIN_MESSAGE_MAP(CDlgTowingRouteStandfamilyAndTemporaryParking, CDialog)
END_MESSAGE_MAP()


// CDlgTowingRouteStandfamilyAndTemporaryParking message handlers

BOOL CDlgTowingRouteStandfamilyAndTemporaryParking::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	LoadTree();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgTowingRouteStandfamilyAndTemporaryParking::LoadTree()
{
	HTREEITEM hStandFamily = m_wndTree.InsertItem(_T("Stand Family :"));
	m_wndTree.InsertItem(_T("All Stand"),hStandFamily);
	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjID, vAirportIds);
	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	{
		ALTObjectList vStands;
		ALTAirport::GetStandList(*iterAirportID,vStands);

		for(int i = 0;i< (int)vStands.size(); ++i)
		{
			Stand * pStand =(Stand*) vStands.at(i).get();
			AddObjectToTree(hStandFamily,pStand);
		}

		//std::vector<int> vStands;
		//ALTAirport::GetStandsIDs(*iterAirportID, vStands);
		//for (std::vector<int>::iterator iterStandID = vStands.begin(); iterStandID != vStands.end(); ++iterStandID)
		//{
		//	Stand stand;
		//	stand.ReadObject(*iterStandID);
		//	AddObjectToTree(hStandFamily, &stand);	
		//}
	}
	m_wndTree.Expand(hStandFamily, TVE_EXPAND);

	HTREEITEM hTemporaryParking = m_wndTree.InsertItem(_T("Temporary Parking :"));
	CString strSQL;
	strSQL.Format(_T("SELECT TAXIWAYID\
					 FROM IN_TEMPPARKINGCRITERIA_TAXIWAY\
					 WHERE (PROJID = %d)"),m_nProjID);

	long lRecordCount = 0;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
	std::vector<int> vTemporaryParkingIds;
	int nTemporaryParkingID;
	while(!adoRecordset.IsEOF())
	{
		adoRecordset.GetFieldValue(_T("TAXIWAYID"),nTemporaryParkingID);
		vTemporaryParkingIds.push_back(nTemporaryParkingID);
		adoRecordset.MoveNextData();
	}
	for (std::vector<int>::iterator iterTemporaryParkingID = vTemporaryParkingIds.begin(); iterTemporaryParkingID != vTemporaryParkingIds.end(); ++iterTemporaryParkingID)
	{
		Taxiway taxiway;
		taxiway.ReadObject(*iterTemporaryParkingID);
		HTREEITEM hItem = m_wndTree.InsertItem(taxiway.GetMarking().c_str(),hTemporaryParking);
		m_wndTree.SetItemData(hItem,*iterTemporaryParkingID);
	}
	m_wndTree.Expand(hTemporaryParking, TVE_EXPAND);
}

void CDlgTowingRouteStandfamilyAndTemporaryParking::AddObjectToTree(HTREEITEM hObjRoot,ALTObject* pObject)
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
				HTREEITEM hTreeItem = m_wndTree.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
				//				m_wndTree.Expand(hParentItem, TVE_EXPAND);
				hParentItem = hTreeItem;

				continue;
			}
			else //objNode
			{
				HTREEITEM hTreeItem = m_wndTree.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
				//				m_wndTree.Expand(hParentItem, TVE_EXPAND);
				break;
			}

		}
		else
		{
			HTREEITEM hTreeItem = m_wndTree.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
			//			m_wndTree.Expand(hParentItem, TVE_EXPAND);
			break;
		}
	}
}

HTREEITEM CDlgTowingRouteStandfamilyAndTemporaryParking::FindObjNode(HTREEITEM hParentItem,const CString& strNodeText)
{
	HTREEITEM hRetItem = NULL;

	HTREEITEM hChildItem = m_wndTree.GetChildItem(hParentItem);

	//find
	while (hChildItem)
	{
		CString strItemText = m_wndTree.GetItemText(hChildItem);
		if(strItemText.CompareNoCase(strNodeText) == 0)
		{
			hRetItem = hChildItem;
			break;
		}
		hChildItem = m_wndTree.GetNextSiblingItem(hChildItem);
	}
	return hRetItem;
}

bool CDlgTowingRouteStandfamilyAndTemporaryParking::StandFamilyAndTemporaryParkingInTree()
{
	//--------------------------------------
	HTREEITEM hSelItem = m_wndTree.GetSelectedItem();
	HTREEITEM hParentItem = m_wndTree.GetParentItem(hSelItem);
	HTREEITEM hRootItem;
	if(!hParentItem)
		hRootItem = hSelItem;
	while(hParentItem)
	{
		hRootItem = hParentItem;
		hParentItem = m_wndTree.GetParentItem(hParentItem);
	}
	CString strRoot = m_wndTree.GetItemText(hRootItem);
	if(strcmp(strRoot,_T("Stand Family :")) == 0)
	{
		m_nSelALTObjectFlag = ALTOBJECT_TYPE(ALT_STAND);
		HTREEITEM hSelItem = m_wndTree.GetSelectedItem();
		if(m_wndTree.GetItemText(hSelItem) == "All Stand")
		{
			m_nSelALTObjectID = -1;
			m_strSelALTObjectName = "All Stand";
			return 1;
		}
		ALTObjectID objName;
		switch(GetCurStandGroupNamePos(hSelItem))
		{
		case 0:
			{
				objName.at(0) = m_wndTree.GetItemText(hSelItem);			
				objName.at(3) = "";
				objName.at(2) = "";
				objName.at(1) = "";
				m_strSelALTObjectName.Format("%s", objName.at(0).c_str());
			}
			break;
		case 1:
			{
				objName.at(1) = m_wndTree.GetItemText(hSelItem);
				hSelItem = m_wndTree.GetParentItem(hSelItem);
				objName.at(0) = m_wndTree.GetItemText(hSelItem);
				objName.at(3) = "";
				objName.at(2) = "";
				m_strSelALTObjectName.Format("%s-%s", objName.at(0).c_str(), objName.at(1).c_str());
			}
			break;
		case 2:
			{			
				objName.at(2) = m_wndTree.GetItemText(hSelItem);
				hSelItem = m_wndTree.GetParentItem(hSelItem);
				objName.at(1) = m_wndTree.GetItemText(hSelItem);
				hSelItem = m_wndTree.GetParentItem(hSelItem);
				objName.at(0) = m_wndTree.GetItemText(hSelItem);
				objName.at(3) = "";
				m_strSelALTObjectName.Format("%s-%s-%s", objName.at(0).c_str(), objName.at(1).c_str(), objName.at(2).c_str());
			}
			break;
		case 3:
			{
				objName.at(3) = m_wndTree.GetItemText(hSelItem);
				hSelItem = m_wndTree.GetParentItem(hSelItem);
				objName.at(2) = m_wndTree.GetItemText(hSelItem);
				hSelItem = m_wndTree.GetParentItem(hSelItem);
				objName.at(1) = m_wndTree.GetItemText(hSelItem);
				hSelItem = m_wndTree.GetParentItem(hSelItem);
				objName.at(0) = m_wndTree.GetItemText(hSelItem);
				//hSelItem = m_wndTree.GetParentItem(hSelItem);
				//CString strAirport = m_wndTree.GetItemText(hSelItem);
				m_strSelALTObjectName.Format("%s-%s-%s-%s", objName.at(0).c_str(), objName.at(1).c_str(), objName.at(2).c_str(), objName.at(3).c_str());
			}
			break;
		default:
			{
				m_strSelALTObjectName = "";
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
			m_nSelALTObjectID = altObjGroup.InsertData(m_nProjID);
			CADODatabase::CommitTransaction();
		}
		catch (CADOException &e)
		{
			CADODatabase::RollBackTransation();
			e.ErrorMessage();
		}
	}
	else
	{
		m_nSelALTObjectFlag = ALTOBJECT_TYPE(ALT_TAXIWAY);
		m_strSelALTObjectName = m_wndTree.GetItemText(hSelItem);
		m_nSelALTObjectID = m_wndTree.GetItemData(hSelItem); 
	}
	return 1;
}

int CDlgTowingRouteStandfamilyAndTemporaryParking::GetCurStandGroupNamePos(HTREEITEM hTreeItem)
{
	if (NULL == hTreeItem)
	{
		return -1;
	}

	HTREEITEM hParentItem = m_wndTree.GetParentItem(hTreeItem);

	//root item
	if (NULL == hParentItem)
	{
		return -1;
	}

	hParentItem = m_wndTree.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 0;
	}

	hParentItem = m_wndTree.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 1;
	}

	hParentItem = m_wndTree.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 2;
	}

	hParentItem = m_wndTree.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 3;
	}

	return -1;
}

void CDlgTowingRouteStandfamilyAndTemporaryParking::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	HTREEITEM hSelItem = m_wndTree.GetSelectedItem();
	HTREEITEM hParentItem = m_wndTree.GetParentItem(hSelItem);
	if (!hSelItem || !hParentItem)
	{
		MessageBox("Please select a Stand Family or Temporary Parking!", "Error", MB_OK);
		return;
	}
	if(StandFamilyAndTemporaryParkingInTree())
		CDialog::OnOK();
}
