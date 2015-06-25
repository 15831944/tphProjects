// RunwayExitAndStandFamilySelectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "RunwayExitAndStandFamilySelectDlg.h"
#include "InputAirside/InputAirside.h"
#include "../InputAirside/ALTAirport.h"
#include "../InputAirside/Runway.h"
#include "InputAirside/stand.h"
#include <iostream>
#include ".\runwayexitandstandfamilyselectdlg.h"
#include "InputAirside\ALTObjectGroup.h"
#include "InputAirside\Taxiway.h"

using namespace std;


// CRunwayExitAndStandFamilySelectDlg dialog

IMPLEMENT_DYNAMIC(CRunwayExitAndStandFamilySelectDlg, CSelectStandFamilyAndTakeoffPositionDlg)
CRunwayExitAndStandFamilySelectDlg::CRunwayExitAndStandFamilySelectDlg(const UINT nID,int nProjID,const CString& strSelExits, const CString& strSelStands, CWnd* pParent /*=NULL*/)
	: CSelectStandFamilyAndTakeoffPositionDlg(nID,nProjID,strSelExits,strSelStands, pParent)
{

}

CRunwayExitAndStandFamilySelectDlg::~CRunwayExitAndStandFamilySelectDlg()
{

}

//void CRunwayExitAndStandFamilySelectDlg::DoDataExchange(CDataExchange* pDX)
//{
//	CDialog::DoDataExchange(pDX);
//	DDX_Control(pDX, IDC_TREE_RUNWAYEXIT, m_TreeCtrlRunwayExit);
//	DDX_Control(pDX, IDC_TREE_STANDFAMILY, m_TreeCtrlStandFamily);
//}


