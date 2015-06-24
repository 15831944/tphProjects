// PaxDistListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "CalloutDispSpecsListCtrl.h"

#include "common\WinMsg.h"

#include "TermPlanDoc.h"

#include "DlgNewPassengerType.h"
#include "CalloutDispSpec.h"
#include "PassengerTypeDialog.h"
#include "DlgTimeRange.h"
#include "ProcesserDialog.h"
#include "DlgIdeticalNameSelect.h"

#include "AirsideGUI/DlgTimePicker.h"
#include "AirsideGUI/GSESelectDlg.h"
#include "AirsideGUI/DlgALTObjectGroup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CCalloutDispSpecsListCtrl

CCalloutDispSpecsListCtrl::CCalloutDispSpecsListCtrl(PFuncSelectFlightType pSelFltType, int nProjID, CTermPlanDoc* pTermPlanDoc)
	: m_pSelFltType(pSelFltType)
	, m_nProjID(nProjID)
	, m_pTermPlanDoc(pTermPlanDoc)
	, m_pInTerminal(&pTermPlanDoc->GetTerminal())
{
	CBitmap bitmap;
	VERIFY(bitmap.LoadBitmap(AFX_IDB_CHECKLISTBOX_95));
	BITMAP bm;
	bitmap.GetObject(sizeof(BITMAP), &bm);
	m_sizeCheck.cx = bm.bmWidth / 3;
	m_sizeCheck.cy = bm.bmHeight;
	m_hBitmapCheck = (HBITMAP) bitmap.Detach();
}

CCalloutDispSpecsListCtrl::~CCalloutDispSpecsListCtrl()
{
	if(m_hBitmapCheck != NULL)
		::DeleteObject(m_hBitmapCheck);
}


BEGIN_MESSAGE_MAP(CCalloutDispSpecsListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CCalloutDispSpecsListCtrl)
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCalloutDispSpecsListCtrl message handlers
/////////////////////////////////////////////////////////////////////////////
// CCalloutDispSpecsListCtrl message handlers

