#include "StdAfx.h"
#include "resource.h"
#include ".\deicepadandrunwayselectdlg.h"
#include "InputAirside/InputAirside.h"
#include "../InputAirside/ALTAirport.h"
#include "../InputAirside/Runway.h"
#include "InputAirside\ALTObjectGroup.h"

IMPLEMENT_DYNAMIC(CDeicepadAndRunwaySelectDlg,CDialog )
CDeicepadAndRunwaySelectDlg::CDeicepadAndRunwaySelectDlg(int nProjID, CWnd* pParent)
:CDialog(CDeicepadAndRunwaySelectDlg::IDD, pParent)
, m_nProjID(nProjID)
, m_bAllDeice(FALSE)
, m_bAllRunway(FALSE)
{
}

CDeicepadAndRunwaySelectDlg::~CDeicepadAndRunwaySelectDlg(void)
{
}

BEGIN_MESSAGE_MAP(CDeicepadAndRunwaySelectDlg, CDialog)	
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_STANDFAMILY, OnTvnSelchangedTreeDeicePads)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DEICEPADS, OnTvnSelchangedTreeRunways)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

void CDeicepadAndRunwaySelectDlg::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_DEICEPADS, m_TreeCtrlRunway);
	DDX_Control(pDX, IDC_TREE_STANDFAMILY, m_TreeCtrlDeicepads);

}

BOOL CDeicepadAndRunwaySelectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText("Select Deice pads and Runways");

	//m_TreeCtrlDeicepads.EnableMultiSelect();
	//m_TreeCtrlRunway.EnableMultiSelect();

	m_TreeCtrlDeicepads.ModifyStyle(0,TVS_SHOWSELALWAYS);
	m_TreeCtrlRunway.ModifyStyle(0,TVS_SHOWSELALWAYS);

	GetDlgItem(IDC_STATIC_FROM)->SetWindowText("Deice pad");
	GetDlgItem(IDC_STATIC_TO)->SetWindowText("Runway");

	SetDeicePadTreeContent();
	SetRunwayTreeContent();

	return TRUE;
}

void CDeicepadAndRunwaySelectDlg::SetDeicePadTreeContent()
{
	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjID, vAirportIds);
	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	{
		ALTAirport airport;
		airport.ReadAirport(*iterAirportID);
		airport.getName();
		HTREEITEM hAirport = m_TreeCtrlDeicepads.InsertItem(airport.getName());
		m_TreeCtrlDeicepads.SetItemData(hAirport, *iterAirportID);

		HTREEITEM hAll = m_TreeCtrlDeicepads.InsertItem("All", hAirport);
		m_TreeCtrlDeicepads.SetItemData(hAll, -2);

		ALTObjectList vDeicepads;
		ALTAirport::GetDeicePadList(*iterAirportID,vDeicepads);

		for(int i = 0;i< (int)vDeicepads.size(); ++i)
		{
			ALTObject* pDeice = vDeicepads.at(i).get();
			AddObjectToDeiceTree(hAirport,pDeice);
		}

		m_TreeCtrlDeicepads.Expand(hAirport, TVE_EXPAND);
	}
}

void CDeicepadAndRunwaySelectDlg::SetRunwayTreeContent()
{
	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjID, vAirportIds);
	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	{

		HTREEITEM hAll = m_TreeCtrlRunway.InsertItem("All");
		m_TreeCtrlRunway.SetItemData(hAll, -2);

		std::vector<int> vRunways;
		ALTAirport::GetRunwaysIDs(*iterAirportID, vRunways);
		for (std::vector<int>::iterator iterRunwayID = vRunways.begin(); iterRunwayID != vRunways.end(); ++iterRunwayID)
		{
			Runway runway;
			runway.ReadObject(*iterRunwayID);

			HTREEITEM hRunwayItem = m_TreeCtrlRunway.InsertItem(runway.GetObjNameString(), hAll);
			m_TreeCtrlRunway.SetItemData(hRunwayItem, *iterRunwayID);

			//marking 1
			HTREEITEM hMarking1Item = m_TreeCtrlRunway.InsertItem(runway.GetMarking1().c_str(), hRunwayItem);
			m_TreeCtrlRunway.SetItemData(hMarking1Item, (int)RUNWAYMARK_FIRST);

			//marking 2
			HTREEITEM hMarking2Item = m_TreeCtrlRunway.InsertItem(runway.GetMarking2().c_str(), hRunwayItem);
			m_TreeCtrlRunway.SetItemData(hMarking2Item, (int)RUNWAYMARK_SECOND);

		}

		m_TreeCtrlRunway.Expand(hAll, TVE_EXPAND);
	}
}