//BEGIN_MESSAGE_MAP(CRunwayExitAndStandFamilySelectDlg, CDialog)
//	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_RUNWAYEXIT, OnTvnSelchangedTreeRunwayExit)
//	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_STANDFAMILY, OnTvnSelchangedTreeStandFamily)
//	ON_BN_CLICKED(IDOK, OnBnClickedOk)
//END_MESSAGE_MAP()
//
//BOOL CRunwayExitAndStandFamilySelectDlg::OnInitDialog()
//{
//	CDialog::OnInitDialog();
//
//	SetRunwayExitTreeContent();
//	SetStandFamilyTreeContent();
//
//	return TRUE;
//}
//
//// CRunwayExitAndStandFamilySelectDlg message handlers
//void CRunwayExitAndStandFamilySelectDlg::SetRunwayExitTreeContent(void)
//{
//	std::vector<int> vAirportIds;
//	InputAirside::GetAirportList(m_nProjID, vAirportIds);
//	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
//	{
//		ALTAirport airport;
//		airport.ReadAirport(*iterAirportID);
//
//		HTREEITEM hAirport = m_TreeCtrlRunwayExit.InsertItem(airport.getName());
//		m_TreeCtrlRunwayExit.SetItemData(hAirport, *iterAirportID);
//
//		std::vector<int> vRunways;
//		ALTAirport::GetRunwaysIDs(*iterAirportID, vRunways);
//		for (std::vector<int>::iterator iterRunwayID = vRunways.begin(); iterRunwayID != vRunways.end(); ++iterRunwayID)
//		{
//			Runway runway;
//			runway.ReadObject(*iterRunwayID);
//
//			RunwayExitList runwayExitList;
//			int nRunwayExitCount = runway.GetExitList(RUNWAYMARK_FIRST, runwayExitList);
//
//			//if the runway haven't exit
//			if (runwayExitList.size() < 1)
//			{
//				continue;
//			}
//
//			HTREEITEM hRunwayItem = m_TreeCtrlRunwayExit.InsertItem(runway.GetObjNameString(), hAirport);
//			m_TreeCtrlRunwayExit.SetItemData(hRunwayItem, *iterRunwayID);
//
//			//marking 1
//			HTREEITEM hMarking1Item = m_TreeCtrlRunwayExit.InsertItem(runway.GetMarking1().c_str(), hRunwayItem);
//			m_TreeCtrlRunwayExit.SetItemData(hMarking1Item, (int)RUNWAYMARK_FIRST);
//
//			vector<RunwayExit>::const_iterator citer;
//			CString runwayExitName;
//			for (citer=runwayExitList.begin(); citer!=runwayExitList.end(); citer++)
//			{
//				RunwayExit runwayExit = *citer;
//
//				//runwayExitName = runwayExit.m_strName;	
//				GetRunwayExitName(&runwayExit, &runwayExitList, runwayExitName);
//
//				HTREEITEM hPositionItem = m_TreeCtrlRunwayExit.InsertItem(runwayExitName, hMarking1Item);
//				m_TreeCtrlRunwayExit.SetItemData(hPositionItem, (DWORD_PTR)runwayExit.GetID());
//			}
//			m_TreeCtrlRunwayExit.Expand(hMarking1Item, TVE_EXPAND);
//
//			//marking 2
//			HTREEITEM hMarking2Item = m_TreeCtrlRunwayExit.InsertItem(runway.GetMarking2().c_str(), hRunwayItem);
//			m_TreeCtrlRunwayExit.SetItemData(hMarking2Item, (int)RUNWAYMARK_SECOND);
//
//			RunwayExitList runwayExitListMarkTwo;
//			nRunwayExitCount = runway.GetExitList(RUNWAYMARK_SECOND, runwayExitListMarkTwo);
//			for (citer=runwayExitListMarkTwo.begin(); citer!=runwayExitListMarkTwo.end(); citer++)
//			{
//				RunwayExit runwayExit = *citer;
//
//				//runwayExitName = runwayExit.m_strName;	
//				GetRunwayExitName(&runwayExit, &runwayExitListMarkTwo, runwayExitName);
//
//				HTREEITEM hPositionItem = m_TreeCtrlRunwayExit.InsertItem(runwayExitName, hMarking2Item);
//				m_TreeCtrlRunwayExit.SetItemData(hPositionItem, (DWORD_PTR)runwayExit.GetID());
//			}
//			m_TreeCtrlRunwayExit.Expand(hMarking2Item, TVE_EXPAND);
//
//			m_TreeCtrlRunwayExit.Expand(hRunwayItem, TVE_EXPAND);
//
//		}
//
//		m_TreeCtrlRunwayExit.Expand(hAirport, TVE_EXPAND);
//	}
//}
//
//void CRunwayExitAndStandFamilySelectDlg::GetRunwayExitName(RunwayExit *pRunwayExit, RunwayExitList *pRunwayExitList, CString& strRunwayExitName)
//{
//	ASSERT(pRunwayExit);
//	ASSERT(pRunwayExitList);
//
//	int nTaxiwayID = pRunwayExit->GetTaxiwayID();
//	Taxiway taxiway;
//	taxiway.ReadObject(nTaxiwayID);
//
//	BOOL bDisplayLeftRight = FALSE;
//	BOOL bDisplayIndex     = FALSE;
//
//	RunwayExitList::iterator iter = pRunwayExitList->begin();
//	for (; iter!=pRunwayExitList->end(); iter++)
//	{
//		RunwayExit *pOtherRunwayExit = &(*iter);
//		ASSERT(pOtherRunwayExit);
//
//		//skip self
//		if (pOtherRunwayExit->GetID() == pRunwayExit->GetID())
//		{
//			continue;
//		}
//
//		//have more intersect node with a taxiway
//		int nOtherTaxiwayID = pOtherRunwayExit->GetTaxiwayID();
//		int nTaxiwayID = pRunwayExit->GetTaxiwayID();
//		if (nOtherTaxiwayID == nTaxiwayID)
//		{
//			bDisplayLeftRight = TRUE;
//
//			if (pOtherRunwayExit->GetSideType() == pRunwayExit->GetSideType())
//			{
//				bDisplayIndex = TRUE;
//			}
//		}
//	}
//
//	CString strTaxiwayMark;
//	strTaxiwayMark.Format(_T("%s"), taxiway.GetMarking().c_str());
//
//	if (bDisplayLeftRight)
//	{
//		if (pRunwayExit->GetSideType() == RunwayExit::EXIT_LEFT)
//		{
//			strTaxiwayMark += "&LEFT";
//		}
//		else
//		{
//			strTaxiwayMark += "&RIGHT";
//		}
//
//		if (bDisplayIndex)
//		{
//			CString strIndex;
//			strIndex.Format(_T("%d"), pRunwayExit->GetIntesectionNode().GetIndex());
//			strTaxiwayMark += "&";
//			strTaxiwayMark += strIndex;
//		}
//	}
//
//	strRunwayExitName.Format(_T("%s"), strTaxiwayMark);
//}
//
//void CRunwayExitAndStandFamilySelectDlg::SetStandFamilyTreeContent(void)
//{
//	std::vector<int> vAirportIds;
//	InputAirside::GetAirportList(m_nProjID, vAirportIds);
//	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
//	{
//		ALTAirport airport;
//		airport.ReadAirport(*iterAirportID);
//
//		HTREEITEM hAirport = m_TreeCtrlStandFamily.InsertItem(airport.getName());
//		m_TreeCtrlRunwayExit.SetItemData(hAirport, *iterAirportID);
//
//		//std::vector<int> vStands;
//		//ALTAirport::GetStandsIDs(*iterAirportID, vStands);
//		//for (std::vector<int>::iterator iterStandID = vStands.begin(); iterStandID != vStands.end(); ++iterStandID)
//		//{
//		//	Stand stand;
//		//	stand.ReadObject(*iterStandID);
//
//		//	AddObjectToTree(hAirport, &stand);	
//		//}
//		ALTObjectList vStands;
//		ALTAirport::GetStandList(*iterAirportID,vStands);
//
//		for(int i = 0;i< (int)vStands.size(); ++i)
//		{
//			Stand * pStand =(Stand*) vStands.at(i).get();
//			AddObjectToTree(hAirport,pStand);
//		}
//
//		m_TreeCtrlStandFamily.Expand(hAirport, TVE_EXPAND);
//	}
//}
//
//void CRunwayExitAndStandFamilySelectDlg::AddObjectToTree(HTREEITEM hObjRoot,ALTObject* pObject)
//{
//	ASSERT(hObjRoot);
//
//	ALTObjectID objName;
//	pObject->getObjName(objName);
//	HTREEITEM hParentItem = hObjRoot;
//	bool bObjNode = false;
//	CString strNodeName = _T("");
//
//	for (int nLevel =0; nLevel< OBJECT_STRING_LEVEL; ++nLevel)
//	{
//		if (nLevel != OBJECT_STRING_LEVEL -1 )
//		{
//			HTREEITEM hItem = FindObjNode(hParentItem,objName.m_val[nLevel].c_str());
//			if (hItem != NULL)
//			{
//				hParentItem = hItem;
//				continue;
//			}
//			if (objName.m_val[nLevel+1] != _T(""))
//			{
//				HTREEITEM hTreeItem = m_TreeCtrlStandFamily.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
//				m_TreeCtrlStandFamily.Expand(hParentItem, TVE_EXPAND);
//				hParentItem = hTreeItem;
//
//				continue;
//			}
//			else //objNode
//			{
//				HTREEITEM hTreeItem = m_TreeCtrlStandFamily.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
//				m_TreeCtrlStandFamily.Expand(hParentItem, TVE_EXPAND);
//				break;
//			}
//
//		}
//		else
//		{
//			HTREEITEM hTreeItem = m_TreeCtrlStandFamily.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
//			m_TreeCtrlStandFamily.Expand(hParentItem, TVE_EXPAND);
//			break;
//		}
//	}
//}
//HTREEITEM CRunwayExitAndStandFamilySelectDlg::FindObjNode(HTREEITEM hParentItem,const CString& strNodeText)
//{
//	HTREEITEM hRetItem = NULL;
//
//	HTREEITEM hChildItem = m_TreeCtrlStandFamily.GetChildItem(hParentItem);
//
//	//find
//	while (hChildItem)
//	{
//		CString strItemText = m_TreeCtrlStandFamily.GetItemText(hChildItem);
//		if(strItemText.CompareNoCase(strNodeText) == 0)
//		{
//			hRetItem = hChildItem;
//			break;
//		}
//		hChildItem = m_TreeCtrlStandFamily.GetNextSiblingItem(hChildItem);
//	}
//	return hRetItem;
//}
//
//RUNWAYEXITTYPE CRunwayExitAndStandFamilySelectDlg::GetRunwayExitType(HTREEITEM hRunwayTreeItem)
//{
//	if (NULL == hRunwayTreeItem)
//	{
//		return RUNWAYEXIT_DEFAULT;
//	}
//
//	HTREEITEM hParent = m_TreeCtrlRunwayExit.GetParentItem(hRunwayTreeItem);
//
//	if (NULL == hParent)
//	{
//		return RUNWAYEXIT_DEFAULT;
//	}
//	else
//	{
//		HTREEITEM hParentParent = m_TreeCtrlRunwayExit.GetParentItem(hParent);
//
//		if (NULL == hParentParent)
//		{
//			return RUNWAYEXIT_ALL;
//		}
//		else
//		{
//			HTREEITEM hParentParentParent = m_TreeCtrlRunwayExit.GetParentItem(hParentParent);
//
//			if (NULL == hParentParentParent)
//			{
//				return RUNWAYEXIT_DEFAULT;
//			}
//			else
//			{
//				HTREEITEM hParentParentParentParent = m_TreeCtrlRunwayExit.GetParentItem(hParentParentParent);
//
//				if (NULL == hParentParentParentParent)
//				{
//					return RUNWAYEXIT_ITEM;
//				}
//				else
//				{
//					return RUNWAYEXIT_DEFAULT;
//				}
//
//			}
//		}
//	}
//
//	return RUNWAYEXIT_DEFAULT;
//}
//
//void CRunwayExitAndStandFamilySelectDlg::OnTvnSelchangedTreeRunwayExit(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
//	// TODO: Add your control notification handler code here
//	HTREEITEM hItem = m_TreeCtrlRunwayExit.GetSelectedItem();
//
//	switch(GetRunwayExitType(hItem))
//	{
//	case RUNWAYEXIT_ALL:
//		{
//			m_nSelRunwayID = (int)m_TreeCtrlRunwayExit.GetItemData(hItem);
//			m_bRunwayAll = TRUE;
//
//			Runway runway;
//			runway.ReadObject(m_nSelRunwayID);
//
//			m_strRunwayExit.Format("%s ALL", runway.GetObjNameString());
//		}
//		break;
//
//	case RUNWAYEXIT_ITEM:
//		{
//			m_nSelRunwayExitID = (int)m_TreeCtrlRunwayExit.GetItemData(hItem);
//			m_bRunwayAll = FALSE;
//			CString strExitName = m_TreeCtrlRunwayExit.GetItemText(hItem);
//
//			HTREEITEM hParent = m_TreeCtrlRunwayExit.GetParentItem(hItem);
//			m_nSelRunwayMarking = (RUNWAY_MARK)m_TreeCtrlRunwayExit.GetItemData(hParent);
//			CString strRunwayMarking = m_TreeCtrlRunwayExit.GetItemText(hParent);
//
//			hParent = m_TreeCtrlRunwayExit.GetParentItem(hParent);
//			m_nSelRunwayID = (int)m_TreeCtrlRunwayExit.GetItemData(hParent);
//
//			//Runway runway;
//			//runway.ReadObject(m_nSelRunwayID);
//
//			m_strRunwayExit.Format("%s&%s", strRunwayMarking, strExitName);
//			//RunwayExitList runwayExitList;
//			//int nRunwayExitCount;
//
//			//if (m_nSelRunwayMarking==RUNWAYMARK_FIRST)
//			//{
//			//	strRunwayMarkingName.Format("%s", runway.GetMarking1().c_str());
//			//	nRunwayExitCount = runway.GetExitList(RUNWAYMARK_FIRST, runwayExitList);
//			//}
//			//else
//			//{
//			//	strRunwayMarkingName.Format("%s", runway.GetMarking2().c_str());
//			//	nRunwayExitCount = runway.GetExitList(RUNWAYMARK_SECOND, runwayExitList);
//			//}
//
//			//CString runwayExitName;
//			//vector<RunwayExit>::const_iterator citer;
//			//for (citer=runwayExitList.begin(); citer!=runwayExitList.end(); citer++)
//			//{
//			//	RunwayExit runwayExit = *citer;
//
//			//	if (runwayExit.GetRunwayExitID() == m_nSelRunwayExitID)
//			//	{
//			//		runwayExitName = runwayExit.GetRunwayExitName();
//
//			//		break;
//			//	}				
//			//}
//
//
//			//m_strRunwayExit.Format("%s %s", strRunwayMarkingName, runwayExitName);
//		}
//		break;
//
//	case RUNWAYEXIT_DEFAULT:
//		{
//			m_strRunwayExit = "";
//			m_bRunwayAll = FALSE;
//		}
//		break;
//
//	default:
//		{
//			m_strRunwayExit = "";
//			m_bRunwayAll = FALSE;
//		}
//		break;
//	}
//
//	UpdateData(FALSE);
//
//	*pResult = 0;
//}
//
//void CRunwayExitAndStandFamilySelectDlg::OnTvnSelchangedTreeStandFamily(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
//	// TODO: Add your control notification handler code here
//	HTREEITEM hItem = m_TreeCtrlStandFamily.GetSelectedItem();
//
//	ALTObjectID objName;
//
//	switch(GetCurStandGroupNamePos(hItem))
//	{
//	case 0:
//		{
//			objName.at(0) = m_TreeCtrlStandFamily.GetItemText(hItem);
//
//			m_strStandFamily.Format("%s", objName.at(0).c_str());
//		}
//		break;
//
//	case 1:
//		{
//			objName.at(1) = m_TreeCtrlStandFamily.GetItemText(hItem);
//			hItem = m_TreeCtrlStandFamily.GetParentItem(hItem);
//			objName.at(0) = m_TreeCtrlStandFamily.GetItemText(hItem);
//			objName.at(2) = "";
//			objName.at(3) = "";
//
//            m_strStandFamily.Format("%s-%s", objName.at(0).c_str(), objName.at(1).c_str());
//		}
//		break;
//
//	case 2:
//		{
//			objName.at(2) = m_TreeCtrlStandFamily.GetItemText(hItem);
//			hItem = m_TreeCtrlStandFamily.GetParentItem(hItem);
//			objName.at(1) = m_TreeCtrlStandFamily.GetItemText(hItem);
//			hItem = m_TreeCtrlStandFamily.GetParentItem(hItem);
//			objName.at(0) = m_TreeCtrlStandFamily.GetItemText(hItem);
//			objName.at(3) = "";
//
//			m_strStandFamily.Format("%s-%s-%s", objName.at(0).c_str(), objName.at(1).c_str(), objName.at(2).c_str());
//		}
//		break;
//
//	case 3:
//		{
//			objName.at(3) = m_TreeCtrlStandFamily.GetItemText(hItem);
//			hItem = m_TreeCtrlStandFamily.GetParentItem(hItem);
//			objName.at(2) = m_TreeCtrlStandFamily.GetItemText(hItem);
//			hItem = m_TreeCtrlStandFamily.GetParentItem(hItem);
//			objName.at(1) = m_TreeCtrlStandFamily.GetItemText(hItem);
//			hItem = m_TreeCtrlStandFamily.GetParentItem(hItem);
//			objName.at(0) = m_TreeCtrlStandFamily.GetItemText(hItem);
//
//			m_strStandFamily.Format("%s-%s-%s-%s", objName.at(0).c_str(), objName.at(1).c_str(), objName.at(2).c_str(), objName.at(3).c_str());
//		}
//		break;
//
//	default:
//		m_strStandFamily = "";
//		break;
//	}
//
//	m_pSelStand->setObjName(objName);
//	
//	UpdateData(FALSE);
//
//	*pResult = 0;
//}
//
//int CRunwayExitAndStandFamilySelectDlg::GetCurStandGroupNamePos(HTREEITEM hTreeItem)
//{
//	if (NULL == hTreeItem)
//	{
//		return -1;
//	}
//
//	HTREEITEM hParentItem = m_TreeCtrlStandFamily.GetParentItem(hTreeItem);
//
//	//root item
//	if (NULL == hParentItem)
//	{
//		return -1;
//	}
//
//	hParentItem = m_TreeCtrlStandFamily.GetParentItem(hParentItem);
//
//	if (NULL == hParentItem)
//	{
//		return 0;
//	}
//
//	hParentItem = m_TreeCtrlStandFamily.GetParentItem(hParentItem);
//
//	if (NULL == hParentItem)
//	{
//		return 1;
//	}
//
//	hParentItem = m_TreeCtrlStandFamily.GetParentItem(hParentItem);
//
//	if (NULL == hParentItem)
//	{
//		return 2;
//	}
//
//	hParentItem = m_TreeCtrlStandFamily.GetParentItem(hParentItem);
//
//	if (NULL == hParentItem)
//	{
//		return 3;
//	}
//
//	return -1;
//}
//void CRunwayExitAndStandFamilySelectDlg::OnBnClickedOk()
//{
//	// TODO: Add your control notification handler code here
//	if (!m_strRunwayExit.Compare(""))
//	{
//		MessageBox("Please select runway exit!", "Error", MB_OK);
//		return;
//	}
//
//    if (!m_strStandFamily.Compare(""))
//    {
//		MessageBox("Please select stand family!", "Error", MB_OK);
//		return;
//    }
//
//	ALTObjectGroup altObjGroup;
//	altObjGroup.setType(m_pSelStand->GetType());
//	altObjGroup.setName(m_pSelStand->GetObjectName());
//
//	try
//	{
//		CADODatabase::BeginTransaction();
//		m_nSelStandFamilyID = altObjGroup.InsertData(m_nProjID);
//		CADODatabase::CommitTransaction();
//
//	}
//	catch (CADOException &e)
//	{
//		CADODatabase::RollBackTransation();
//		e.ErrorMessage();
//	}
//
//	OnOK();
//}
//
//void CRunwayExitAndStandFamilySelectDlg::SetSelRunwayID(int nRunwayID)
//{
//	m_nSelRunwayID = nRunwayID;
//}
//
//int CRunwayExitAndStandFamilySelectDlg::GetSelRunwayID()
//{
//	return m_nSelRunwayID;
//}
//
//void CRunwayExitAndStandFamilySelectDlg::SetSelRunwayMarking(RUNWAY_MARK runwayMarking)
//{
//	m_nSelRunwayMarking = runwayMarking;
//}
//
//RUNWAY_MARK CRunwayExitAndStandFamilySelectDlg::GetSelRunwayMarking()
//{
//	return m_nSelRunwayMarking;
//}
//
//void CRunwayExitAndStandFamilySelectDlg::SetRunwayExitID(int nRunwayExitID)
//{
//	m_nSelRunwayExitID = nRunwayExitID;
//}
//
//int CRunwayExitAndStandFamilySelectDlg::GetSelRunwayExitID()
//{
//	return m_nSelRunwayExitID;
//}
//
//void CRunwayExitAndStandFamilySelectDlg::SetSelStandFamilyID(int nStandFamilyID)
//{
//	m_nSelStandFamilyID = nStandFamilyID;
//}
//
//int CRunwayExitAndStandFamilySelectDlg::GetSelStandFamilyID()
//{
//	return m_nSelStandFamilyID;
//}
//
//void CRunwayExitAndStandFamilySelectDlg::SetRunwayAllExitFlag(BOOL bRunwayAllExitFlag)
//{
//	m_bRunwayAll = bRunwayAllExitFlag;
//}
//
//BOOL CRunwayExitAndStandFamilySelectDlg::GetRunwayAllExitFlag()
//{
//	return m_bRunwayAll;
//}
//
//void CRunwayExitAndStandFamilySelectDlg::SetRunwayExitName(CString& strRunwayExitName)
//{
//	m_strRunwayExit = strRunwayExitName;
//}
//
//CString CRunwayExitAndStandFamilySelectDlg::GetRunwayExitName()
//{
//	return m_strRunwayExit;
//}
//
//void CRunwayExitAndStandFamilySelectDlg::SetStandFamilyName(CString& strStandFamilyName)
//{
//	m_strStandFamily = strStandFamilyName;
//}
//
//CString CRunwayExitAndStandFamilySelectDlg::GetStandFamilyName()
//{
//	return m_strStandFamily;
//}
//
//void CRunwayExitAndStandFamilySelectDlg::SetStand(int nStandGroupID)
//{
//	ALTObjectGroup altObjGroup;
//	altObjGroup.ReadData(nStandGroupID);	
//
//	m_pSelStand->setObjName(altObjGroup.getName());
//}