void CCalloutDispSpecsListCtrl::DrawItem( LPDRAWITEMSTRUCT lpdis)
{
	COLORREF clrTextSave, clrBkSave;
	CRect rcItem(lpdis->rcItem);
	CDC* pDC = CDC::FromHandle(lpdis->hDC);
	int nItem = lpdis->itemID;
	LVITEM lvi; 
	lvi.mask = LVIF_PARAM | LVIF_STATE; 
	lvi.iItem = nItem;
	lvi.iSubItem = 0;
	lvi.stateMask = 0xFFFF;
	GetItem(&lvi); 

	BOOL bFocus = (GetFocus() == this);
	BOOL bSelected = (lvi.state & LVIS_SELECTED) || (lvi.state & LVIS_DROPHILITED);
	CalloutDispSpecDataItem* pDataItem = (CalloutDispSpecDataItem*)lvi.lParam;

	// calc all the rectangles
	CRect rcAllLabels;
	this->GetItemRect(nItem, rcAllLabels, LVIR_BOUNDS);
	CRect rcEnabled;
	GetSubItemRect(nItem, colEnabled, LVIR_LABEL, rcEnabled);
	GetEnabledRect(rcEnabled);
	CRect rcEnvMode; 
	GetSubItemRect(nItem, colEnvMode, LVIR_LABEL, rcEnvMode);
	CRect rcFacility; 
	GetSubItemRect(nItem, colFacility, LVIR_LABEL, rcFacility);
	CRect rcClient; 
	GetSubItemRect(nItem, colClient, LVIR_LABEL, rcClient);
	CRect rcStartTime; 
	GetSubItemRect(nItem, colStartTime, LVIR_LABEL, rcStartTime);
	CRect rcEndTime; 
	GetSubItemRect(nItem, colEndTime, LVIR_LABEL, rcEndTime);
	CRect rcIntervalTime; 
	GetSubItemRect(nItem, colIntervalTime, LVIR_LABEL, rcIntervalTime);
	CRect rcMeassure; 
	GetSubItemRect(nItem, colMeasure, LVIR_LABEL, rcMeassure);
	CRect rcConnection; 
	GetSubItemRect(nItem, colConnection, LVIR_LABEL, rcConnection);

	rcEnvMode.DeflateRect(4, 0, 0, 0);
	rcFacility.DeflateRect(4, 0, 0, 0);
	rcClient.DeflateRect(4, 0, 0, 0);
	rcStartTime.DeflateRect(4, 0, 0, 0);
	rcEndTime.DeflateRect(4, 0, 0, 0);
	rcIntervalTime.DeflateRect(4, 0, 0, 0);
	rcMeassure.DeflateRect(4, 0, 0, 0);
	rcConnection.DeflateRect(4, 0, 0, 0);

	CString strText;
	if(bSelected) { 
		clrTextSave = pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		clrBkSave = pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
		pDC->FillRect(rcAllLabels, &CBrush(::GetSysColor(COLOR_HIGHLIGHT))); 
	} 
	else {
		clrTextSave = pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
		clrBkSave = pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
		pDC->FillRect(rcAllLabels, &CBrush(::GetSysColor(COLOR_WINDOW)));
	}

	//draw Enabled checkbox
	CDC bitmapDC;
	HBITMAP hOldBM;
	int nCheck = (int)pDataItem->GetEnabled();
	if(bitmapDC.CreateCompatibleDC(pDC)) {
		hOldBM = (HBITMAP) ::SelectObject(bitmapDC.m_hDC, m_hBitmapCheck);
		pDC->BitBlt(rcEnabled.left, rcEnabled.top, m_sizeCheck.cx, m_sizeCheck.cy, &bitmapDC,
			m_sizeCheck.cx * nCheck, 0, SRCCOPY);
		::SelectObject(bitmapDC.m_hDC, hOldBM);
		bitmapDC.DeleteDC();
	}

	//draw Environment Mode
	strText = CalloutDispSpecDataItem::m_sEnvModeStrings[pDataItem->GetEnvMode()];
	pDC->DrawText(strText, rcEnvMode, DT_LEFT | DT_VCENTER | DT_SINGLELINE );

	//draw Facility
	strText = pDataItem->GetFacilityText();
	DrawTypeValuePairText(strText, pDC, rcFacility, bSelected);

	//draw Client
	strText = pDataItem->GetClientText();
	DrawTypeValuePairText(strText, pDC, rcClient, bSelected);


	//draw Start Time
	strText = pDataItem->GetStartTimeString();
	pDC->DrawText(strText, rcStartTime, DT_LEFT | DT_VCENTER | DT_SINGLELINE );

	//draw End Time
	strText = pDataItem->GetEndTimeString();
	pDC->DrawText(strText, rcEndTime, DT_LEFT | DT_VCENTER | DT_SINGLELINE );

	//draw Interval Time
	strText = pDataItem->GetIntervalTime().printTime();
	pDC->DrawText(strText, rcIntervalTime, DT_LEFT | DT_VCENTER | DT_SINGLELINE );

	//draw Measure
	strText = CalloutDispSpecDataItem::m_sMessureTypeStrings[pDataItem->GetMessure()];
	pDC->DrawText(strText, rcMeassure, DT_LEFT | DT_VCENTER | DT_SINGLELINE );

	//draw Connection
	strText = CalloutDispSpecDataItem::m_sConnectionTypeStrings[pDataItem->GetConnection()];
	pDC->DrawText(strText, rcConnection, DT_LEFT | DT_VCENTER | DT_SINGLELINE );
}

void CCalloutDispSpecsListCtrl::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint ptMsg = GetMessagePos();
	ScreenToClient( &ptMsg );
	NMLISTVIEW* pNMLW = (NMLISTVIEW*) pNMHDR;
	int nItem = pNMLW->iItem;
	if(nItem != -1) 
	{
		if(pNMLW->iSubItem == 0) 
		{
			// Enabled column
			CRect rcEnabled;
			GetSubItemRect(nItem,0,LVIR_LABEL,rcEnabled);
			GetEnabledRect(rcEnabled);
			if(rcEnabled.PtInRect(ptMsg))
			{
				CalloutDispSpecDataItem* pDataItem = (CalloutDispSpecDataItem*)GetItemData(nItem);
				pDataItem->SetEnabled(!pDataItem->GetEnabled());
			}
		}
		CurrentSelection = nItem;
		Invalidate();
	}

	*pResult = 0;
}