void CDeicepadAndRunwaySelectDlg::AddObjectToDeiceTree( HTREEITEM hObjRoot,ALTObject* pObject )
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
			HTREEITEM hItem = FindObjDeiceNode(hParentItem,objName.m_val[nLevel].c_str());
			if (hItem != NULL)
			{
				hParentItem = hItem;
				continue;
			}
			if (objName.m_val[nLevel+1] != _T(""))
			{
				HTREEITEM hTreeItem = m_TreeCtrlDeicepads.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
				hParentItem = hTreeItem;

				continue;
			}
			else //objNode
			{
				HTREEITEM hTreeItem = m_TreeCtrlDeicepads.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
				break;
			}

		}
		else
		{
			HTREEITEM hTreeItem = m_TreeCtrlDeicepads.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
			break;
		}
	}
}

HTREEITEM CDeicepadAndRunwaySelectDlg::FindObjDeiceNode( HTREEITEM hParentItem,const CString& strNodeText )
{
	HTREEITEM hRetItem = NULL;

	HTREEITEM hChildItem = m_TreeCtrlDeicepads.GetChildItem(hParentItem);

	//find
	while (hChildItem)
	{
		CString strItemText = m_TreeCtrlDeicepads.GetItemText(hChildItem);
		if(strItemText.CompareNoCase(strNodeText) == 0)
		{
			hRetItem = hChildItem;
			break;
		}
		hChildItem = m_TreeCtrlDeicepads.GetNextSiblingItem(hChildItem);
	}
	return hRetItem;
}

int CDeicepadAndRunwaySelectDlg::GetCurDeiceGroupNamePos( HTREEITEM hTreeItem )
{
	if (NULL == hTreeItem)
	{
		return -1;
	}

	HTREEITEM hParentItem = m_TreeCtrlDeicepads.GetParentItem(hTreeItem);

	//root item
	if (NULL == hParentItem)
	{
		return -1;
	}

	hParentItem = m_TreeCtrlDeicepads.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 0;
	}

	hParentItem = m_TreeCtrlDeicepads.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 1;
	}

	hParentItem = m_TreeCtrlDeicepads.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 2;
	}

	hParentItem = m_TreeCtrlDeicepads.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 3;
	}

	return -1;
}

void CDeicepadAndRunwaySelectDlg::GetDeiceAltID( HTREEITEM hItem, ALTObjectID& objName, CString& strDeicePads )
{
	switch(GetCurDeiceGroupNamePos(hItem))
	{
	case 0:
		{
			objName.at(0) = m_TreeCtrlDeicepads.GetItemText(hItem);

			strDeicePads.Format("%s", objName.at(0).c_str());
		}
		break;

	case 1:
		{
			objName.at(1) = m_TreeCtrlDeicepads.GetItemText(hItem);
			hItem = m_TreeCtrlDeicepads.GetParentItem(hItem);
			objName.at(0) = m_TreeCtrlDeicepads.GetItemText(hItem);
			objName.at(2) = "";
			objName.at(3) = "";

			strDeicePads.Format("%s-%s", objName.at(0).c_str(), objName.at(1).c_str());
		}
		break;

	case 2:
		{
			objName.at(2) = m_TreeCtrlDeicepads.GetItemText(hItem);
			hItem = m_TreeCtrlDeicepads.GetParentItem(hItem);
			objName.at(1) = m_TreeCtrlDeicepads.GetItemText(hItem);
			hItem = m_TreeCtrlDeicepads.GetParentItem(hItem);
			objName.at(0) = m_TreeCtrlDeicepads.GetItemText(hItem);
			objName.at(3) = "";

			strDeicePads.Format("%s-%s-%s", objName.at(0).c_str(), objName.at(1).c_str(), objName.at(2).c_str());
		}
		break;

	case 3:
		{
			objName.at(3) = m_TreeCtrlDeicepads.GetItemText(hItem);
			hItem = m_TreeCtrlDeicepads.GetParentItem(hItem);
			objName.at(2) = m_TreeCtrlDeicepads.GetItemText(hItem);
			hItem = m_TreeCtrlDeicepads.GetParentItem(hItem);
			objName.at(1) = m_TreeCtrlDeicepads.GetItemText(hItem);
			hItem = m_TreeCtrlDeicepads.GetParentItem(hItem);
			objName.at(0) = m_TreeCtrlDeicepads.GetItemText(hItem);

			strDeicePads.Format("%s-%s-%s-%s", objName.at(0).c_str(), objName.at(1).c_str(), objName.at(2).c_str(), objName.at(3).c_str());
		}
		break;

	default:
		strDeicePads = "";
		break;
	}
}

