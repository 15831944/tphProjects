// DlgReleasePointSelection.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "DlgReleasePointSelection.h"

#include "../InputAirside/ALTAirport.h"
#include "InputAirside\ALTObjectGroup.h"
#include "InputAirside/InputAirside.h"
#include "InputAirside/stand.h"
#include "InputAirside/DeicePad.h"
#include "InputAirside/Runway.h"
#include "InputAirside/StartPosition.h"

// DlgReleasePointSelection dialog

IMPLEMENT_DYNAMIC(DlgReleasePointSelection, CXTResizeDialog)
DlgReleasePointSelection::DlgReleasePointSelection(int nPrjID, CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(DlgReleasePointSelection::IDD, pParent)
	,m_nPrjID(nPrjID)
	,m_nObjID(-1)
{
}

DlgReleasePointSelection::~DlgReleasePointSelection()
{
}

void DlgReleasePointSelection::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_RELEASEPOINT, m_wndTreeCtrl);
}


BEGIN_MESSAGE_MAP(DlgReleasePointSelection, CXTResizeDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

// DlgReleasePointSelection message handlers

BOOL DlgReleasePointSelection::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	SetResize(IDC_TREE_RELEASEPOINT,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);

	// TODO:  Add extra initialization here
	LoadTree();
	return TRUE;  
}

void DlgReleasePointSelection::LoadTree()
{

	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nPrjID, vAirportIds);

	//Stand node
	ALTObjectList vStands;
	ALTAirport::GetStandList(vAirportIds.at(0),vStands);
	if (!vStands.empty())
	{
		HTREEITEM hStandFamily = m_wndTreeCtrl.InsertItem(_T("Stands:"));
		m_wndTreeCtrl.SetItemData(hStandFamily, (int)TOWOPERATIONDESTINATION_STAND);
		m_wndTreeCtrl.InsertItem(_T("All stands"),hStandFamily);

		for(int i = 0;i< (int)vStands.size(); ++i)
		{
			Stand * pStand =(Stand*) vStands.at(i).get();
			AddObjectToTree(hStandFamily,pStand);
		}

		m_wndTreeCtrl.Expand(hStandFamily, TVE_EXPAND);
	}

	//----------------------------------------------------------
	
	//deice pad
	ALTObjectList vDeices;
	ALTAirport::GetDeicePadList(vAirportIds.at(0),vDeices);

	if (!vDeices.empty())
	{
		HTREEITEM hDeicepad = m_wndTreeCtrl.InsertItem(_T("Deice pads:"));
		m_wndTreeCtrl.SetItemData(hDeicepad, (int)TOWOPERATIONDESTINATION_DEICINGPAD);
		m_wndTreeCtrl.InsertItem(_T("All pads"),hDeicepad);

		for(int i = 0;i< (int)vDeices.size(); ++i)
		{
			DeicePad * pDeice =(DeicePad*) vDeices.at(i).get();
			AddObjectToTree(hDeicepad,pDeice);
		}

		m_wndTreeCtrl.Expand(hDeicepad, TVE_EXPAND);
	}

	//----------------------------------------------------------

	//Runway
	HTREEITEM hRunway = m_wndTreeCtrl.InsertItem(_T("Runway threshold:"));
	m_wndTreeCtrl.SetItemData(hRunway, (int)TOWOPERATIONDESTINATION_RUNWAYTHRESHOLD);

	ALTObjectList vRunways;
	ALTAirport::GetRunwayList(vAirportIds.at(0),vRunways);

	for(int i = 0;i< (int)vRunways.size(); ++i)
	{
		Runway * pRunway =(Runway*) vRunways.at(i).get();

		CString strRunwayThreshold1;
		strRunwayThreshold1.Format(_T("%s"), pRunway->GetMarking1().c_str());

		HTREEITEM hThreshold = m_wndTreeCtrl.InsertItem(strRunwayThreshold1,hRunway,TVI_LAST);
		m_wndTreeCtrl.SetItemData(hThreshold,pRunway->getID());

		CString strRunwayThreshold2;
		strRunwayThreshold2.Format(_T("%s"), pRunway->GetMarking2().c_str());

		HTREEITEM hThreshold2 = m_wndTreeCtrl.InsertItem(strRunwayThreshold2,hRunway,TVI_LAST);
		m_wndTreeCtrl.SetItemData(hThreshold2,pRunway->getID());
	}

	m_wndTreeCtrl.Expand(hRunway, TVE_EXPAND);
	//----------------------------------------------------------

	//Interrupt line
	m_holdShortLines.ReadData(m_nPrjID);
	int nCount = m_holdShortLines.GetElementCount();
	if (nCount >0)
	{
		HTREEITEM hHoldshortline = m_wndTreeCtrl.InsertItem(_T("Taxi interrupt lines:"));
		m_wndTreeCtrl.SetItemData(hHoldshortline, (int)TOWOPERATIONDESTINATION_TAXIWAYHOLDSHORTLINE);

		for (int i =0; i < nCount; ++i)
		{
			CTaxiInterruptLine *pNode = m_holdShortLines.GetItem(i);
			if (pNode == NULL)
				continue;

			CString srtName = pNode->GetName();
			HTREEITEM hLine = m_wndTreeCtrl.InsertItem(srtName,hHoldshortline,TVI_LAST);
			m_wndTreeCtrl.SetItemData(hLine,pNode->GetID());
		}
		m_wndTreeCtrl.Expand(hHoldshortline, TVE_EXPAND);	
	}

	//---------------------------------------------------------------

	//Start position
	ALTObjectList vStartPos;
	ALTAirport::GetStartPositionList(vAirportIds.at(0),vStartPos);

	if (!vStartPos.empty())
	{
		HTREEITEM hStartPos = m_wndTreeCtrl.InsertItem(_T("Start position:"));
		m_wndTreeCtrl.SetItemData(hStartPos, (int)TOWOPERATIONDESTINATION_STARTPOSITION);
		m_wndTreeCtrl.InsertItem(_T("All start positions"),hStartPos);

		size_t tSize = vStartPos.size();
		for(size_t i = 0;i< tSize; ++i)
		{
			CStartPosition * pStartPos =(CStartPosition*) vStartPos.at(i).get();
			AddObjectToTree(hStartPos,pStartPos);
		}

		m_wndTreeCtrl.Expand(hStartPos, TVE_EXPAND);
	}

}
void DlgReleasePointSelection::AddObjectToTree(HTREEITEM hObjRoot,ALTObject* pObject)
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
				HTREEITEM hTreeItem = m_wndTreeCtrl.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
				hParentItem = hTreeItem;

				continue;
			}
			else //objNode
			{
				HTREEITEM hTreeItem = m_wndTreeCtrl.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
				break;
			}

		}
		else
		{
			HTREEITEM hTreeItem = m_wndTreeCtrl.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
			break;
		}
	}
}
HTREEITEM DlgReleasePointSelection::FindObjNode(HTREEITEM hParentItem,const CString& strNodeText)
{
	HTREEITEM hRetItem = NULL;

	HTREEITEM hChildItem = m_wndTreeCtrl.GetChildItem(hParentItem);

	//find
	while (hChildItem)
	{
		CString strItemText = m_wndTreeCtrl.GetItemText(hChildItem);
		if(strItemText.CompareNoCase(strNodeText) == 0)
		{
			hRetItem = hChildItem;
			break;
		}
		hChildItem = m_wndTreeCtrl.GetNextSiblingItem(hChildItem);
	}
	return hRetItem;
}

