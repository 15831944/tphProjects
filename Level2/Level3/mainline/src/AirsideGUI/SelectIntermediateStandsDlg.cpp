// SelectIntermediateStandsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "SelectIntermediateStandsDlg.h"
#include "../InputAirside/TowOffStandAssignmentData.h"
#include "../InputAirside/InputAirside.h"
#include "../InputAirside/ALTAirport.h"
#include "../InputAirside/ALTObject.h"
#include "../InputAirside/TemporaryParkingCriteria.h"
#include "../InputAirside/Taxiway.h"
#include "../InputAirside/ALTObject.h"
#include "../InputAirside/stand.h"
#include "../InputAirside/ALTObjectGroup.h"
#include <iostream>
#include <vector>
#include ".\selectintermediatestandsdlg.h"

using namespace std;

// CSelectIntermediateStandsDlg dialog
const int ANY_DEF = STAND_ID_ALL;

IMPLEMENT_DYNAMIC(CSelectIntermediateStandsDlg, CDialog)
CSelectIntermediateStandsDlg::CSelectIntermediateStandsDlg(int nProjID, CTowOffStandAssignmentData *pTowOffStandAssignmentData,
		int nStandID,CSelectIntermediateStandsDlg::SelectStandType enumParkStandType/* = ParkStand*/, int nPriorityStandID/* = -1*/,
		CWnd* pParent /* = NULL */)
	: CDialog(CSelectIntermediateStandsDlg::IDD, pParent)
	, m_nProjID(nProjID)
	, m_nCurStandID(nStandID)
	, m_enumSelectStandType(enumParkStandType)
	, m_nPriorityStandID(nPriorityStandID)
	, m_pNewCreatedData(NULL)
{
	ASSERT(pTowOffStandAssignmentData);
	m_pTowOffStandAssignment = pTowOffStandAssignmentData;

	m_pSelStand = new Stand;
}

CSelectIntermediateStandsDlg::~CSelectIntermediateStandsDlg()
{
	if (NULL != m_pSelStand)
	{
		delete m_pSelStand;
	}
}

void CSelectIntermediateStandsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_INTERMEDIATESTANDS, m_TreeCtrlIntermediateStands);
}


BEGIN_MESSAGE_MAP(CSelectIntermediateStandsDlg, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_INTERMEDIATESTANDS, OnTvnSelchangedTreeIntermediatestands)
END_MESSAGE_MAP()

// CSelectIntermediateStandsDlg message handlers
BOOL CSelectIntermediateStandsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjID, vAirportIds);
	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	{
		ALTAirport airport;
		airport.ReadAirport(*iterAirportID);

		//std::vector<ALTObject> vStand;
		//ALTObject::GetObjectList(ALT_STAND,*iterAirportID,vStand);
		ALTObjectList vObject;
		ALTAirport::GetStandList(*iterAirportID,vObject);

		int nStandCount = static_cast<int>(vObject.size());
//		int nTempParkingCriteriaCount = m_pTempParkingCriteriaList->GetItemCount();
		//no stand and no tempParkingCriteria
		if (nStandCount < 1)
		{
			continue;
		}

		//insert airport
		HTREEITEM hAirportItem = m_TreeCtrlIntermediateStands.InsertItem(airport.getName());

		CString strAny = _T("All");
		HTREEITEM hAny = m_TreeCtrlIntermediateStands.InsertItem(strAny,hAirportItem);
		m_TreeCtrlIntermediateStands.SetItemData(hAny,ANY_DEF);

		//insert stands
		if (0 < nStandCount)
		{
			CString strStand;
			strStand.LoadString(IDS_STAND);

			HTREEITEM hStandItem = m_TreeCtrlIntermediateStands.InsertItem(strStand, hAirportItem);
			m_TreeCtrlIntermediateStands.SetItemData(hStandItem, (DWORD_PTR)INTERMEDIATESTANDS_STAND);

			for (int i = 0; i < static_cast<int>(vObject.size()); ++ i)
			{
				AddObjectToTree(hStandItem, vObject[i].get());	
			}

			m_TreeCtrlIntermediateStands.Expand(hAirportItem, TVE_EXPAND);
			m_TreeCtrlIntermediateStands.Expand(hStandItem, TVE_EXPAND);
		}
	}

	return TRUE;
}

