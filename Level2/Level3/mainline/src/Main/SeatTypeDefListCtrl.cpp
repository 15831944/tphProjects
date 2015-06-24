#include "StdAfx.h"
#include ".\seattypedeflistctrl.h"
#include "Resource.h"
#include "DlgSeatGroupSelection.h"


CSeatTypeDefListCtrl::CSeatTypeDefListCtrl(void)
{
	m_pInputOnboard=NULL;
	m_pCurFlightSeatTypeDef=NULL;
	m_seatTypeName[ST_Bulkhead]=_T("Bulkhead");
	m_seatTypeName[ST_EmergencyExit]=_T("EmergencyExit");
	m_seatTypeName[ST_Aisle]=_T("Aisle");
	m_seatTypeName[ST_Window]=_T("Window");
	m_seatTypeName[ST_TowardsFront]=_T("TowardsFront");
	m_seatTypeName[ST_TowardsBack]=_T("TowardsBack");
	m_seatTypeName[ST_TowardsMiddle]=_T("TowardsMiddle");
	m_seatTypeName[ST_UpperDeck]=_T("UpperDeck");
	m_seatTypeName[ST_LowerDeck]=_T("LowerDeck");
	m_seatTypeName[ST_MiddleDeck]=_T("MiddleDeck");

	for (int i=0;i<ST_SeatTypeNum;i++)
	{
		m_sSeatTypeList.AddTail(m_seatTypeName[i]);
	}
}

CSeatTypeDefListCtrl::~CSeatTypeDefListCtrl(void)
{
}
BEGIN_MESSAGE_MAP(CSeatTypeDefListCtrl, CListCtrlEx)
	//{{AFX_MSG_MAP(CConDBListCtrl)
	ON_NOTIFY_REFLECT_EX(LVN_ENDLABELEDIT, OnEndlabeledit)
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
	//	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnLvnEndlabeledit)
	//ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()
void CSeatTypeDefListCtrl::resetListContent(CFlightSeatTypeDefine *CurFlightSeatTypeDef)
{
	DeleteAllItems();
	m_pCurFlightSeatTypeDef=CurFlightSeatTypeDef;
	if (CurFlightSeatTypeDef)
	{		
		for(int i=0;i<(int)CurFlightSeatTypeDef->GetItemCount();i++)
		{
			CSeatTypeDefine *curSeatTypeDef=(CSeatTypeDefine *)CurFlightSeatTypeDef->GetItem(i);
			int nIndex = InsertItem( i, _T("") );
			SetItemData(i,(DWORD)curSeatTypeDef);

			if (curSeatTypeDef->isSeatGroupDefault())
			{
				SetItemText(i,0,"ALL SEAT");
			}else
			{
				SetItemText(i,0,curSeatTypeDef->getSeatGroup().GetIDString());
			}

			{
				SetItemText(i,1,m_seatTypeName[curSeatTypeDef->getSeatType()]);
			}

		}
	}
}

void CSeatTypeDefListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	int index, column;
	if((index = HitTestEx(point, &column)) != -1)
	{		
		if (column==0)
		{
			if (m_pInputOnboard==NULL||m_pCurFlightSeatTypeDef==NULL)
			{
				return;
			}

			COnBoardAnalysisCandidates *pOnBoardList = m_pInputOnboard->GetOnBoardFlightCandiates();
			COnboardFlight* curFlight=m_pCurFlightSeatTypeDef->getonboardFlight();
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
			m_pCurFlightSeatTypeDef->setLayoutID(_layout->GetID());				
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

			CSeatTypeDefine *curSeatTypeDef=(CSeatTypeDefine *)GetItemData(index);
			curSeatTypeDef->setSeatGroup(seatGroup);
		}else if(column==1)
		{
			::SendMessage( this->GetParent()->GetSafeHwnd(), WM_COLLUM_INDEX,WPARAM ( index ),0);	
			if( GetItemData( index ) == -1 )
				return;

			CStringList& strNameList = ((LVCOLDROPLIST*)ddStyleList[column])->List;
			strNameList.RemoveAll();
			strNameList.AddTail( &m_sSeatTypeList );

		}
	}
	CListCtrlEx::OnLButtonDblClk(nFlags, point);
}
BOOL CSeatTypeDefListCtrl::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	CListCtrlEx::OnEndlabeledit( pNMHDR, pResult );

	// TODO: Add your control notification handler code here
	LV_ITEM* plvItem = &pDispInfo->item;

	if (plvItem->iSubItem == 1)
	{
		for (int i=0;i<ST_SeatTypeNum;i++)
		{
			if (strcmp(m_seatTypeName[i],plvItem->pszText)==0)
			{
				CSeatTypeDefine *curSeatTypeDef=(CSeatTypeDefine *)GetItemData(plvItem->iItem);
				curSeatTypeDef->setSeatType((OnboardSeatType)i);
				break;
			}
		}
	}
	return true;
}
