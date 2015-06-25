// SelectStandFamilyAndTakeoffPositionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgSelectAirsideOrigDest.h"
#include "InputAirside/InputAirside.h"
#include "../InputAirside/ALTAirport.h"
#include "../InputAirside/Runway.h"
#include "InputAirside/stand.h"
#include <iostream>
#include ".\runwayexitandstandfamilyselectdlg.h"
#include "InputAirside\ALTObjectGroup.h"
#include "InputAirside\Taxiway.h"

#include "InputAirside/TaxiRoute.h"
#include ".\dlgselectairsideorigdest.h"

static const int IDC_VERTICAL_SPLITTER_BAR = 30;
// CDlgSelectAirsideOrigDest dialog

IMPLEMENT_DYNAMIC(CDlgSelectAirsideOrigDest, CXTResizeDialogEx)

CDlgSelectAirsideOrigDest::CDlgSelectAirsideOrigDest(const UINT nID,int nProjID, const CString& strSelExits,  const CString& strSelStands,CWnd* pParent /*=NULL*/)
	: CXTResizeDialogEx(CXTResizeDialogEx::styleUseInitSizeAsMinSize, nID, pParent)
	, m_nProjID(nProjID)
	, m_bAllStand(FALSE)
	, m_bAllDeice(FALSE)
	, m_bAllStartPosition(FALSE)
	, m_bAllTaxiInterruptLine(FALSE)
	, m_bAllRunwayExit(FALSE)
	, m_hStands(NULL)
	, m_hDeicepads(NULL)
	, m_hStartPositions(NULL)
	, m_hTaxiInterruptLines(NULL)
	, m_hAllStands(NULL)
	, m_hAllDeicepads(NULL)
	, m_hAllStartPositions(NULL)
	, m_hAllTaxiInterruptLines(NULL)
	, m_hAllRunwayExit(NULL)
{
	m_strRunwayExits = strSelExits;
	m_strOriginName = strSelStands + ",";

	m_taxiInterruptLines.ReadData(m_nProjID);
}

CDlgSelectAirsideOrigDest::~CDlgSelectAirsideOrigDest()
{
	
}

void CDlgSelectAirsideOrigDest::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_RUNWAYEXIT, m_treelRunwayExit);
	DDX_Control(pDX, IDC_TREE_STANDFAMILY, m_treeOrg);

}


BEGIN_MESSAGE_MAP(CDlgSelectAirsideOrigDest, CXTResizeDialogEx)	
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgSelectAirsideOrigDest message handlers
BOOL CDlgSelectAirsideOrigDest::OnInitDialog()
{
	CXTResizeDialogEx::OnInitDialog();
	SetWindowText(_T("Select Origin and Destination"));
	GetDlgItem(IDC_STATIC_ORG)->SetWindowText(_T("From"));
	GetDlgItem(IDC_STATIC_DST)->SetWindowText(_T("To"));

	//m_treelRunwayExit.EnableMultiSelect();
	//m_treeOrg.EnableMultiSelect();

	m_treelRunwayExit.ModifyStyle(0,TVS_SHOWSELALWAYS);
	m_treeOrg.ModifyStyle(0,TVS_SHOWSELALWAYS);

	SetRunwayExitTreeContent();
	SetOriginTreeContent();

	m_strOriginName = "";

	m_wndSplitterVer.SetStyle(SPS_VERTICAL);
	m_wndSplitterVer.SetBuddies(GetDlgItem(IDC_STATIC_ORG), GetDlgItem(IDC_STATIC_DST));

	UpdateResize();
	UpdateSplitterRange();

	return TRUE;
}

CString CDlgSelectAirsideOrigDest::GetRunwayExitName()
{
	return m_strRunwayExits;
}

CString CDlgSelectAirsideOrigDest::GetOriginName()
{
	return m_strOriginName;
}

