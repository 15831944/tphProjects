// DlgSelectTakeoffPosi.cpp : implementation file
//

#include "stdafx.h"
//#include "AirsideGUI.h"
#include "Resource.h"
#include "DlgTakeoffPositionSelection.h"
#include "../InputAirside/runwaytakeoffposition.h"
#include ".\dlgtakeoffpositionselection.h"
#include "../InputAirside/TakeoffSequencing.h"
#include "../Common/AirportDatabase.h"
// CDlgSelectTakeoffPosi dialog

IMPLEMENT_DYNAMIC(CDlgSelectTakeoffPosition, CDialog)
CDlgSelectTakeoffPosition::CDlgSelectTakeoffPosition(int nProjID,int nRunwayID, int nRunwayMarkIndex,CWnd* pParent /*=NULL*/,bool bLeftDistFlag/*=FALSE*/)
: CDialog(CDlgSelectTakeoffPosition::IDD, pParent) 
,m_nProjID(nProjID)
,m_nSelRwyID(nRunwayID)
,m_nSelRwyMark(nRunwayMarkIndex)
,m_bLeftDistFlag(bLeftDistFlag)
{ 
	m_bSelAll = false;
	m_vRunwayExitList.clear();
	m_vTreeItemData.clear();
}

CDlgSelectTakeoffPosition::~CDlgSelectTakeoffPosition()
{
	for (size_t i = 0; i < m_vTreeItemData.size(); i++)
	{
		if (m_vTreeItemData[i])
		{
			delete m_vTreeItemData[i];
			m_vTreeItemData[i] = NULL;
		}
	}
	m_vTreeItemData.clear();
}

void CDlgSelectTakeoffPosition::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_TAKEOFFPOSI, m_wndTakeoffPosiTree);
}


BEGIN_MESSAGE_MAP(CDlgSelectTakeoffPosition, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_TAKEOFFPOSI, OnTvnSelchangedTreeTakeoffposi)
END_MESSAGE_MAP()
// CDlgSelectTakeoffPosi message handlers