void CCalloutDispSpecsListCtrl::GetEnabledRect(CRect& r)
{
	r.left = (r.left+r.right-m_sizeCheck.cx)/2;
	r.top = (r.bottom+r.top-m_sizeCheck.cy)/2;
	r.bottom = r.top+m_sizeCheck.cy;
	r.right = r.left+m_sizeCheck.cx;
}


LRESULT CCalloutDispSpecsListCtrl::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(message == WM_INPLACE_COMBO_KILLFOUCUS)
	{
		int nSelIndex = (int) wParam;
		if (CB_ERR != nSelIndex)
		{
			LV_DISPINFO* dispInfo = (LV_DISPINFO*)lParam;
			int nSelItem = dispInfo->item.iItem;
			int nSubItem = dispInfo->item.iSubItem;

			if (-1 != nSelItem)
			{
				CalloutDispSpecDataItem* pDataItem = (CalloutDispSpecDataItem*)GetItemData(nSelItem);
				ASSERT(pDataItem);
				switch (nSubItem)
				{
				case colEnvMode:
					{
						pDataItem->ChangeEnvMode( ( EnvironmentMode ) nSelIndex );
						SetItemText(nSelItem, colEnvMode, CalloutDispSpecDataItem::m_sEnvModeStrings[pDataItem->GetEnvMode()]);
					}
					break;
				case colFacility:
					{
						CalloutDispSpecDataItem::FacilityType selFacility =
							(CalloutDispSpecDataItem::FacilityType)(CalloutDispSpecDataItem::m_sFacilityStart[pDataItem->GetEnvMode()] + nSelIndex);

						CalloutDispSpecDataItem::FacilityData facilityData;
						facilityData.SetFacility(selFacility);
						bool bFacilityModified = false;
						if (CalloutDispSpecDataItem::FacilityData::IsALTObjectGroup(selFacility))
						{
							ALTOBJECT_TYPE altobjType = CalloutDispSpecDataItem::FacilityData::m_sALTObjectGroupTypeMapping[selFacility];
							CDlgALTObjectGroup dlg(m_nProjID, altobjType);
							if (IDOK == dlg.DoModal())
							{
								facilityData.SetALTObjectGroupID(dlg.GetSelALTObjectGroupID());
								bFacilityModified = true;
							}
						}
						else
						{
							switch (selFacility)
							{
							case CalloutDispSpecDataItem::Facility_Processor:
								{
									CProcesserDialog dlg(m_pInTerminal);
									if (IDOK == dlg.DoModal())
									{
										ProcessorID id;
										if (dlg.GetProcessorID(id))
										{
											facilityData.SetProcessorID(id);
											bFacilityModified = true;
										}
									}
								}
								break;
							case CalloutDispSpecDataItem::Facility_Area_Terminal:
								{
									CDlgIdeticalNameSelect::IdenticalNameList areaNameList;
									const CAreaList& arealList = m_pInTerminal->m_pAreas->m_vAreas;
									size_t nCount = arealList.size();
									for(size_t i=0;i<nCount;i++)
									{
										areaNameList.push_back(arealList[i]->name);
									}

									CDlgIdeticalNameSelect dlg(areaNameList, _T("Select Area"));
									if (IDOK == dlg.DoModal())
									{
										facilityData.SetAreaName_Terminal(dlg.GetSelName());
										bFacilityModified = true;
									}
								}
								break;
							case CalloutDispSpecDataItem::Facility_Portal:
								{
									CDlgIdeticalNameSelect::IdenticalNameList portalNameList;
									const CPortalList& portalList = m_pTermPlanDoc->m_portals.m_vPortals;
									size_t nCount = portalList.size();
									for(size_t i=0;i<nCount;i++)
									{
										portalNameList.push_back(portalList[i]->name);
									}

									CDlgIdeticalNameSelect dlg(portalNameList, _T("Select Portal"));
									if (IDOK == dlg.DoModal())
									{
										facilityData.SetPortalName(dlg.GetSelName());
										bFacilityModified = true;
									}
								}
								break;
							default:
								{
									ASSERT(FALSE);
								}
								break;
							}
						}

						if (bFacilityModified)
						{
							pDataItem->ChangeFacilityType(selFacility);
							pDataItem->SetFacilityData(facilityData);
							SetItemText(nSelItem, colFacility, pDataItem->GetFacilityText());
						}
					}
					break;
				case colStartTime:
					{
						pDataItem->SetStartTimeMode( ( CalloutDispSpecDataItem::StartTimeMode ) nSelIndex );
						if (pDataItem->GetStartTimeMode() == CalloutDispSpecDataItem::StartTimeMode_Absolute)
						{
							EditTimeRange(pDataItem);
							SetItemText(nSelItem, colStartTime, pDataItem->GetStartTimeString());
							SetItemText(nSelItem, colEndTime, pDataItem->GetEndTimeString());
						}
					}
					break;
				case colEndTime:
					{
						pDataItem->SetEndTimeMode( ( CalloutDispSpecDataItem::EndTimeMode ) nSelIndex );
						if (pDataItem->GetEndTimeMode() == CalloutDispSpecDataItem::EndTimeMode_Absolute)
						{
							EditTimeRange(pDataItem);
							SetItemText(nSelItem, colStartTime, pDataItem->GetStartTimeString());
							SetItemText(nSelItem, colEndTime, pDataItem->GetEndTimeString());
						}
					}
					break;
				case colMeasure:
					{
						CalloutDispSpecDataItem::MessureType selMeasure =
							(CalloutDispSpecDataItem::MessureType)CalloutDispSpecDataItem::m_sMessureTypeOfFacilities[pDataItem->GetFacilityType()][nSelIndex];
						pDataItem->SetMessure(selMeasure);
						SetItemText(nSelItem, colMeasure, CalloutDispSpecDataItem::m_sMessureTypeStrings[pDataItem->GetMessure()]);
					}
					break;
				case colConnection:
					{
						pDataItem->SetConnection( ( CalloutDispSpecDataItem::ConnectionType ) nSelIndex );
						SetItemText(nSelItem, colConnection, CalloutDispSpecDataItem::m_sConnectionTypeStrings[pDataItem->GetConnection()]);
					}
					break;
				default:
					{
						// ASSERT(FALSE);
					}
					break;
				}
			}

			Invalidate();
			SetFocus();
			return TRUE;
		}

	}


	return CListCtrl::DefWindowProc(message, wParam, lParam);
}