void CDlgSelectAirsideOrigDest::SetRunwayExitTreeContent(void)
{
	m_hAllRunwayExit = m_treelRunwayExit.InsertItem("All");
	m_treelRunwayExit.SetItemData(m_hAllRunwayExit, -2);

	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjID, vAirportIds);
	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	{
		ALTAirport airport;
		airport.ReadAirport(*iterAirportID);

		HTREEITEM hAirport = m_treelRunwayExit.InsertItem(airport.getName(), m_hAllRunwayExit);
		m_treelRunwayExit.SetItemData(hAirport, *iterAirportID);

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

			HTREEITEM hRunwayItem = m_treelRunwayExit.InsertItem(runway.GetObjNameString(), hAirport);
			m_treelRunwayExit.SetItemData(hRunwayItem, *iterRunwayID);

			//marking 1
			HTREEITEM hMarking1Item = m_treelRunwayExit.InsertItem(runway.GetMarking1().c_str(), hRunwayItem);
			m_treelRunwayExit.SetItemData(hMarking1Item, (int)RUNWAYMARK_FIRST);

			std::vector<RunwayExit>::iterator citer;
			CString runwayExitName;
			for (citer=runwayExitList.begin(); citer!=runwayExitList.end(); citer++)
			{
				RunwayExit runwayExit = *citer;
				//runwayExitName = runwayExit.GetName();
				GetRunwayExitNameString(&runwayExit, &runwayExitList, runwayExitName);

				HTREEITEM hPositionItem = m_treelRunwayExit.InsertItem(runwayExitName, hMarking1Item);
				m_treelRunwayExit.SetItemData(hPositionItem, (DWORD_PTR)runwayExit.GetID());

				if (IsExitSelected(runwayExitName))
					m_treelRunwayExit.SelectItem(hPositionItem);				
			}
			m_treelRunwayExit.Expand(hMarking1Item, TVE_EXPAND);

			//marking 2
			HTREEITEM hMarking2Item = m_treelRunwayExit.InsertItem(runway.GetMarking2().c_str(), hRunwayItem);
			m_treelRunwayExit.SetItemData(hMarking2Item, (int)RUNWAYMARK_SECOND);

			RunwayExitList runwayExitListMarkTwo;
			nRunwayExitCount = runway.GetExitList(RUNWAYMARK_SECOND, runwayExitListMarkTwo);
			for (citer=runwayExitListMarkTwo.begin(); citer!=runwayExitListMarkTwo.end(); citer++)
			{
				RunwayExit runwayExit = *citer;

				//runwayExitName = runwayExit.m_strName;	
				GetRunwayExitNameString(&runwayExit, &runwayExitListMarkTwo, runwayExitName);
				//runwayExitName = runwayExit.GetName();

				HTREEITEM hPositionItem = m_treelRunwayExit.InsertItem(runwayExitName, hMarking2Item);
				m_treelRunwayExit.SetItemData(hPositionItem, (DWORD_PTR)runwayExit.GetID());

				if (IsExitSelected(runwayExitName))
					m_treelRunwayExit.SelectItem(hPositionItem);
			}
			m_treelRunwayExit.Expand(hMarking2Item, TVE_EXPAND);
			m_treelRunwayExit.Expand(hRunwayItem, TVE_EXPAND);

		}

		m_treelRunwayExit.Expand(hAirport, TVE_EXPAND);
	}
	m_treelRunwayExit.Expand(m_hAllRunwayExit, TVE_EXPAND);
}

void CDlgSelectAirsideOrigDest::GetRunwayExitNameString(RunwayExit *pRunwayExit, RunwayExitList *pRunwayExitList, CString& strRunwayExitName)
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