BOOL CDlgSelectTakeoffPosition::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	//m_wndTakeoffPosiTree.EnableMultiSelect();
	SetTreeContents();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSelectTakeoffPosition::SetTreeContents(void)
{
	m_wndTakeoffPosiTree.SetRedraw(FALSE);
	m_wndTakeoffPosiTree.DeleteAllItems();
	CRunwayTakeOffPosition rwTakeOffPos(m_nProjID);
	rwTakeOffPos.SetAirportDB(m_pAirportDB);
 	rwTakeOffPos.ReadData();
	rwTakeOffPos.RemoveInvalidData();
	rwTakeOffPos.UpdateRwyTakeoffPosData();
	HTREEITEM hRoot = 0;
	HTREEITEM hRunwayMark = 0;

	int nAirportCount = rwTakeOffPos.GetAirportCount();
	for (int i = 0; i < nAirportCount; ++i) 
	{
		CRunwayTakeOffAirportData* pTakeOffAirportData = rwTakeOffPos.GetAirportItem(i);
		if(!pTakeOffAirportData)
			continue;

		hRoot = m_wndTakeoffPosiTree.InsertItem(pTakeOffAirportData->getAirport()->getName());

		if (m_nSelRwyID < 0)
			m_wndTakeoffPosiTree.InsertItem(_T("All"),hRoot);

		int nRunwayCount = pTakeOffAirportData->GetRunwayCount();
		for (int j = 0; j < nRunwayCount; ++j) 
		{
			CRunwayTakeOffRunwayData* pRwTakeOffRunwayData = pTakeOffAirportData->GetRunwayItem(j);
			if(!pRwTakeOffRunwayData)
				continue;

			CString strMarkTemp = _T("");
			Runway* pRunway = pRwTakeOffRunwayData->getRunway();

			if (m_nSelRwyID > -1 && pRunway->getID() != m_nSelRwyID)
				continue;

			if (m_nSelRwyMark < 0 || (m_nSelRwyMark > -1 && m_nSelRwyMark ==0))
			{
				CRunwayTakeOffRunwayWithMarkData* pRunwayMark1Data = pRwTakeOffRunwayData->getRunwayMark1Data();
				if(pRunwayMark1Data)
				{
					strMarkTemp = pRunway->GetMarking1().c_str();
					pRunwayMark1Data->setRunwayMarkIndex(0);
					hRunwayMark = m_wndTakeoffPosiTree.InsertItem(strMarkTemp,hRoot);
					m_wndTakeoffPosiTree.SetItemData(hRunwayMark, (DWORD_PTR)pRunwayMark1Data);
					SetRunwayTakeoffPositionInfoIntoTree(pRunwayMark1Data,pRunway,hRunwayMark);
				}
			}

			if (m_nSelRwyMark < 0 || (m_nSelRwyMark > -1 && m_nSelRwyMark ==1))
			{
				CRunwayTakeOffRunwayWithMarkData* pRunwayMark2Data = pRwTakeOffRunwayData->getRunwayMark2Data();
				if(pRunwayMark2Data)
				{
					strMarkTemp = pRunway->GetMarking2().c_str();
					pRunwayMark2Data->setRunwayMarkIndex(1);
					hRunwayMark = m_wndTakeoffPosiTree.InsertItem(strMarkTemp,hRoot);
					m_wndTakeoffPosiTree.SetItemData(hRunwayMark, (DWORD_PTR)pRunwayMark2Data);
					SetRunwayTakeoffPositionInfoIntoTree(pRunwayMark2Data,pRunway,hRunwayMark);

				}
			}
		}
		m_wndTakeoffPosiTree.Expand(hRoot,TVE_EXPAND);
	}
	m_wndTakeoffPosiTree.SetRedraw(TRUE);
}

void CDlgSelectTakeoffPosition::SetRunwayTakeoffPositionInfoIntoTree(CRunwayTakeOffRunwayWithMarkData* pRwTakeOffRunwayMarkData,Runway* pRunway,HTREEITEM hRunwayMark)
{
	if(pRwTakeOffRunwayMarkData == NULL)
		return;

	if (pRunway == NULL)
		return;

	CRunwayTakeOffPositionData* pRwyTakeoffPosData = NULL;

	CString strExitName = _T(""); 
	//IntersectionNode intersectionNode;
	//int idx = -1;
	HTREEITEM hTaxiway = NULL;

	RunwayExitList exitlist;
	pRunway->GetExitList((RUNWAY_MARK)pRwTakeOffRunwayMarkData->getRunwayMarkIndex(),exitlist);
	int nExit = exitlist.size();

	int nCount = pRwTakeOffRunwayMarkData->GetRwyTakeoffPosCount();
	for (int i =0; i < nCount; i++)
	{
		pRwyTakeoffPosData = pRwTakeOffRunwayMarkData->GetRwyTakeoffPosItem(i);
		if(!pRwyTakeoffPosData)
			continue;

//  		if (m_bLeftDistFlag || pRwyTakeoffPosData->GetEligible())
		{
			for (int m =0; m < nExit; m++)
			{
				if (exitlist[m].EqualToRunwayExitDescription(pRwyTakeoffPosData->GetRunwayExitDescription()) )
				{
					strExitName = exitlist[m].GetName();
					if (m_bLeftDistFlag)
					{
						std::vector<IntersectionNode> lstIntersecNode;
						lstIntersecNode.clear();
						IntersectionNode::ReadIntersectNodeList(pRwyTakeoffPosData->GetRunwayExitDescription().GetRunwayID(),pRwyTakeoffPosData->GetRunwayExitDescription().GetTaxiwayID(),lstIntersecNode);
						int nCount = (int)lstIntersecNode.size();
						if ( nCount != 0)
						{
							CPoint2008 posTaxiInt;
							for (int nIdx = 0; nIdx < nCount; nIdx++)
							{
								IntersectionNode& node = lstIntersecNode.at(nIdx);
								if (node.GetIndex() == pRwyTakeoffPosData->GetRunwayExitDescription().GetIndex())
								{
									posTaxiInt = node.GetPosition();
									break;
								}
							}
							CPoint2008 posRunMark = pRunway->getPath().GetIndexPoint((float)(pRwTakeOffRunwayMarkData->getRunwayMarkIndex()));
							DistanceUnit rwLength = pRunway->getPath().GetTotalLength();

							double distance = rwLength- posRunMark.distance(posTaxiInt);
							double dRemainFt = distance/100.0;

							CString strRemainFt;
							strRemainFt.Format(" %.2f m remaining",dRemainFt);
                            
							strExitName+=strRemainFt;							
						}
						

					}
					RunwayExit* pExit = new RunwayExit(exitlist[m]);
					hTaxiway = m_wndTakeoffPosiTree.InsertItem(strExitName,hRunwayMark); 
					m_wndTakeoffPosiTree.SetItemData(hTaxiway,(DWORD_PTR)pExit);
					break;
				}
			}
		}
		
	}
	m_wndTakeoffPosiTree.Expand(hRunwayMark,TVE_EXPAND);
}

