// DlgRunwayExitSelect.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgRunwayExitSelect.h"

#include "../InputAirside/ALT_BIN.h"
// CDlgRunwayExitSelect dialog

IMPLEMENT_DYNAMIC(CDlgRunwayExitSelect, CDialog)
CDlgRunwayExitSelect::CDlgRunwayExitSelect(std::vector<ALTObject>* _RunwayData , bool bSelectOneExit/* = false*/, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRunwayExitSelect::IDD, pParent),m_RunwayData(_RunwayData)
{
	m_bSelectOneExit = bSelectOneExit;
}

CDlgRunwayExitSelect::~CDlgRunwayExitSelect()
{
}

void CDlgRunwayExitSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_RUNWAYEXIT, m_treeCtrl);
}


BEGIN_MESSAGE_MAP(CDlgRunwayExitSelect, CDialog)
END_MESSAGE_MAP()

BOOL CDlgRunwayExitSelect::OnInitDialog()
{
	CDialog::OnInitDialog() ;
	SetWindowText(_T("Runway Exit Select")) ;
	InitTreeCtrlData() ;

	if(m_treeCtrl.GetRootItem()!=NULL)
		m_treeCtrl.SelectItem(m_treeCtrl.GetRootItem());


	return TRUE ;
}
void CDlgRunwayExitSelect::InitTreeCtrlData()
{
	SetRunwayExitTreeContent() ;
}
void CDlgRunwayExitSelect::SetRunwayExitTreeContent(void)
{

		Runway runway ;
		std::vector<ALTObject>::iterator iterRunwayID = m_RunwayData->begin();
		for (; iterRunwayID != m_RunwayData->end(); ++iterRunwayID)
		{
			runway.ReadObject((*iterRunwayID).getID());
		
			RunwayExitList runwayExitList;
			int nRunwayExitCount = runway.GetExitList(RUNWAYMARK_FIRST, runwayExitList);

			//if the runway haven't exit
			if (runwayExitList.size() < 1)
			{
				continue;
			}

			HTREEITEM hRunwayItem = m_treeCtrl.InsertItem(runway.GetObjNameString());
			m_treeCtrl.SetItemData(hRunwayItem,runway.getID());

			//marking 1
			HTREEITEM hMarking1Item = m_treeCtrl.InsertItem(runway.GetMarking1().c_str(), hRunwayItem);
			m_treeCtrl.SetItemData(hMarking1Item, (int)RUNWAYMARK_FIRST);

			std::vector<RunwayExit>::const_iterator citer;
			CString runwayExitName;
			for (citer=runwayExitList.begin(); citer!=runwayExitList.end(); citer++)
			{
				RunwayExit runwayExit = *citer;

				//runwayExitName = runwayExit.m_strName;	
				GetRunwayExitName(&runwayExit, &runwayExitList, runwayExitName);

				HTREEITEM hPositionItem = m_treeCtrl.InsertItem( runwayExitName, hMarking1Item);
				m_treeCtrl.SetItemData(hPositionItem, (DWORD_PTR)runwayExit.GetID());
			}
			m_treeCtrl.Expand(hMarking1Item, TVE_EXPAND);

			//marking 2
			HTREEITEM hMarking2Item = m_treeCtrl.InsertItem(runway.GetMarking2().c_str(), hRunwayItem);
			m_treeCtrl.SetItemData(hMarking2Item, (int)RUNWAYMARK_SECOND);

			RunwayExitList runwayExitListMarkTwo;
			nRunwayExitCount = runway.GetExitList(RUNWAYMARK_SECOND, runwayExitListMarkTwo);
			for (citer=runwayExitListMarkTwo.begin(); citer!=runwayExitListMarkTwo.end(); citer++)
			{
				RunwayExit runwayExit = *citer;

				//runwayExitName = runwayExit.m_strName;	
				GetRunwayExitName(&runwayExit, &runwayExitListMarkTwo, runwayExitName);

				HTREEITEM hPositionItem = m_treeCtrl.InsertItem(runwayExitName, hMarking2Item);
				m_treeCtrl.SetItemData(hPositionItem, (DWORD_PTR)runwayExit.GetID());
			}
			m_treeCtrl.Expand(hMarking2Item, TVE_EXPAND);

			m_treeCtrl.Expand(hRunwayItem, TVE_EXPAND);

			m_treeCtrl.Expand(hRunwayItem,TVE_EXPAND) ;

		}

}