void CDlgSelectAirsideOrigDest::SetOriginTreeContent(void)
{
	m_hStands = m_treeOrg.InsertItem(_T("Stands:"));
	m_hAllStands = m_treeOrg.InsertItem(_T("All"), m_hStands);
	m_treeOrg.SetItemData(m_hAllStands, -2);

	m_hDeicepads = m_treeOrg.InsertItem(_T("Deicepads:"));
	m_hAllDeicepads = m_treeOrg.InsertItem(_T("All"), m_hDeicepads);
	m_treeOrg.SetItemData(m_hAllDeicepads, -2);

	m_hStartPositions = m_treeOrg.InsertItem(_T("Start Positions:"));
	m_hAllStartPositions = m_treeOrg.InsertItem(_T("All"), m_hStartPositions);
	m_treeOrg.SetItemData(m_hAllStartPositions, -2);

	m_hTaxiInterruptLines = m_treeOrg.InsertItem(_T("Taxi Interrupt Lines:"));
	m_hAllTaxiInterruptLines = m_treeOrg.InsertItem(_T("All"), m_hTaxiInterruptLines);
	m_treeOrg.SetItemData(m_hAllTaxiInterruptLines, -2);


	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjID, vAirportIds);
	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	{
		ALTAirport airport;
		airport.ReadAirport(*iterAirportID);

		ALTObjectList vStands;
		ALTAirport::GetStandList(*iterAirportID, vStands);
		AddAirportALTObjectList(airport, vStands, m_hStands);

		ALTObjectList vDeicepads;
		ALTAirport::GetDeicePadList(*iterAirportID, vDeicepads);
		AddAirportALTObjectList(airport, vDeicepads, m_hDeicepads);

		ALTObjectList vStartPositions;
		ALTAirport::GetStartPositionList(*iterAirportID, vStartPositions);
		AddAirportALTObjectList(airport, vStartPositions, m_hStartPositions);

		size_t nCount = m_taxiInterruptLines.GetElementCount();
		for(size_t i=0;i<nCount;i++)
		{
			CTaxiInterruptLine* pLine = m_taxiInterruptLines.GetItem(i);
			HTREEITEM hLineItem = m_treeOrg.InsertItem(pLine->GetName(), m_hTaxiInterruptLines);
			m_treeOrg.SetItemData(hLineItem, (DWORD)pLine);
		}
	}

	m_treeOrg.Expand(m_hStands, TVE_EXPAND);
	m_treeOrg.Expand(m_hDeicepads, TVE_EXPAND);
	m_treeOrg.Expand(m_hStartPositions, TVE_EXPAND);
	m_treeOrg.Expand(m_hTaxiInterruptLines, TVE_EXPAND);
}
void CDlgSelectAirsideOrigDest::AddAirportALTObjectList( const ALTAirport& airport, const ALTObjectList& vALTObjectList, HTREEITEM hParentItem )
{
	size_t nCount = vALTObjectList.size();
	if (nCount)
	{
		HTREEITEM hAirport = m_treeOrg.InsertItem(airport.getName(), hParentItem);
		m_treeOrg.SetItemData(hAirport, airport.GetAirportID());
	
		for(size_t i = 0;i< nCount; ++i)
		{
			AddObjectToTree(hAirport,vALTObjectList.at(i).get());
		}
	
		m_treeOrg.Expand(hAirport, TVE_EXPAND);
	}
}
void CDlgSelectAirsideOrigDest::AddObjectToTree(HTREEITEM hObjRoot, const ALTObject* pObject)
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
			HTREEITEM hItem = FindChildObjNode(hParentItem,objName.m_val[nLevel].c_str());
			if (hItem != NULL)
			{
				if (IsStandSelected(pObject->GetObjNameString(nLevel)))
					m_treeOrg.SelectItem(hItem);

				hParentItem = hItem;

				continue;
			}

			if (objName.m_val[nLevel+1] != _T(""))
			{
				hTreeItem = m_treeOrg.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
				//m_treeOrg.Expand(hParentItem, TVE_EXPAND);
				if (IsStandSelected(pObject->GetObjNameString(nLevel)))
					m_treeOrg.SelectItem(hTreeItem);

				hParentItem = hTreeItem;

				continue;
			}
			else //objNode
			{
				hTreeItem = m_treeOrg.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
				//m_treeOrg.Expand(hParentItem, TVE_EXPAND);
				break;
			}

		}
		else
		{
			hTreeItem = m_treeOrg.InsertItem(objName.m_val[nLevel].c_str(),hParentItem);
			//m_treeOrg.Expand(hParentItem, TVE_EXPAND);
			break;
		}
	}

	if (IsStandSelected(pObject->GetObjNameString()))
	{
		m_treeOrg.SelectItem(hTreeItem);
		m_treeOrg.Expand(hParentItem, TVE_EXPAND);
	}

}

HTREEITEM CDlgSelectAirsideOrigDest::FindChildObjNode(HTREEITEM hParentItem,const CString& strNodeText)
{
	HTREEITEM hRetItem = NULL;

	HTREEITEM hChildItem = m_treeOrg.GetChildItem(hParentItem);

	//find
	while (hChildItem)
	{
		CString strItemText = m_treeOrg.GetItemText(hChildItem);
		if(strItemText.CompareNoCase(strNodeText) == 0)
		{
			hRetItem = hChildItem;
			break;
		}
		hChildItem = m_treeOrg.GetNextSiblingItem(hChildItem);
	}
	return hRetItem;
}

