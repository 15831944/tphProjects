// SelectStandFamilyAndTakeoffPositionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "SelectStandFamilyAndTakeoffPositionDlg.h"
#include "InputAirside/InputAirside.h"
#include "../InputAirside/ALTAirport.h"
#include "../InputAirside/Runway.h"
#include "InputAirside/stand.h"
#include <iostream>
#include ".\runwayexitandstandfamilyselectdlg.h"
#include "InputAirside\ALTObjectGroup.h"
#include "InputAirside\Taxiway.h"


// CSelectStandFamilyAndTakeoffPositionDlg dialog

IMPLEMENT_DYNAMIC(CSelectStandFamilyAndTakeoffPositionDlg, CDialog)
CSelectStandFamilyAndTakeoffPositionDlg::CSelectStandFamilyAndTakeoffPositionDlg(const UINT nID,int nProjID, const CString& strSelExits,  const CString& strSelStands,CWnd* pParent /*=NULL*/)
	: CDialog(nID, pParent)
	, m_nProjID(nProjID)
	, m_bAllStand(FALSE)
	, m_bAllRunwayExit(FALSE)
{
	m_strRunwayExits = strSelExits;
	m_strStandGroups = strSelStands + ",";

}

CSelectStandFamilyAndTakeoffPositionDlg::~CSelectStandFamilyAndTakeoffPositionDlg()
{
	
}

void CSelectStandFamilyAndTakeoffPositionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_RUNWAYEXIT, m_TreeCtrlRunwayExit);
	DDX_Control(pDX, IDC_TREE_STANDFAMILY, m_TreeCtrlStandFamily);

}


BEGIN_MESSAGE_MAP(CSelectStandFamilyAndTakeoffPositionDlg, CDialog)	
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_RUNWAYEXIT, OnTvnSelchangedTreeRunwayExit)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_STANDFAMILY, OnTvnSelchangedTreeStandFamily)
	ON_NOTIFY(NM_KILLFOCUS,IDC_TREE_STANDFAMILY,OnNmKillfocusTreeStandFamily)
	ON_NOTIFY(NM_KILLFOCUS,IDC_TREE_RUNWAYEXIT,OnNmKillfocusTreeRunwayExit)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CSelectStandFamilyAndTakeoffPositionDlg message handlers
BOOL CSelectStandFamilyAndTakeoffPositionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//m_TreeCtrlRunwayExit.EnableMultiSelect();
	//m_TreeCtrlStandFamily.EnableMultiSelect();

	m_TreeCtrlRunwayExit.ModifyStyle(0,TVS_SHOWSELALWAYS);
	m_TreeCtrlStandFamily.ModifyStyle(0,TVS_SHOWSELALWAYS);

	SetRunwayExitTreeContent();
	SetStandFamilyTreeContent();

	m_strStandGroups = "";

	return TRUE;
}

CString CSelectStandFamilyAndTakeoffPositionDlg::GetRunwayExitName()
{
	return m_strRunwayExits;
}

CString CSelectStandFamilyAndTakeoffPositionDlg::GetStandFamilyName()
{
	return m_strStandGroups;
}

std::vector<int>& CSelectStandFamilyAndTakeoffPositionDlg::GetRunwayExitList()
{
	return m_vExitSelID;
}

std::vector<int>& CSelectStandFamilyAndTakeoffPositionDlg::GetStandGroupList()
{
	return m_vStandSelID;
}

BOOL CSelectStandFamilyAndTakeoffPositionDlg::IsAllRunwayExit()
{
	return m_bAllRunwayExit;
}

BOOL CSelectStandFamilyAndTakeoffPositionDlg::IsAllStand()
{
	return m_bAllStand;
}