void CDeicepadAndRunwaySelectDlg::OnTvnSelchangedTreeRunways(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	HTREEITEM hItem = m_TreeCtrlRunway.GetSelectedItem();
	if(hItem != NULL)
	{
		int nId = (int)m_TreeCtrlRunway.GetItemData(hItem);
		if (nId == -2)
			m_bAllRunway = TRUE;
		else
			m_bAllRunway = FALSE;
	}

	UpdateData(FALSE);

	*pResult = 0;
}


void CDeicepadAndRunwaySelectDlg::OnTvnSelchangedTreeDeicePads(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	HTREEITEM hItem = m_TreeCtrlDeicepads.GetSelectedItem();
	if(hItem != NULL)
	{
		int nId = (int)m_TreeCtrlDeicepads.GetItemData(hItem);
		if (nId == -2)
			m_bAllDeice = TRUE;
		else
			m_bAllDeice = FALSE;
	}

	UpdateData(FALSE);

	*pResult = 0;
}


void CDeicepadAndRunwaySelectDlg::OnBnClickedOk()
{
	m_strDeices = "";
	m_strRunways = "";
	m_vDeiceSelID.clear();
	m_vRunwaySelID.clear();

	if(!m_bAllDeice)
	{
		int nSelCount = 1; //m_TreeCtrlDeicepads.GetSelectedCount();

		HTREEITEM hSelItem = m_TreeCtrlDeicepads.GetSelectedItem();

		for (int i = 0; i < nSelCount;++i) 
		{		
			if (hSelItem != NULL )
			{	
				ALTObjectGroup altObjGroup;
				ALTObjectID objName;
				CString strName;

				GetDeiceAltID(hSelItem,objName,strName);

				altObjGroup.setType(ALT_DEICEBAY);
				altObjGroup.setName(objName);
				int nSelDeiceFamilyID = altObjGroup.InsertData(m_nProjID);

				m_vDeiceSelID.push_back(nSelDeiceFamilyID);

				if (m_strDeices =="")
				{
					m_strDeices = strName;
				}
				else
				{
					m_strDeices += ", ";
					m_strDeices += strName;
				}
			}
//			hSelItem = m_TreeCtrlDeicepads.GetNextSelectedItem(hSelItem);
		}
	}
	else
		m_strDeices = "All";

	if(!m_bAllRunway)
	{
		int nSelCount = 1; //m_TreeCtrlRunway.GetSelectedCount();

		HTREEITEM hSelItem = m_TreeCtrlRunway.GetSelectedItem();

		for (int i = 0; i < nSelCount;++i) 
		{		
			if (hSelItem != NULL )
			{	
				HTREEITEM hParItem = m_TreeCtrlRunway.GetParentItem(hSelItem);
				CString strText = m_TreeCtrlRunway.GetItemText(hParItem);
				if (strText.CompareNoCase("All") == 0)
					continue;

				int nRwyID = (int)m_TreeCtrlRunway.GetItemData(hParItem);
				int nMark = (int)m_TreeCtrlRunway.GetItemData(hSelItem);
				CString strName = m_TreeCtrlRunway.GetItemText(hSelItem);
				
				m_vRunwaySelID.push_back(CPostDeice_BoundRoute::LogicRwyID(nRwyID, nMark));

				if (m_strRunways =="")
				{
					m_strRunways = strName;
				}
				else
				{
					m_strRunways += ", ";
					m_strRunways += strName;
				}
			}
			//hSelItem = m_TreeCtrlRunway.GetNextSelectedItem(hSelItem);
		}
	}
	else
		m_strRunways = "All";

	if (!m_strRunways.Compare(""))
	{
		MessageBox("Please select runway!", "Error", MB_OK|MB_ICONEXCLAMATION );
		return;
	}

	if (!m_strDeices.Compare(""))
	{
		MessageBox("Please select deice pad family!", "Error", MB_OK|MB_ICONEXCLAMATION );
		return;
	}
	OnOK();
}