bool DlgReleasePointSelection::ALTObjectGroupInTree( TOWOPERATIONDESTINATION eType )
{
	if (eType != TOWOPERATIONDESTINATION_STAND && eType != TOWOPERATIONDESTINATION_DEICINGPAD && eType != TOWOPERATIONDESTINATION_STARTPOSITION)
		return false;

	//--------------------------------------
	HTREEITEM hSelItem = m_wndTreeCtrl.GetSelectedItem();
	if(m_wndTreeCtrl.GetItemText(hSelItem) == "All stands")
	{
		m_nObjID = -1;
		m_strSelectName = "All Stand";
		return 1;
	}
	if(m_wndTreeCtrl.GetItemText(hSelItem) == "All pads")
	{
		m_nObjID = -1;
		m_strSelectName = "All pads";
		return 1;
	}
	if(m_wndTreeCtrl.GetItemText(hSelItem) == "All start positions")
	{
		m_nObjID = -1;
		m_strSelectName = "All start positions";
		return 1;
	}
	HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hSelItem);

	if (!hParentItem)
		return false;

	ALTObjectID objName;

	switch(GetCurStandGroupNamePos(hSelItem))
	{
	case 0:
		{
			objName.at(0) = m_wndTreeCtrl.GetItemText(hSelItem);			
			objName.at(3) = "";
			objName.at(2) = "";
			objName.at(1) = "";
			m_strSelectName.Format("%s", objName.at(0).c_str());
		}
		break;

	case 1:
		{
			objName.at(1) = m_wndTreeCtrl.GetItemText(hSelItem);
			hSelItem = m_wndTreeCtrl.GetParentItem(hSelItem);
			objName.at(0) = m_wndTreeCtrl.GetItemText(hSelItem);
			objName.at(3) = "";
			objName.at(2) = "";
			m_strSelectName.Format("%s-%s", objName.at(0).c_str(), objName.at(1).c_str());
		}
		break;

	case 2:
		{			
			objName.at(2) = m_wndTreeCtrl.GetItemText(hSelItem);
			hSelItem = m_wndTreeCtrl.GetParentItem(hSelItem);
			objName.at(1) = m_wndTreeCtrl.GetItemText(hSelItem);
			hSelItem = m_wndTreeCtrl.GetParentItem(hSelItem);
			objName.at(0) = m_wndTreeCtrl.GetItemText(hSelItem);
			objName.at(3) = "";
			m_strSelectName.Format("%s-%s-%s", objName.at(0).c_str(), objName.at(1).c_str(), objName.at(2).c_str());
		}
		break;
	case 3:
		{
			objName.at(3) = m_wndTreeCtrl.GetItemText(hSelItem);
			hSelItem = m_wndTreeCtrl.GetParentItem(hSelItem);
			objName.at(2) = m_wndTreeCtrl.GetItemText(hSelItem);
			hSelItem = m_wndTreeCtrl.GetParentItem(hSelItem);
			objName.at(1) = m_wndTreeCtrl.GetItemText(hSelItem);
			hSelItem = m_wndTreeCtrl.GetParentItem(hSelItem);
			objName.at(0) = m_wndTreeCtrl.GetItemText(hSelItem);
			m_strSelectName.Format("%s-%s-%s-%s", objName.at(0).c_str(), objName.at(1).c_str(), objName.at(2).c_str(), objName.at(3).c_str());
		}
		break;

	default:
		{
			m_strSelectName= "";
			return false;
		}		
		break;
	}

	ALTObjectGroup altObjGroup;	
	altObjGroup.setName(objName);

	if (eType == TOWOPERATIONDESTINATION_STAND)
	{
		altObjGroup.setType(ALT_STAND);
	}

	if (eType == TOWOPERATIONDESTINATION_DEICINGPAD)
	{
		altObjGroup.setType(ALT_DEICEBAY);
	}

	if (eType == TOWOPERATIONDESTINATION_STARTPOSITION)
	{
		altObjGroup.setType(ALT_STARTPOSITION);
	}

	try
	{
		CADODatabase::BeginTransaction();
		m_nObjID = altObjGroup.InsertData(m_nPrjID);
		CADODatabase::CommitTransaction();

	}
	catch (CADOException &e)
	{
		CADODatabase::RollBackTransation();
		e.ErrorMessage();
	}

	return true;
}