CDlgSelectAirsideOrigDest::RUNWAYEXITTYPE CDlgSelectAirsideOrigDest::GetRunwayExitType(HTREEITEM hRunwayTreeItem)
{
	int nLevel = 0;
	while (hRunwayTreeItem)
	{
		hRunwayTreeItem = m_treelRunwayExit.GetParentItem(hRunwayTreeItem);
		nLevel++;
	}
	static const RUNWAYEXITTYPE exitTypeMap[] =
	{
		RUNWAYEXIT_DEFAULT,
		RUNWAYEXIT_ALLRUNWAY,
		RUNWAYEXIT_DEFAULT,
		RUNWAYEXIT_DEFAULT,
		RUNWAYEXIT_LOGICRUNWAY,
		RUNWAYEXIT_ITEM
	};
	if (nLevel<sizeof(exitTypeMap)/sizeof(exitTypeMap[0]))
	{
		return exitTypeMap[nLevel];
	}
	ASSERT(FALSE);
	return RUNWAYEXIT_DEFAULT;
}

void CDlgSelectAirsideOrigDest::GetALTObjectFamilyAltID(HTREEITEM hItem, ALTObjectID& objName, CString& strStandFamily)
{
	switch(GetCurALTObjectNamePos(hItem))
	{
	case 0:
		{
			objName.at(0) = m_treeOrg.GetItemText(hItem);

			strStandFamily.Format("%s", objName.at(0).c_str());
		}
		break;

	case 1:
		{
			objName.at(1) = m_treeOrg.GetItemText(hItem);
			hItem = m_treeOrg.GetParentItem(hItem);
			objName.at(0) = m_treeOrg.GetItemText(hItem);
			objName.at(2) = "";
			objName.at(3) = "";

			strStandFamily.Format("%s-%s", objName.at(0).c_str(), objName.at(1).c_str());
		}
		break;

	case 2:
		{
			objName.at(2) = m_treeOrg.GetItemText(hItem);
			hItem = m_treeOrg.GetParentItem(hItem);
			objName.at(1) = m_treeOrg.GetItemText(hItem);
			hItem = m_treeOrg.GetParentItem(hItem);
			objName.at(0) = m_treeOrg.GetItemText(hItem);
			objName.at(3) = "";

			strStandFamily.Format("%s-%s-%s", objName.at(0).c_str(), objName.at(1).c_str(), objName.at(2).c_str());
		}
		break;

	case 3:
		{
			objName.at(3) = m_treeOrg.GetItemText(hItem);
			hItem = m_treeOrg.GetParentItem(hItem);
			objName.at(2) = m_treeOrg.GetItemText(hItem);
			hItem = m_treeOrg.GetParentItem(hItem);
			objName.at(1) = m_treeOrg.GetItemText(hItem);
			hItem = m_treeOrg.GetParentItem(hItem);
			objName.at(0) = m_treeOrg.GetItemText(hItem);

			strStandFamily.Format("%s-%s-%s-%s", objName.at(0).c_str(), objName.at(1).c_str(), objName.at(2).c_str(), objName.at(3).c_str());
		}
		break;

	default:
		strStandFamily = "";
		break;
	}
}

int CDlgSelectAirsideOrigDest::GetCurALTObjectNamePos(HTREEITEM hTreeItem)
{
	int nLevel = 0;
	while (hTreeItem)
	{
		hTreeItem = m_treeOrg.GetParentItem(hTreeItem);
		nLevel++;
	}
	static const int posMap[] = { -1, -1, -1, 0, 1, 2, 3};
	if (nLevel<sizeof(posMap)/sizeof(posMap[0]))
	{
		return posMap[nLevel];
	}
	ASSERT(FALSE);
	return -1;
}