void CCalloutDispSpecsListCtrl::InitListHeader()
{
	DWORD dwStyle = GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	SetExtendedStyle( dwStyle );

	static int CONST columnFormats[col_TotalCount] =
	{
		LVCFMT_CHECK | LVCFMT_CENTER,
		LVCFMT_DROPDOWN,
		LVCFMT_DROPDOWN,
		LVCFMT_NOEDIT,
		LVCFMT_DROPDOWN,
		LVCFMT_DROPDOWN,
		LVCFMT_NOEDIT,
		LVCFMT_DROPDOWN,
		LVCFMT_DROPDOWN,
	};
	static LPCTSTR CONST columnLabels[col_TotalCount] =
	{
		_T("Enabled"),
		_T("Environment Mode"),
		_T("Facility"),
		_T("Client"),
		_T("Start(day, hh:mm:ss)"),
		_T("End(day, hh:mm:ss)"),
		_T("Interval(hh:mm:ss)"),
		_T("Measure"),
		_T("Connection"),
	};
	static int CONST columnWidths[col_TotalCount] =
	{
		50,
		110,
		200,
		150,
		100,
		100,
		100,
		200,
		80,
	};

	CStringList strList;
	LV_COLUMNEX	lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.csList = &strList;
	for (int i = 0; i < 9; i++)
	{
		lvc.fmt = columnFormats[i];
		lvc.pszText = (LPSTR)columnLabels[i];
		lvc.cx = columnWidths[i];
		InsertColumn(i, &lvc);
	}
}