void CSelectStandFamilyAndTakeoffPositionDlg::SetRunwayExitTreeContent(void)
{
	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjID, vAirportIds);
	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	{
		ALTAirport airport;
		airport.ReadAirport(*iterAirportID);

		HTREEITEM hAirport = m_TreeCtrlRunwayExit.InsertItem(airport.getName());
		m_TreeCtrlRunwayExit.SetItemData(hAirport, *iterAirportID);

		HTREEITEM hAll = m_TreeCtrlRunwayExit.InsertItem("All",hAirport);
		m_TreeCtrlRunwayExit.SetItemData(hAll,-2);

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

			HTREEITEM hRunwayItem = m_TreeCtrlRunwayExit.InsertItem(runway.GetObjNameString(), hAll);
			m_TreeCtrlRunwayExit.SetItemData(hRunwayItem, *iterRunwayID);

			//marking 1
			HTREEITEM hMarking1Item = m_TreeCtrlRunwayExit.InsertItem(runway.GetMarking1().c_str(), hRunwayItem);
			m_TreeCtrlRunwayExit.SetItemData(hMarking1Item, (int)RUNWAYMARK_FIRST);

			std::vector<RunwayExit>::iterator citer;
			CString runwayExitName;
			for (citer=runwayExitList.begin(); citer!=runwayExitList.end(); citer++)
			{
				RunwayExit runwayExit = *citer;
				//runwayExitName = runwayExit.GetName();
				GetRunwayExitNameString(&runwayExit, &runwayExitList, runwayExitName);

				HTREEITEM hPositionItem = m_TreeCtrlRunwayExit.InsertItem(runwayExitName, hMarking1Item);
				m_TreeCtrlRunwayExit.SetItemData(hPositionItem, (DWORD_PTR)runwayExit.GetID());

				if (IsExitSelected(runwayExitName))
					m_TreeCtrlRunwayExit.SelectItem(hPositionItem);				
			}
			m_TreeCtrlRunwayExit.Expand(hMarking1Item, TVE_EXPAND);

			//marking 2
			HTREEITEM hMarking2Item = m_TreeCtrlRunwayExit.InsertItem(runway.GetMarking2().c_str(), hRunwayItem);
			m_TreeCtrlRunwayExit.SetItemData(hMarking2Item, (int)RUNWAYMARK_SECOND);

			RunwayExitList runwayExitListMarkTwo;
			nRunwayExitCount = runway.GetExitList(RUNWAYMARK_SECOND, runwayExitListMarkTwo);
			for (citer=runwayExitListMarkTwo.begin(); citer!=runwayExitListMarkTwo.end(); citer++)
			{
				RunwayExit runwayExit = *citer;

				//runwayExitName = runwayExit.m_strName;	
				GetRunwayExitNameString(&runwayExit, &runwayExitListMarkTwo, runwayExitName);
				//runwayExitName = runwayExit.GetName();

				HTREEITEM hPositionItem = m_TreeCtrlRunwayExit.InsertItem(runwayExitName, hMarking2Item);
				m_TreeCtrlRunwayExit.SetItemData(hPositionItem, (DWORD_PTR)runwayExit.GetID());

				if (IsExitSelected(runwayExitName))
					m_TreeCtrlRunwayExit.SelectItem(hPositionItem);
			}
			m_TreeCtrlRunwayExit.Expand(hMarking2Item, TVE_EXPAND);
			m_TreeCtrlRunwayExit.Expand(hRunwayItem, TVE_EXPAND);

		}

		m_TreeCtrlRunwayExit.Expand(hAll, TVE_EXPAND);
		m_TreeCtrlRunwayExit.Expand(hAirport, TVE_EXPAND);
	}
}

void CSelectStandFamilyAndTakeoffPositionDlg::GetRunwayExitNameString(RunwayExit *pRunwayExit, RunwayExitList *pRunwayExitList, CString& strRunwayExitName)
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
		//if (pRunwayExit->GetSideType() == RunwayExit::EXIT_LEFT)
		//{
		//	strTaxiwayMark += "&LEFT";
		//}
		//else
		//{
		//	strTaxiwayMark += "&RIGHT";
		//}

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

void CSelectStandFamilyAndTakeoffPositionDlg::SetStandFamilyTreeContent(void)
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

void CSelectStandFamilyAndTakeoffPositionDlg::AddObjectToTree(HTREEITEM hObjRoot,ALTObject* pObject)
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

HTREEITEM CSelectStandFamilyAndTakeoffPositionDlg::FindObjNode(HTREEITEM hParentItem,const CString& strNodeText)
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

void CSelectStandFamilyAndTakeoffPositionDlg::OnTvnSelchangedTreeRunwayExit(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	HTREEITEM hItem = m_TreeCtrlRunwayExit.GetSelectedItem();
	if(hItem != NULL)
	{
		int nId = (int)m_TreeCtrlRunwayExit.GetItemData(hItem);
		if (nId == -2)
			m_bAllRunwayExit = TRUE;
	}
	
	UpdateData(FALSE);

	*pResult = 0;
}

void CSelectStandFamilyAndTakeoffPositionDlg::OnNmKillfocusTreeStandFamily(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here


	UpdateData(FALSE);

	*pResult = 0;
}

void CSelectStandFamilyAndTakeoffPositionDlg::OnNmKillfocusTreeRunwayExit(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here


	UpdateData(FALSE);

	*pResult = 0;
}

void CSelectStandFamilyAndTakeoffPositionDlg::OnTvnSelchangedTreeStandFamily(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	HTREEITEM hItem = m_TreeCtrlStandFamily.GetSelectedItem();
	if(hItem != NULL)
	{
		int nId = (int)m_TreeCtrlStandFamily.GetItemData(hItem);
		if (nId == -2)
			m_bAllStand = TRUE;
	}

	UpdateData(FALSE);

	*pResult = 0;
}