BOOL CSelectIntermediateStandsDlg::IsSelectStand(HTREEITEM hSelItem)
{
	if (NULL == hSelItem)
	{
		return FALSE;
	}

	HTREEITEM hParentItem = m_TreeCtrlIntermediateStands.GetParentItem(hSelItem);

	while(NULL != hParentItem)
	{
		HTREEITEM hRootItem = m_TreeCtrlIntermediateStands.GetParentItem(hParentItem);

		if (NULL == hRootItem)
		{
			return FALSE;
		}
		else
		{
			HTREEITEM hNullItem = m_TreeCtrlIntermediateStands.GetParentItem(hRootItem);
			if (NULL == hNullItem)
			{
				INTERMEDIATESTANDSTYPE interMediateStandsType =
					(INTERMEDIATESTANDSTYPE)m_TreeCtrlIntermediateStands.GetItemData(hParentItem);

				if (interMediateStandsType == INTERMEDIATESTANDS_STAND)
				{
					return TRUE;
				}
				else
				{
					return FALSE;
				}				
			}
			else
			{
				hParentItem = hRootItem;
			}
		}
	}

	return FALSE;
}


void CSelectIntermediateStandsDlg::OnTvnSelchangedTreeIntermediatestands(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	HTREEITEM hSelItem = m_TreeCtrlIntermediateStands.GetSelectedItem();

	//select stand
	if (IsSelectStand(hSelItem))
	{
		SetCurStandGroupName(hSelItem);
	}
	*pResult = 0;
}

BOOL CSelectIntermediateStandsDlg::DealWithStandFamilyID(int nStandFamilyID)
{
	switch (m_enumSelectStandType)
	{
	case CSelectIntermediateStandsDlg::ParkStand:
		{
			m_pTowOffStandAssignment->SetParkStandsID(nStandFamilyID);
		}
		break;
	case CSelectIntermediateStandsDlg::PriorityStand:
		{
			if (m_nCurStandID == -1)
			{
				if (m_pTowOffStandAssignment->FindPriorityItem(nStandFamilyID) == NULL)
				{
					m_pNewCreatedData = m_pTowOffStandAssignment->AddPriorityItem(nStandFamilyID);
				}
				else
				{
					AfxMessageBox(_T("The priority exists!"));
					return FALSE;
				}
			}
			else
			{
				if ((m_pTowOffStandAssignment->FindPriorityItem(nStandFamilyID) == NULL)|| (m_nCurStandID == nStandFamilyID ))
				{
					m_pTowOffStandAssignment->SetPriorityItemData(m_nCurStandID,nStandFamilyID);
				}
				else
				{
					AfxMessageBox(_T("The priority exists!"));
					return FALSE;
				}
			}
		}
		break;
	case CSelectIntermediateStandsDlg::ReturnToStand:
		{
			if (m_nCurStandID == -1)
			{
				if (m_pTowOffStandAssignment->FindReturnToStandItem(m_nPriorityStandID, nStandFamilyID) == NULL)
				{
					m_pNewCreatedData = m_pTowOffStandAssignment->AddReturnToStandItem(m_nPriorityStandID, nStandFamilyID);
				}
				else
				{
					AfxMessageBox(_T("The return to stand exists!"));
					return FALSE;
				}
			}
			else
			{
				if (   (m_pTowOffStandAssignment->FindReturnToStandItem(m_nPriorityStandID, nStandFamilyID) == NULL)
					|| (m_nCurStandID == nStandFamilyID )
					)
				{
					m_pTowOffStandAssignment->SetReturnToStandItemData(m_nPriorityStandID, m_nCurStandID, nStandFamilyID);
				}
				else
				{
					AfxMessageBox(_T("The return to stand exists!"));
					return FALSE;
				}
			}
		}
		break;
	default:
		{
			ASSERT(FALSE);
		}
		break;
	}

	return TRUE;
}

void CSelectIntermediateStandsDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	HTREEITEM hSelItem = m_TreeCtrlIntermediateStands.GetSelectedItem();
	if (NULL == hSelItem)
	{
		AfxMessageBox(_T("Nothing Selected!"), MB_ICONEXCLAMATION);
		return;
	}

	if (!IsSelectStand(hSelItem))
	{
		int nID = m_TreeCtrlIntermediateStands.GetItemData(hSelItem);
		if (nID == STAND_ID_ALL)
		{
// 			m_pTowOffStandAssignment->SetParkStandsID(nID);
			if (FALSE == DealWithStandFamilyID(nID))
			{
				return;
			}
		}
		else
		{
			AfxMessageBox(_T("Nothing Selected!"), MB_ICONEXCLAMATION);
			return;
		}
	}
	else 
	{
		ALTObjectGroup altObjGroup;
		altObjGroup.setType(m_pSelStand->GetType());
		altObjGroup.setName(m_pSelStand->GetObjectName());
		m_strStandName = m_pSelStand->GetObjectName().GetIDString();
		int nStandFamilyID = -1;
		try
		{
			CADODatabase::BeginTransaction();
			nStandFamilyID = altObjGroup.InsertData(m_nProjID);
			CADODatabase::CommitTransaction();
		}
		catch (CADOException &e)
		{
			CADODatabase::RollBackTransation();
			e.ErrorMessage();
		}

		if (FALSE == DealWithStandFamilyID(nStandFamilyID))
		{
			return;
		}

	}
	CDialog::OnOK();
}

