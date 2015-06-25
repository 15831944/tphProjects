// DlgRunwayExitSelect.cpp : implementation file
//

#include "stdafx.h"
// #include "AirsideGUI.h"
#include "DlgRunwayExitSelect.h"
#include "../InputAirside/ALTAirport.h"
#include "InputAirside/InputAirside.h"


// CDlgRunwayExitSelect

IMPLEMENT_DYNAMIC(CDlgRunwayExitSelect, CXTResizeDialog)
CDlgRunwayExitSelect::CDlgRunwayExitSelect(const UINT nID,int nProjID,const CString& strSelExits, CWnd* pParent)
:CXTResizeDialog(CDlgRunwayExitSelect::IDD, pParent)
, m_nProjID(nProjID)
, m_bAllRunwayExit(FALSE)
{
	m_strRunwayExits = strSelExits;

}

CDlgRunwayExitSelect::~CDlgRunwayExitSelect()
{
}

BEGIN_MESSAGE_MAP(CDlgRunwayExitSelect, CXTResizeDialog)
	ON_WM_SIZE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_SELECT, OnTvnSelchangedTreeRunwayExit)
END_MESSAGE_MAP()

void CDlgRunwayExitSelect::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_TREE_SELECT,m_TreeCtrlRunwayExit);
}

BOOL CDlgRunwayExitSelect::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();
	SetDlgItemText(IDC_STATIC,_T("Runway Eixt"));
//	m_TreeCtrlRunwayExit.EnableMultiSelect();
	SetRunwayExitTreeContent();
	return true;
}
int CDlgRunwayExitSelect::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}
void CDlgRunwayExitSelect::OnTvnSelchangedTreeRunwayExit(NMHDR *pNMHDR, LRESULT *pResult)
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
CDlgRunwayExitSelect::RUNWAYEXITTYPE CDlgRunwayExitSelect::GetRunwayExitType(HTREEITEM hRunwayTreeItem)
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

void CDlgRunwayExitSelect::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	m_strRunwayExits = "";
	m_vExitSelID.clear();

	if(!m_bAllRunwayExit)
	{
	//	int nSelCount = m_TreeCtrlRunwayExit.GetSelectedCount();

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
//			hSelItem = m_TreeCtrlRunwayExit.GetNextSelectedItem(hSelItem);
		}
	}
	else
		m_strRunwayExits = "All";


	if (!m_strRunwayExits.Compare(""))
	{
		MessageBox("Please select takeoff position or exit!", "Error", MB_OK|MB_ICONEXCLAMATION );
		return;
	}
	CXTResizeDialog::OnOK();
}




// CDlgRunwayExitSelect message handlers


bool CDlgRunwayExitSelect::IsExitSelected( const CString& strExitName )
{
	if (m_strRunwayExits.Find(strExitName) >= 0)
		return true;

	return false;
}
void CDlgRunwayExitSelect::GetRunwayExitNameString(RunwayExit *pRunwayExit, RunwayExitList *pRunwayExitList, CString& strRunwayExitName)
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

void CDlgRunwayExitSelect::SetRunwayExitTreeContent(void)
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
void CDlgRunwayExitSelect::OnSize(UINT nType, int cx, int cy)
{

}
std::vector<int>& CDlgRunwayExitSelect::GetRunwayExitList()
{
	return m_vExitSelID;
}
BOOL CDlgRunwayExitSelect::IsAllRunwayExit()
{
	return m_bAllRunwayExit;
}
CString CDlgRunwayExitSelect::GetRunwayExitName()
{
	return m_strRunwayExits;
}