bool CDlgSelectTakeoffPosition::IsTaxiwayItem(HTREEITEM hItem)
{
	HTREEITEM hParentItem = m_wndTakeoffPosiTree.GetChildItem(hItem);
	return (hParentItem == NULL);
}

void CDlgSelectTakeoffPosition::SetAirportDB(CAirportDatabase* pDB)
{
	m_pAirportDB = pDB;
}

void CDlgSelectTakeoffPosition::OnOK()
{
	if(!m_bSelAll)
	{
		//int nSelCount = m_wndTakeoffPosiTree.GetSelectedCount();

		HTREEITEM hSelItem = m_wndTakeoffPosiTree.GetSelectedItem();
		if (!hSelItem)
		{
			MessageBox(_T("No item select,please select the item!"),_T("Warning"),MB_OK);
			return;
		}
		if (!IsTaxiwayItem(hSelItem))
		{
			MessageBox(_T("Can't select This item !"),_T("warning"),MB_OK);
			return;
		}
		HTREEITEM hRunMarkItem = m_wndTakeoffPosiTree.GetParentItem(hSelItem);	
		CRunwayTakeOffRunwayWithMarkData* pRunwayMarkData = (CRunwayTakeOffRunwayWithMarkData*)m_wndTakeoffPosiTree.GetItemData(hRunMarkItem);
		if(pRunwayMarkData)
		{
			m_nRwyID = pRunwayMarkData->getRunwayID();
			m_nRwyMark =pRunwayMarkData->getRunwayMarkIndex();

			//for (int i = 0; i < nSelCount;++i) 
			{		
				if (hSelItem != NULL && IsTaxiwayItem(hSelItem))
				{
					RunwayExit* exit = (RunwayExit*)m_wndTakeoffPosiTree.GetItemData(hSelItem);
					m_vrSelID.push_back(exit->GetID());
					m_vRunwayExitList.push_back(*exit);

					m_arSelName.Add(m_wndTakeoffPosiTree.GetItemText(hSelItem));
				}
				//hSelItem = m_wndTakeoffPosiTree.GetNextSelectedItem(hSelItem);
			}
		}
	
	}
	CDialog::OnOK();
}

void CDlgSelectTakeoffPosition::OnTvnSelchangedTreeTakeoffposi(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	if(_T("All") == m_wndTakeoffPosiTree.GetItemText( m_wndTakeoffPosiTree.GetSelectedItem()))
		m_bSelAll = true;
	else
		m_bSelAll = false;

	*pResult = 0;
}