void CCalloutDispSpecsListCtrl::ResetListContent( CalloutDispSpecItem* pSpecItem )
{
	DeleteAllItems();
	if (pSpecItem)
	{
		CalloutDispSpecDataList& specDataList = pSpecItem->GetData();
		size_t nCount = specDataList.GetElementCount();
		for(size_t i=0;i<nCount;i++)
		{
			CalloutDispSpecDataItem* pDataItem = specDataList.GetItem(i);
			int nIndex = InsertItem( i, _T("") );
			SetItemData( nIndex, (DWORD)pDataItem );

			SetItemText(nIndex, colEnvMode, CalloutDispSpecDataItem::m_sEnvModeStrings[pDataItem->GetEnvMode()]);
			SetItemText(nIndex, colFacility, pDataItem->GetFacilityText());
			SetItemText(nIndex, colClient, pDataItem->GetClientText());
			SetItemText(nIndex, colStartTime, pDataItem->GetStartTimeString());
			SetItemText(nIndex, colEndTime, pDataItem->GetEndTimeString());
			SetItemText(nIndex, colIntervalTime, pDataItem->GetIntervalTime().printTime());
			SetItemText(nIndex, colMeasure, CalloutDispSpecDataItem::m_sMessureTypeStrings[pDataItem->GetMessure()]);
			SetItemText(nIndex, colConnection, CalloutDispSpecDataItem::m_sConnectionTypeStrings[pDataItem->GetConnection()]);
		}
		SetItemState(0, LVIS_SELECTED, LVIS_SELECTED );
	}
	
}

void CCalloutDispSpecsListCtrl::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	int index;
	int column;
	if((index = HitTestEx(point, &column)) != -1)
	{
		CalloutDispSpecDataItem* pDataItem = (CalloutDispSpecDataItem*)GetItemData(index);
		CStringList& dropList = ((LVCOLDROPLIST*)ddStyleList[column])->List;
		dropList.RemoveAll();
		switch (column)
		{
		case colEnabled:
			{
				// no action
			}
			break;
		case colEnvMode:
			{
				for(int i=0;i<CalloutDispSpecDataItem::EnvModeMaxLimit;i++)
				{
					dropList.AddTail(CalloutDispSpecDataItem::m_sEnvModeStrings[i]);
				}
			}
			break;
		case colFacility:
			{
				for(int i=CalloutDispSpecDataItem::m_sFacilityStart[pDataItem->GetEnvMode()];
					i<CalloutDispSpecDataItem::m_sFacilityEnd[pDataItem->GetEnvMode()];
					i++)
				{
					dropList.AddTail(CalloutDispSpecDataItem::m_sFacilityStrings[i]);
				}
			}
			break;
		case colClient:
			{
				bool bModified = false;
				switch (pDataItem->GetClientType())
				{
				case CalloutDispSpecDataItem::Client_FlightType:
					{
						FlightConstraint fltType;
						if (m_pSelFltType(NULL, fltType))
						{
							pDataItem->SetFltType(fltType);
							bModified = true;
						}
					}
					break;
				case CalloutDispSpecDataItem::Client_GSEType:
					{
						CGSESelectDlg dlg(m_nProjID);
						if (IDOK == dlg.DoModal())
						{
							//
							pDataItem->SetGSEID(dlg.GetSelGSEID());
							bModified = true;
						}
					}
					break;
				case CalloutDispSpecDataItem::Client_VehicleType:
					{
						// not implemented
					}
					break;
				case CalloutDispSpecDataItem::Client_MobileElementType:
					{
						CPassengerTypeDialog dlg(this);
						if (IDOK == dlg.DoModal())
						{
							pDataItem->SetMobileElementType(dlg.GetMobileSelection());
							bModified = true;
						}
					}
					break;
				case CalloutDispSpecDataItem::Client_MobileElement:
					{
						CPassengerType paxType(m_pInTerminal->inStrDict);
						CDlgNewPassengerType dlg(&paxType, m_pInTerminal);
						if (dlg.DoModal() == IDOK)
						{
							pDataItem->SetOnboardPaxType(paxType);
							bModified = true;
						}
					}
					break;
				case CalloutDispSpecDataItem::Client_NonPaxMobileElement:
					{
						// note implemented
					}
					break;
				default:
					{
						ASSERT(FALSE);
					}
					break;
				}
				if (bModified)
				{
					SetItemText(index, colClient, pDataItem->GetClientText());
					Invalidate();
				}
			}
			break;
		case colStartTime:
			{
				if (pDataItem->GetEnvMode() == EnvMode_OnBoard)
				{
					for(int i=0;i<CalloutDispSpecDataItem::StartTimeMode_TypeCount;i++)
					{
						dropList.AddTail(CalloutDispSpecDataItem::m_sStartTimeModeStrings[i]);
					}
				} 
				else
				{
					EditTimeRange(pDataItem);
					SetItemText(index, colStartTime, pDataItem->GetStartTimeString());
					SetItemText(index, colEndTime, pDataItem->GetEndTimeString());
					return;  // must return to avoid droplist display
				}
			}
			break;
		case colEndTime:
			{
				if (pDataItem->GetEnvMode() == EnvMode_OnBoard)
				{
					for(int i=0;i<CalloutDispSpecDataItem::EndTimeMode_TypeCount;i++)
					{
						dropList.AddTail(CalloutDispSpecDataItem::m_sEndTimeModeStrings[i]);
					}
				} 
				else
				{
					EditTimeRange(pDataItem);
					SetItemText(index, colStartTime, pDataItem->GetStartTimeString());
					SetItemText(index, colEndTime, pDataItem->GetEndTimeString());
					return;  // must return to avoid droplist display
				}
			}
			break;
		case colIntervalTime:
			{
				CDlgTimePicker dlg(pDataItem->GetIntervalTime(), _T("Set Interval Time"));
				MoveDialogToCursor(dlg);
				if (IDOK == dlg.DoModal())
				{
					pDataItem->SetIntervalTime(dlg.GetTime());
					SetItemText(index, colIntervalTime, pDataItem->GetIntervalTime().printTime());
					Invalidate();
				}
			}
			break;
		case colMeasure:
			{
				for(int i=0;i<CalloutDispSpecDataItem::MessureMappingArrayMaxCount;i++)
				{
					int nMeassureType = CalloutDispSpecDataItem::m_sMessureTypeOfFacilities[pDataItem->GetFacilityType()][i];
					if (-1 == nMeassureType)
					{
						break;
					}
					dropList.AddTail(CalloutDispSpecDataItem::m_sMessureTypeStrings[nMeassureType]);
				}
			}
			break;
		case colConnection:
			{
				for(int i=0;i<CalloutDispSpecDataItem::Connection_TypeCount;i++)
				{
					dropList.AddTail(CalloutDispSpecDataItem::m_sConnectionTypeStrings[i]);
				}
			}
			break;
		default:
			{
				ASSERT(FALSE);
			}
			break;
		}
	}
	CListCtrlEx::OnLButtonDblClk(nFlags, point);
}