void CSelectIntermediateStandsDlg::AddObjectToTree(HTREEITEM hObjRoot,ALTObject* pObject)
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
				HTREEITEM hTreeItem = m_TreeCtrlIntermediateStands.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
				//m_TreeCtrlIntermediateStands.Expand(hParentItem, TVE_EXPAND);
				hParentItem = hTreeItem;

				continue;
			}
			else //objNode
			{
				HTREEITEM hTreeItem = m_TreeCtrlIntermediateStands.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
				//m_TreeCtrlIntermediateStands.Expand(hParentItem, TVE_EXPAND);
				break;
			}
			m_TreeCtrlIntermediateStands.Expand(hItem, TVE_EXPAND);

		}
		else
		{
			HTREEITEM hTreeItem = m_TreeCtrlIntermediateStands.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
			//m_TreeCtrlIntermediateStands.Expand(hParentItem, TVE_EXPAND);
			break;
		}
	}
	m_TreeCtrlIntermediateStands.Expand(hParentItem, TVE_EXPAND);
}

HTREEITEM CSelectIntermediateStandsDlg::FindObjNode(HTREEITEM hParentItem,const CString& strNodeText)
{
	HTREEITEM hRetItem = NULL;

	HTREEITEM hChildItem = m_TreeCtrlIntermediateStands.GetChildItem(hParentItem);

	//find
	while (hChildItem)
	{
		CString strItemText = m_TreeCtrlIntermediateStands.GetItemText(hChildItem);
		if(strItemText.CompareNoCase(strNodeText) == 0)
		{
			hRetItem = hChildItem;
			break;
		}
		hChildItem = m_TreeCtrlIntermediateStands.GetNextSiblingItem(hChildItem);
	}
	return hRetItem;
}

void CSelectIntermediateStandsDlg::SetCurStandGroupName(HTREEITEM hTreeItem)
{
	ALTObjectID objName;

	switch(GetCurStandGroupNamePos(hTreeItem))
	{
	case 0:
		{
			objName.at(0) = m_TreeCtrlIntermediateStands.GetItemText(hTreeItem);
		}
		break;

	case 1:
		{
			objName.at(1) = m_TreeCtrlIntermediateStands.GetItemText(hTreeItem);
			hTreeItem = m_TreeCtrlIntermediateStands.GetParentItem(hTreeItem);
			objName.at(0) = m_TreeCtrlIntermediateStands.GetItemText(hTreeItem);
			objName.at(2) = "";
			objName.at(3) = "";
		}
		break;

	case 2:
		{
			objName.at(2) = m_TreeCtrlIntermediateStands.GetItemText(hTreeItem);
			hTreeItem = m_TreeCtrlIntermediateStands.GetParentItem(hTreeItem);
			objName.at(1) = m_TreeCtrlIntermediateStands.GetItemText(hTreeItem);
			hTreeItem = m_TreeCtrlIntermediateStands.GetParentItem(hTreeItem);
			objName.at(0) = m_TreeCtrlIntermediateStands.GetItemText(hTreeItem);
			objName.at(3) = "";
		}
		break;

	case 3:
		{
			objName.at(3) = m_TreeCtrlIntermediateStands.GetItemText(hTreeItem);
			hTreeItem = m_TreeCtrlIntermediateStands.GetParentItem(hTreeItem);
			objName.at(2) = m_TreeCtrlIntermediateStands.GetItemText(hTreeItem);
			hTreeItem = m_TreeCtrlIntermediateStands.GetParentItem(hTreeItem);
			objName.at(1) = m_TreeCtrlIntermediateStands.GetItemText(hTreeItem);
			hTreeItem = m_TreeCtrlIntermediateStands.GetParentItem(hTreeItem);
			objName.at(0) = m_TreeCtrlIntermediateStands.GetItemText(hTreeItem);
		}
		break;

	default:
		break;
	}

	m_pSelStand->setObjName(objName);
}

int CSelectIntermediateStandsDlg::GetCurStandGroupNamePos(HTREEITEM hTreeItem)
{
	if (NULL == hTreeItem)
	{
		return -1;
	}

	HTREEITEM hParentItem = m_TreeCtrlIntermediateStands.GetParentItem(hTreeItem);

	if (NULL == hParentItem)
	{
		return -1;
	}

	//root item
	hParentItem = m_TreeCtrlIntermediateStands.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return -1;
	}

	hParentItem = m_TreeCtrlIntermediateStands.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 0;
	}

	hParentItem = m_TreeCtrlIntermediateStands.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 1;
	}

	hParentItem = m_TreeCtrlIntermediateStands.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 2;
	}

	hParentItem = m_TreeCtrlIntermediateStands.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 3;
	}

	return -1;
}