void CDlgSelectAirsideOrigDest::OnOK()
{
	// TODO: Add your control notification handler code here

	m_strRunwayExits = "";
	m_strOriginName = "";
	m_vExitSelID.clear();
	m_vStandSelID.clear();
	m_vDeiceGroupSelID.clear();
	m_vStartPositionSelID.clear();
	m_vTaxiInterruptSelID.clear();

	TreeSelectList orgList;
	//m_treeOrg.GetSelectedList(orgList);
	orgList.AddTail(m_treeOrg.GetSelectedItem());
	TreeSelectList stands;
	TreeSelectList deicepads;
	TreeSelectList startpositions;
	TreeSelectList taxiinterruptlines;
	PureOriginList(orgList, stands, deicepads, startpositions, taxiinterruptlines);

	BOOL bHasPrev = FALSE;
	bHasPrev = RecALTObject(bHasPrev, m_bAllStand, stands, m_vStandSelID, ALT_STAND, _T(" Stand: "));
	bHasPrev = RecALTObject(bHasPrev, m_bAllDeice, deicepads, m_vDeiceGroupSelID, ALT_DEICEBAY, _T(" Deicepad: "));
	bHasPrev = RecALTObject(bHasPrev, m_bAllStartPosition, startpositions, m_vStartPositionSelID, ALT_STARTPOSITION, _T(" Start Position: "));

	if(!m_bAllTaxiInterruptLine)
	{
		BOOL bFirst = TRUE;
		POSITION pos = taxiinterruptlines.GetHeadPosition();
		while (pos)
		{
			HTREEITEM hSelItem = taxiinterruptlines.GetNext(pos);
			CTaxiInterruptLine* pLine = (CTaxiInterruptLine*)m_treeOrg.GetItemData(hSelItem);
			m_vTaxiInterruptSelID.push_back(pLine->GetID());

			if (bFirst)
			{
				if (bHasPrev)
					m_strOriginName += ";";

				m_strOriginName += _T(" Taxi Interrupt Line: ");
				bHasPrev = TRUE;
				bFirst = FALSE;
			}
			else
				m_strOriginName += ", ";

			m_strOriginName += pLine->GetName();
		}
	}
	else
	{
		if (bHasPrev)
			m_strOriginName += _T(";");

		m_strOriginName += _T(" Taxi Interrupt Line: All");
		bHasPrev = TRUE;
	}


	if (!m_strOriginName.Compare(""))
	{
		MessageBox("Please select some origins!", "Error", MB_OK|MB_ICONEXCLAMATION );
		return;
	}

	//
	TreeSelectList dstList;
	//m_treelRunwayExit.GetSelectedList(dstList);
	dstList.AddTail(m_treelRunwayExit.GetSelectedItem());
	m_bAllRunwayExit = FALSE;
	POSITION pos = dstList.GetHeadPosition();
	while (pos)
	{
		HTREEITEM hItem = dstList.GetNext(pos);

		if (!m_bAllRunwayExit)
		{
			if (hItem == m_hAllRunwayExit)
			{
				dstList.RemoveAll();
				m_bAllRunwayExit = TRUE;
				m_strRunwayExits = _T("All");
				break;
			}
			RUNWAYEXITTYPE type = GetRunwayExitType(hItem);
			if (type == RUNWAYEXIT_LOGICRUNWAY)
			{
				HTREEITEM hItem2 = m_treelRunwayExit.GetChildItem(hItem);
				while (hItem2)
				{
					m_vExitSelID.push_back(m_treelRunwayExit.GetItemData(hItem2));

					if (!m_strRunwayExits.IsEmpty())
						m_strRunwayExits += ", ";
					m_strRunwayExits += m_treelRunwayExit.GetItemText(hItem2);

					hItem2 = m_treelRunwayExit.GetNextSiblingItem(hItem2);
				}

			}
			else if (type == RUNWAYEXIT_ITEM)
			{
				HTREEITEM hItemParent = m_treelRunwayExit.GetParentItem(hItem);
				if (dstList.Find(hItemParent))
					continue;

				m_vExitSelID.push_back(m_treelRunwayExit.GetItemData(hItem));

				if (!m_strRunwayExits.IsEmpty())
					m_strRunwayExits += ", ";
				m_strRunwayExits += m_treelRunwayExit.GetItemText(hItem);

			}

		}
	}

	if (!m_strRunwayExits.Compare(""))
	{
		MessageBox("Please select takeoff position or exit!", "Error", MB_OK|MB_ICONEXCLAMATION );
		return;
	}

	CXTResizeDialogEx::OnOK();
}