void CCalloutDispSpecsListCtrl::EditTimeRange( CalloutDispSpecDataItem* pDataItem )
{
	CDlgTimeRange dlg(pDataItem->GetStartTime(), pDataItem->GetEndTime());
	MoveDialogToCursor(dlg);
	if (IDOK == dlg.DoModal())
	{
		pDataItem->SetStartTime(dlg.GetStartTime());
		pDataItem->SetEndTime(dlg.GetEndTime());
		Invalidate();
	}
}

void CCalloutDispSpecsListCtrl::MoveDialogToCursor( CDialog& dlg )
{
// 	CPoint pt;
// 	::GetCursorPos(&pt);
// 	CRect rc;
// 	dlg.GetWindowRect(rc);
// 
// 	rc.MoveToXY(pt.x - rc.Width()/2, pt.y - rc.Height()/2);
// 	ScreenToClient(rc);
// 
// 	dlg.MoveWindow(rc);
}

void CCalloutDispSpecsListCtrl::DrawTypeValuePairText( CString strText, CDC* pDC, CRect &rcClient, BOOL bSelected )
{
	int nFound = strText.Find(':');
	if (-1 == nFound)
	{
		pDC->DrawText(strText, rcClient, DT_LEFT | DT_VCENTER | DT_SINGLELINE );
	}
	else
	{
		CString strLeft = strText.Left(nFound + 1);
		pDC->DrawText(strLeft, rcClient, DT_LEFT | DT_VCENTER | DT_SINGLELINE );

		CSize sz = pDC->GetTextExtent(strLeft);
		rcClient.DeflateRect(sz.cx, 0, 0, 0);
		CString strRight = strText.Right(strText.GetLength() - nFound - 1);

		COLORREF clrOld = pDC->GetTextColor();
		pDC->SetTextColor(bSelected ? RGB(255, 255, 0) : RGB(0, 0, 255));
		pDC->DrawText(strRight, rcClient, DT_LEFT | DT_VCENTER | DT_SINGLELINE );
		pDC->SetTextColor(clrOld);
	}
}