void CDlgRunwayExitSelect::GetRunwayExitName(RunwayExit *pRunwayExit, RunwayExitList *pRunwayExitList, CString& strRunwayExitName)
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
		int nOtherTaxiwayID = pOtherRunwayExit->GetTaxiwayID();
		int nTaxiwayID = pRunwayExit->GetTaxiwayID();
		if (nOtherTaxiwayID == nTaxiwayID)
		{
			bDisplayLeftRight = TRUE;

			if (pOtherRunwayExit->GetSideType() == pRunwayExit->GetSideType())
			{
				bDisplayIndex = TRUE;
			}
		}
	}

	CString strTaxiwayMark;
	strTaxiwayMark.Format(_T("%s"), taxiway.GetMarking().c_str());

	if (bDisplayLeftRight)
	{
		if (pRunwayExit->GetSideType() == RunwayExit::EXIT_LEFT)
		{
			strTaxiwayMark += "&LEFT";
		}
		else
		{
			strTaxiwayMark += "&RIGHT";
		}

		if (bDisplayIndex)
		{
			CString strIndex;
			strIndex.Format(_T("%d"), pRunwayExit->GetIntesectionNode().GetIndex());
			strTaxiwayMark += "&";
			strTaxiwayMark += strIndex;
		}
	}else
		strTaxiwayMark.Format(_T("%s"), pRunwayExit->GetName());

	strRunwayExitName.Format(_T("%s"), strTaxiwayMark);
}
void CDlgRunwayExitSelect::OnOK()
{
		HTREEITEM _item = m_treeCtrl.GetSelectedItem() ;
		if(_item != NULL)
		{
			
				HTREEITEM ParItem = m_treeCtrl.GetParentItem(_item) ;
				if(ParItem == NULL) // select all the runway exit
				{	
					if(m_bSelectOneExit)
					{
						MessageBox(_T("Please select one exit."),_T("ARCPort"),MB_OK);
						return;
					}

					HTREEITEM _childItem = m_treeCtrl.GetChildItem(_item) ;
					while(_childItem)
					{
						AddExitItemNode(_childItem) ;
						_childItem = m_treeCtrl.GetNextSiblingItem(_childItem) ;
					}
				}
				else
				{

					int NodeData = m_treeCtrl.GetItemData(_item) ;
					if(NodeData  == RUNWAYMARK_SECOND || NodeData == RUNWAYMARK_FIRST) //select first or second runway exit
					{
						if(m_bSelectOneExit)
						{
							MessageBox(_T("Please select one exit."),_T("ARCPort"),MB_OK);
							return;
						}

						HTREEITEM ParItem = m_treeCtrl.GetParentItem(_item) ;
						if(ParItem != NULL)
						{
							m_RunwayID = m_treeCtrl.GetItemData(ParItem) ;
							m_RunwayName = m_treeCtrl.GetItemText(ParItem) ;
						}
						AddExitItemNode(_item) ;
					}
					else 
					{ 
					//select one exit
						CSelectData ExitPair ;
						ExitPair.m_ExitID = m_treeCtrl.GetItemData(_item) ;
						ExitPair.m_ExitName = m_treeCtrl.GetItemText(_item) ;
						ExitPair.m_RunwayID = m_treeCtrl.GetItemData(m_treeCtrl.GetParentItem(ParItem)) ;
						ExitPair.m_RunwayName = m_treeCtrl.GetItemText(m_treeCtrl.GetParentItem(ParItem)) ;
						m_SelectExit.push_back(ExitPair) ;
					}

				}
		}
		CDialog::OnOK() ;
}
void CDlgRunwayExitSelect::AddExitItemNode(HTREEITEM _ItemNode)
{
	if(_ItemNode == NULL)
		return ;
	int runwayID = m_treeCtrl.GetItemData(m_treeCtrl.GetParentItem(_ItemNode)) ;
	CString runwayname  ;
	runwayname = m_treeCtrl.GetItemText(m_treeCtrl.GetParentItem(_ItemNode)) ;
	HTREEITEM ChildItem = m_treeCtrl.GetChildItem(_ItemNode) ;
	while(ChildItem)
	{
		CSelectData ExitPair ;
		ExitPair.m_ExitID = m_treeCtrl.GetItemData(ChildItem) ;
		ExitPair.m_ExitName = m_treeCtrl.GetItemText(ChildItem) ;
		ExitPair.m_RunwayID = runwayID ;
		ExitPair.m_RunwayName = runwayname ;
		m_SelectExit.push_back(ExitPair) ;
		ChildItem = m_treeCtrl.GetNextSiblingItem(ChildItem) ;
	}
	m_treeCtrl.Expand(ChildItem,TVE_EXPAND) ;
}
// CDlgRunwayExitSelect message handlers
