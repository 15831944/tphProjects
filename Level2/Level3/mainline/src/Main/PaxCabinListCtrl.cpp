#include "StdAfx.h"
#include ".\paxcabinlistctrl.h"
#include "Resource.h"
#include "DlgNewPassengerType.h"
#include "..\InputOnboard\AircaftLayOut.h"
#include "DlgSeatGroupSelection.h"
#include "..\InputOnboard\AircraftPlacements.h"

CPaxCabinListCtrl::CPaxCabinListCtrl(void)
{
	m_pTerminal=NULL;
	m_pInputOnboard=NULL;
	m_pCurAssign=NULL;
}

CPaxCabinListCtrl::~CPaxCabinListCtrl(void)
{
}
BEGIN_MESSAGE_MAP(CPaxCabinListCtrl, CListCtrlEx)
	//{{AFX_MSG_MAP(CCalloutDispSpecsListCtrl)	
	ON_WM_LBUTTONDBLCLK()
	
	//}}AFX_MSG_MAP

END_MESSAGE_MAP()
// void CPaxCabinListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
// {
// 	GetParent()->SendMessage(WM_ONLBTN_CLICK);
// }
void CPaxCabinListCtrl::resetListContent(CFlightPaxCabinAssign *curAssign)
{
	DeleteAllItems();
	m_pCurAssign=curAssign;
	if (curAssign)
	{		
		for(int i=0;i<(int)curAssign->GetItemCount();i++)
		{
			CPaxSeatGroupAssign *curPaxSeatGroupAssign=(CPaxSeatGroupAssign *)curAssign->GetItem(i);
			int nIndex = InsertItem( i, _T("") );
			SetItemData(i,(DWORD)curPaxSeatGroupAssign);
			if (curPaxSeatGroupAssign->isPaxTypeDefault())
			{
				SetItemText(i,0,"DEFAULT");
			}else
			{
				SetItemText(i,0,curPaxSeatGroupAssign->getPaxType()->PrintStringForShow());
			}

			if (curPaxSeatGroupAssign->isSeatGroupDefault())
			{
				SetItemText(i,1,"ALL SEAT");
			}else
			{
                SetItemText(i,1,curPaxSeatGroupAssign->getSeatGroup().GetIDString());
			}
			
		}
	}

}
void CPaxCabinListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	int index;
	int column;
	if((index = HitTestEx(point, &column)) != -1)
	{
		switch (column)
		{
		case 0:
			{
				CPassengerType *paxType=new CPassengerType(m_pTerminal->inStrDict);
				CDlgNewPassengerType dlg(paxType,m_pTerminal);
				if (dlg.DoModal() == IDOK)
				{
					CString strPaxType = _T("");
					strPaxType = paxType->PrintStringForShow();
					CPaxSeatGroupAssign *curPaxSeatGroupAssign=(CPaxSeatGroupAssign *)GetItemData(index);
					curPaxSeatGroupAssign->setPaxType(paxType);
					SetItemText(index,column,strPaxType);
				}
				break;
			}
		case 1:
			{
				if (m_pInputOnboard==NULL||m_pCurAssign==NULL)
				{
					return;
				}
				
				COnBoardAnalysisCandidates *pOnBoardList = m_pInputOnboard->GetOnBoardFlightCandiates();
				COnboardFlight* curFlight=m_pCurAssign->getonboardFlight();
				COnboardFlight* pFlightData=pOnBoardList->GetExistOnboardFlight(curFlight->getFlightID(),curFlight->getACType(),curFlight->getFlightType(),curFlight->getDay());
				
				if (pFlightData==NULL)
				{
					MessageBox("Please add the flight in candidates.");
					return;
				}
				
				CAircaftLayOut* _layout = CAircarftLayoutManager::GetInstance()->GetAircraftLayOutByFlightID(pFlightData->getID());
				if (_layout==NULL)
				{
					MessageBox("Please define layout.");
						return;
				}
                m_pCurAssign->setLayoutID(_layout->GetID());				
				CDlgSeatGroupSelection dlg(_layout->GetPlacements()->GetSeatData());
				if(dlg.DoModal()!=IDOK)
				{
                    return;
				}
				
				ALTObjectID seatGroup=dlg.getSeatGroup();
				if (seatGroup.GetIDString()==_T(""))
				{
                    SetItemText(index,column,"ALL SEAT");
				}else
				{
					SetItemText(index,column,seatGroup.GetIDString());
				}
				
				CPaxSeatGroupAssign *curPaxSeatGroupAssign=(CPaxSeatGroupAssign *)GetItemData(index);
				curPaxSeatGroupAssign->setSeatGroup(seatGroup);
				m_pCurAssign->setLayoutID(_layout->GetID());

// 				int nCount = pOnBoardList->getCount();
// 				for (int i = 0; i < nCount; i++)
// 				{
// 					CFlightTypeCandidate* pFltData = pOnBoardList->getItem(i);
// 					for (int j = 0; j < pFltData->getCount(); j++)
// 					{
// 						COnboardFlight* pFlightData = pFltData->getItem(j);
// 						COnboardFlight* curFlight=m_pCurAssign->getonboardFlight();
// 						
// 					}
// 				}
				break;
			}
		}
	}
}
void CPaxCabinListCtrl::setTerminal(InputTerminal *terminal)
{
	m_pTerminal=terminal;
}
void CPaxCabinListCtrl::setInputOnboard(InputOnboard* pInputOnboard)
{
	m_pInputOnboard=pInputOnboard;
}