bool CDlgSelectAirsideOrigDest::IsExitSelected( const CString& strExitName )
{
	if (m_strRunwayExits.Find(strExitName) >= 0)
		return true;

	return false;
}

bool CDlgSelectAirsideOrigDest::IsStandSelected( const CString& strStandName )
{
	CString strName = strStandName + ",";
	if (m_strOriginName.Find(strName) >= 0)
		return true;

	return false;
	
}

void CDlgSelectAirsideOrigDest::UpdateResize()
{
	CXTResizeDialogEx::Init();

	SetResize(IDC_STATIC_ORG, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	SetResize(m_treeOrg.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_BOTTOM_CENTER);

	SetResize(m_wndSplitterVer.GetDlgCtrlID(), SZ_TOP_CENTER, SZ_BOTTOM_CENTER);

	SetResize(IDC_STATIC_DST, SZ_TOP_CENTER, SZ_BOTTOM_RIGHT);
	SetResize(m_treelRunwayExit.GetDlgCtrlID(), SZ_TOP_CENTER, SZ_BOTTOM_RIGHT);

	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
}


static BOOL IsTreeAncestor(CTreeCtrl& ctrl, HTREEITEM hAncestorItem, HTREEITEM hItem, int& nLevelDiff)
{
	nLevelDiff = 0;
	while (hItem)
	{
		hItem = ctrl.GetParentItem(hItem);
		nLevelDiff++;
		if (hItem == hAncestorItem)
			return TRUE;
	}
	return FALSE;
}

static BOOL PureALTObject( HTREEITEM hItem, BOOL& bAll, HTREEITEM hAllItem, HTREEITEM hItemBase,
						  CDlgSelectAirsideOrigDest::TreeSelectList& itemList, CTreeCtrl& treeCtrl )
{
	if (!bAll)
	{
		if (hItem == hItemBase)
			return TRUE;
		if (hItem == hAllItem)
		{
			itemList.RemoveAll();
			bAll = TRUE;
			return TRUE;
		}
		int nLevelDiff = 0;
		if (IsTreeAncestor(treeCtrl, hItemBase, hItem, nLevelDiff))
		{
			int nNameLevel = nLevelDiff - 2;
			if (nNameLevel>=0 && nNameLevel <4)
			{
				BOOL bToAdd = TRUE;
				POSITION pos2 = itemList.GetHeadPosition();
				while (pos2)
				{
					POSITION pos3 = pos2;
					HTREEITEM hItem2 = itemList.GetNext(pos2);
					if (IsTreeAncestor(treeCtrl, hItem2, hItem, nLevelDiff))
					{
						bToAdd = FALSE;
						break;
					}
					if (IsTreeAncestor(treeCtrl, hItem, hItem2, nLevelDiff))
						itemList.RemoveAt(pos3);
				}
				if (bToAdd)
					itemList.AddHead(hItem);
			}
			return TRUE;
		}
	}
	return FALSE;
}

void CDlgSelectAirsideOrigDest::PureOriginList( const TreeSelectList& orgList,
											   TreeSelectList& stands,
											   TreeSelectList& deicepads,
											   TreeSelectList& startpositions,
											   TreeSelectList& taxiinterruptlines
											   )
{
	m_bAllStand = FALSE;
	m_bAllDeice = FALSE;
	m_bAllStartPosition = FALSE;
	m_bAllTaxiInterruptLine = FALSE;

	POSITION pos = orgList.GetHeadPosition();
	while (pos)
	{
		HTREEITEM hItem = orgList.GetNext(pos);

		if (PureALTObject(hItem, m_bAllStand, m_hAllStands, m_hStands, stands, m_treeOrg))
			continue;
		if (PureALTObject(hItem, m_bAllDeice, m_hAllDeicepads, m_hDeicepads, deicepads, m_treeOrg))
			continue;
		if (PureALTObject(hItem, m_bAllStartPosition, m_hAllStartPositions, m_hStartPositions, startpositions, m_treeOrg))
			continue;

		if (!m_bAllTaxiInterruptLine)
		{
			if (hItem == m_hTaxiInterruptLines)
				continue;
			if (hItem == m_hAllTaxiInterruptLines)
			{
				taxiinterruptlines.RemoveAll();
				m_bAllTaxiInterruptLine = TRUE;
				continue;
			}
			int nLevelDiff = 0;
			if (m_treeOrg.GetParentItem(hItem) == m_hTaxiInterruptLines)
			{
				taxiinterruptlines.AddTail(hItem);
			}
		}
	}
}

BOOL CDlgSelectAirsideOrigDest::RecALTObject( BOOL bHasPrev, BOOL bAll, TreeSelectList& selects, std::vector<int>& recALTObj, ALTOBJECT_TYPE objType, LPCTSTR orgTypeTag )
{
	if(!bAll)
	{
		BOOL bFirst = TRUE;
		POSITION pos = selects.GetHeadPosition();
		while (pos)
		{
			HTREEITEM hSelItem = selects.GetNext(pos);
			ALTObjectGroup altObjGroup;
			ALTObjectID objName;
			CString strName;

			GetALTObjectFamilyAltID(hSelItem,objName,strName);

			altObjGroup.setType(objType);
			altObjGroup.setName(objName);
			int nSelStandFamilyID = altObjGroup.InsertData(m_nProjID);

			recALTObj.push_back(nSelStandFamilyID);
			if (bFirst)
			{
				if (bHasPrev)
					m_strOriginName += ";";

				m_strOriginName += orgTypeTag;
				bHasPrev = TRUE;
				bFirst = FALSE;
			}
			else
				m_strOriginName += ", ";

			m_strOriginName += strName;
		}
	}
	else
	{
		if (bHasPrev)
			m_strOriginName += _T(";");

		m_strOriginName += orgTypeTag;
		m_strOriginName += _T("All");
		bHasPrev = TRUE;
	}
	return bHasPrev;
}

void CDlgSelectAirsideOrigDest::PackIntoData( CIn_OutBoundRoute* pRoute ) const
{
	if (pRoute)
	{
		pRoute->SetAllStand(m_bAllStand);
		pRoute->SetStandGroupIDList(m_vStandSelID);
		pRoute->SetAllDeice(m_bAllDeice);
		pRoute->SetDeiceGroupIDList(m_vDeiceGroupSelID);
		pRoute->SetAllStartPosition(m_bAllStartPosition);
		pRoute->SetStartPositionGroupIDList(m_vStartPositionSelID);
		pRoute->SetAllTaxiInterruptLine(m_bAllTaxiInterruptLine);
		pRoute->SetTaxiInterruptLineIDList(m_vTaxiInterruptSelID);

		pRoute->SetAllRunway(m_bAllRunwayExit);
		pRoute->SetRunwayExitIDList(m_vExitSelID);
	}
}
int CDlgSelectAirsideOrigDest::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	m_wndSplitterVer.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, IDC_VERTICAL_SPLITTER_BAR);

	return 0;
}

