// ListCtrlTestLvnItemChanged.cpp : implementation file
//

#include "stdafx.h"
#include "TestUnicodeIO.h"
#include "ListCtrlTestLvnItemChanged.h"


// CListCtrlTestLvnItemChanged

IMPLEMENT_DYNAMIC(CListCtrlTestLvnItemChanged, CListCtrl)

CListCtrlTestLvnItemChanged::CListCtrlTestLvnItemChanged()
{

}

CListCtrlTestLvnItemChanged::~CListCtrlTestLvnItemChanged()
{
}


BEGIN_MESSAGE_MAP(CListCtrlTestLvnItemChanged, CListCtrl)
    ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnLvnItemchanged)
END_MESSAGE_MAP()

void CListCtrlTestLvnItemChanged::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    CString strDebugLog;
    TRACE(_T("CListCtrlKeepHighlight::OnLvnItemchanged()\r\n"));
    strDebugLog.Format(_T("select item: %d, old state: %d, new state: %d\r\n"), pNMLV->iItem, pNMLV->uOldState, pNMLV->uNewState);
    TRACE(strDebugLog);
    if (pNMLV->uChanged & LVIF_STATE)
    {
        if (((pNMLV->uOldState & LVIS_SELECTED) != (pNMLV->uNewState & LVIS_SELECTED)) 
            || ((pNMLV->uOldState & LVIS_STATEIMAGEMASK) != (pNMLV->uNewState & LVIS_STATEIMAGEMASK)))
        {
            //InvalidateItem(pNMLV->iItem);
        }
    }

    for(int i=0; i<10; i++)
    {
        int itemState = GetItemState(i, 7);
        CString strDbgLog;
        strDbgLog.Format(_T("item: %d, state: %d\r\n"), i, itemState);
        TRACE(strDbgLog);
    }

    *pResult = 0;
}


// CListCtrlTestLvnItemChanged message handlers