CSelectStandFamilyAndTakeoffPositionDlg::RUNWAYEXITTYPE CSelectStandFamilyAndTakeoffPositionDlg::GetRunwayExitType(HTREEITEM hRunwayTreeItem)
{
	if (NULL == hRunwayTreeItem)
	{
		return RUNWAYEXIT_DEFAULT;
	}

	HTREEITEM hParent = m_TreeCtrlRunwayExit.GetParentItem(hRunwayTreeItem);

	if (NULL == hParent)
	{
		return RUNWAYEXIT_DEFAULT;
	}
	else
	{
		HTREEITEM hParentParent = m_TreeCtrlRunwayExit.GetParentItem(hParent);

		if (NULL == hParentParent)
		{
			return RUNWAYEXIT_ALLRUNWAY;
		}
		else
		{
			HTREEITEM hParentParentParent = m_TreeCtrlRunwayExit.GetParentItem(hParentParent);

			if (NULL == hParentParentParent)
			{
				return RUNWAYEXIT_DEFAULT;
			}
			else
			{
				HTREEITEM hParentParentParentParent = m_TreeCtrlRunwayExit.GetParentItem(hParentParentParent);

				if (NULL == hParentParentParentParent)
				{
					return RUNWAYEXIT_LOGICRUNWAY;
				}
				else
				{
					HTREEITEM hItem = m_TreeCtrlRunwayExit.GetParentItem(hParentParentParentParent);
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

void CSelectStandFamilyAndTakeoffPositionDlg::GetStandFamilyAltID(HTREEITEM hItem, ALTObjectID& objName, CString& strStandFamily)
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

int CSelectStandFamilyAndTakeoffPositionDlg::GetCurStandGroupNamePos(HTREEITEM hTreeItem)
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

void CSelectStandFamilyAndTakeoffPositionDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here

	m_strRunwayExits = "";
	m_strStandGroups = "";
	m_vExitSelID.clear();
	m_vStandSelID.clear();

	if(!m_bAllRunwayExit)
	{
		//int nSelCount = m_TreeCtrlRunwayExit.GetSelectedCount();

		HTREEITEM hSelItem = m_TreeCtrlRunwayExit.GetSelectedItem();


	//	for (int i = 0; i < nSelCount;++i) 
		{		

			if (hSelItem != NULL )
			{
				RUNWAYEXITTYPE type = GetRunwayExitType(hSelItem);
				if (type == RUNWAYEXIT_DEFAULT)
				{
					//hSelItem = m_TreeCtrlRunwayExit.GetNextSelectedItem(hSelItem);
					//continue;
				}

				if (type == RUNWAYEXIT_LOGICRUNWAY)
				{
					if (m_TreeCtrlRunwayExit.ItemHasChildren(hSelItem))
					{
						HTREEITEM hItem = m_TreeCtrlRunwayExit.GetChildItem(hSelItem);

						while (hItem != NULL)
						{
							m_vExitSelID.push_back((int)(m_TreeCtrlRunwayExit.GetItemData(hItem)));

							CString strName = m_TreeCtrlRunwayExit.GetItemText(hItem);
							if (m_strRunwayExits =="")
							{
								m_strRunwayExits = strName;
							}
							else
							{
								m_strRunwayExits += ", ";
								m_strRunwayExits += strName;
							}

							hItem = m_TreeCtrlRunwayExit.GetNextSiblingItem(hItem);
						}
					}

				}
				else
				{
					m_vExitSelID.push_back((int)(m_TreeCtrlRunwayExit.GetItemData(hSelItem)) );

					CString strName = m_TreeCtrlRunwayExit.GetItemText(hSelItem);
					if (m_strRunwayExits =="")
					{
						m_strRunwayExits = strName;
					}
					else
					{
						m_strRunwayExits += ", ";
						m_strRunwayExits += strName;
					}
				}

			}
		//	hSelItem = m_TreeCtrlRunwayExit.GetNextSelectedItem(hSelItem);
		}
	}
	else
		m_strRunwayExits = "All";

	if(!m_bAllStand)
	{
	//	int nSelCount = m_TreeCtrlStandFamily.GetSelectedCount();

		HTREEITEM hSelItem = m_TreeCtrlStandFamily.GetSelectedItem();

	//	for (int i = 0; i < nSelCount;++i) 
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
		//	hSelItem = m_TreeCtrlStandFamily.GetNextSelectedItem(hSelItem);
		}
	}
	else
		m_strStandGroups = "All";

	if (!m_strStandGroups.Compare(""))
	{
		MessageBox("Please select stand family!", "Error", MB_OK|MB_ICONEXCLAMATION );
		return;
	}

	if (!m_strRunwayExits.Compare(""))
	{
		MessageBox("Please select takeoff position or exit!", "Error", MB_OK|MB_ICONEXCLAMATION );
		return;
	}
	OnOK();
}

bool CSelectStandFamilyAndTakeoffPositionDlg::IsExitSelected( const CString& strExitName )
{
	if (m_strRunwayExits.Find(strExitName) >= 0)
		return true;

	return false;
}

bool CSelectStandFamilyAndTakeoffPositionDlg::IsStandSelected( const CString& strStandName )
{
	CString strName = strStandName + ",";
	if (m_strStandGroups.Find(strName) >= 0)
		return true;

	return false;
	
}