void CDlgSelectAirsideOrigDest::OnSize(UINT nType, int cx, int cy)
{
	CXTResizeDialogEx::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	UpdateSplitterRange();
}

void CDlgSelectAirsideOrigDest::UpdateSplitterRange()
{
	const int nLeftMargin = 100;
	const int nRightMargin = 100;
	CRect rc;
	GetClientRect(rc);
	m_wndSplitterVer.SetRange(nLeftMargin, rc.right - nRightMargin);
}
LRESULT CDlgSelectAirsideOrigDest::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if (message == WM_NOTIFY)
	{
		if (wParam == IDC_VERTICAL_SPLITTER_BAR)
		{
			SPC_NMHDR* pHdr = (SPC_NMHDR*)lParam;
			int delta = pHdr->delta;

			CSplitterControl::ChangeWidth(GetDlgItem(IDC_STATIC_ORG), delta, CW_LEFTALIGN);
			CSplitterControl::ChangeWidth(&m_treeOrg, delta,CW_LEFTALIGN);

			CSplitterControl::ChangeWidth(GetDlgItem(IDC_STATIC_DST),-delta,CW_RIGHTALIGN);
			CSplitterControl::ChangeWidth(&m_treelRunwayExit,-delta,CW_RIGHTALIGN);

			UpdateResize();
		}
	}

	return CXTResizeDialogEx::DefWindowProc(message, wParam, lParam);
}