int DlgReleasePointSelection::GetCurStandGroupNamePos(HTREEITEM hTreeItem)
{
	if (NULL == hTreeItem)
	{
		return -1;
	}

	HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hTreeItem);

	//root item
	if (NULL == hParentItem)
	{
		return -1;
	}

	hParentItem = m_wndTreeCtrl.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 0;
	}

	hParentItem = m_wndTreeCtrl.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 1;
	}

	hParentItem = m_wndTreeCtrl.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 2;
	}

	hParentItem = m_wndTreeCtrl.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 3;
	}

	return -1;
}

TOWOPERATIONDESTINATION DlgReleasePointSelection::GetSelectionType(HTREEITEM hSelItem)
{
	if (NULL == hSelItem)
	{
		return TOWOPERATIONDESTINATION_COUNT;
	}

	HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hSelItem);

	//root item
	if (NULL == hParentItem)
	{
		return TOWOPERATIONDESTINATION_COUNT;
	}

	HTREEITEM hGrandItem = m_wndTreeCtrl.GetParentItem(hParentItem);

	if (NULL == hGrandItem)
	{
		return (TOWOPERATIONDESTINATION)m_wndTreeCtrl.GetItemData(hParentItem);
	}

	hParentItem = hGrandItem;
	hGrandItem = m_wndTreeCtrl.GetParentItem(hParentItem);

	if (NULL == hGrandItem)
	{
		return (TOWOPERATIONDESTINATION)m_wndTreeCtrl.GetItemData(hParentItem);
	}

	hParentItem = hGrandItem;
	hGrandItem = m_wndTreeCtrl.GetParentItem(hParentItem);

	if (NULL == hGrandItem)
	{
		return (TOWOPERATIONDESTINATION)m_wndTreeCtrl.GetItemData(hParentItem);
	}

	hParentItem = hGrandItem;
	hGrandItem = m_wndTreeCtrl.GetParentItem(hParentItem);

	if (NULL == hGrandItem)
	{
		return (TOWOPERATIONDESTINATION)m_wndTreeCtrl.GetItemData(hParentItem);
	}

	return TOWOPERATIONDESTINATION_COUNT;
}

void DlgReleasePointSelection::GetSelection(CReleasePoint* pPoint)
{

	pPoint->SetEnumDestType(m_eSelType);
	pPoint->SetObjectID(m_nObjID);
	pPoint->setName(m_strSelectName);

}

void DlgReleasePointSelection::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	HTREEITEM hSel = m_wndTreeCtrl.GetSelectedItem();
	if (hSel == NULL || m_wndTreeCtrl.GetParentItem(hSel) == NULL)
	{
		MessageBox("Please select a release point!", "Error", MB_OK);
		return;
	}


	 m_eSelType = GetSelectionType(hSel);
	if (m_eSelType == TOWOPERATIONDESTINATION_RUNWAYTHRESHOLD || m_eSelType == TOWOPERATIONDESTINATION_TAXIWAYHOLDSHORTLINE)
	{
		m_nObjID = m_wndTreeCtrl.GetItemData(hSel);
		m_strSelectName = m_wndTreeCtrl.GetItemText(hSel);
	}
	else
	{
		if(!ALTObjectGroupInTree(m_eSelType))
		{
			MessageBox("Please select a release point!", "Error", MB_OK);
			return;
		}
	}

	OnOK();
}
