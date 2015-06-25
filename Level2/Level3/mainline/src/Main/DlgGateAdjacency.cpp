#include "stdafx.h"
#include "TermPlan.h"
#include "DlgGateAdjacency.h"
#include "afxdialogex.h"
#include "DlgGateSelect.h"


DlgGateAdjacency::DlgGateAdjacency( CGateAssignPreferenceMan* pGateMan, 
                                    InputTerminal* _pInputTerm,
                                    CWnd* pParent)
 :  CToolTipDialog(IDD, pParent), 
    m_pGateMan(pGateMan),
    m_pInputTerm(_pInputTerm)
{
    m_vGateAdjas = *pGateMan->GetGateAdjacency();
}

DlgGateAdjacency::~DlgGateAdjacency()
{
}

void DlgGateAdjacency::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_GATE_ADJACENCY, m_wndListCtrl);
}

BEGIN_MESSAGE_MAP(DlgGateAdjacency, CDialog)
    ON_WM_SIZE()
    ON_WM_CREATE()
    ON_WM_GETMINMAXINFO()
    ON_COMMAND(ID_TOOLBARBUTTONADD, OnToolbarAdd)
    ON_COMMAND(ID_TOOLBARBUTTONDEL, OnToolbarDelete)
    ON_COMMAND(ID_TOOLBARBUTTONEDIT, OnToolBarEdit)
    ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
    ON_MESSAGE(WM_COLLUM_INDEX, OnCollumnIndex)
    ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_GATE_ADJACENCY, OnLvnEndlabeleditListContents)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_GATE_ADJACENCY, OnLvnSelItemchangedList)
END_MESSAGE_MAP()

int DlgGateAdjacency::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CToolTipDialog::OnCreate(lpCreateStruct) == -1)
        return -1;

    if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
        | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
        !m_wndToolBar.LoadToolBar(IDR_ADDDELEDITTOOLBAR))
    {
        return -1;
    }
    return 0;
}

BOOL DlgGateAdjacency::OnInitDialog()
{
    CDialog::OnInitDialog();

    GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);

    // set list control window style
    DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
    dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
    m_wndListCtrl.SetExtendedStyle(dwStyle);

    // set the layout of child controls
    RefreshLayout();

    // init the list control
    InitListControl();

    // set the toolbar button state
    UpdateToolBarState();

    return TRUE;
}

// refresh the layout of child controls
void DlgGateAdjacency::RefreshLayout(int cx, int cy)
{
    if (cx == -1 && cy == -1)
    {
        CRect rectClient;
        GetClientRect(&rectClient);
        cx = rectClient.Width();
        cy = rectClient.Height();
    }

    CWnd* pWndSave = GetDlgItem(IDC_BUTTON_SAVE);
    CWnd* pWndOK = GetDlgItem(IDOK);
    CWnd* pWndCancel = GetDlgItem(IDCANCEL);
    if (pWndSave == NULL || pWndOK == NULL || pWndCancel == NULL)
        return;

    CRect rcCancel;
    pWndCancel->GetClientRect(&rcCancel);
    pWndCancel->MoveWindow(cx - rcCancel.Width() - 10,
        cy - rcCancel.Height() - 10, 
        rcCancel.Width(), 
        rcCancel.Height());

    CRect rcOK;
    pWndOK->GetClientRect(&rcOK);
    pWndOK->MoveWindow(cx - rcCancel.Width() - rcOK.Width() - 20,
        cy - rcOK.Height() - 10, 
        rcOK.Width(),
        rcOK.Height());

    CRect rcSave;
    pWndSave->GetClientRect(&rcSave);
    pWndSave->MoveWindow(cx - rcCancel.Width() - rcOK.Width() - rcSave.Width() - 30,
        cy - rcSave.Height() - 10, 
        rcSave.Width(),
        rcSave.Height());

    m_wndListCtrl.MoveWindow(10, 35, cx - 20, cy - rcOK.Height() - 57);

    if (!::IsWindow(m_wndToolBar.m_hWnd))
        return;
    m_wndToolBar.MoveWindow(11, 11, cx - 25, 22);

    Invalidate();
}


void DlgGateAdjacency::InitListControl()
{
    if (!::IsWindow(m_wndListCtrl.m_hWnd))
        return;

    // set the list header contents;
    CStringList strList;
    LV_COLUMNEX lvc;
    lvc.csList = &strList;
    lvc.mask = LVCF_WIDTH|LVCF_TEXT;
    lvc.pszText = _T("Original Gate");
    lvc.cx = 150;
    lvc.fmt = LVCFMT_NOEDIT;
    m_wndListCtrl.InsertColumn(0, &lvc);

    lvc.pszText = _T("Adjacency Gate");
    lvc.cx = 150;
    lvc.fmt = LVCFMT_NOEDIT;
    m_wndListCtrl.InsertColumn(1, &lvc);

    lvc.pszText = _T("Reciprocate");
    strList.AddTail(_T("Yes"));
    strList.AddTail(_T("No"));
    lvc.cx = 80;
    lvc.fmt = LVCFMT_DROPDOWN;
    m_wndListCtrl.InsertColumn(2, &lvc);
    strList.RemoveAll();

    int nConstraintCount = (int)m_vGateAdjas.size();
    for (int i = 0; i < nConstraintCount; i++)
    {
        AddGateAdjacencyItem(m_vGateAdjas.at(i));
    }
}

void DlgGateAdjacency::UpdateToolBarState()
{
    if (!::IsWindow(m_wndToolBar.m_hWnd) || !::IsWindow(m_wndListCtrl.m_hWnd))
        return;

    m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD, TRUE);
    m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL, FALSE);
    m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONEDIT, FALSE);

    // delete & edit button
    POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
    if (pos != NULL)
    {
        m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL, TRUE);
        m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONEDIT, TRUE);
    }
}

void DlgGateAdjacency::OnSize(UINT nType, int cx, int cy)
{
    RefreshLayout(cx, cy);
}

void DlgGateAdjacency::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
    lpMMI->ptMinTrackSize.x = 400;
    lpMMI->ptMinTrackSize.y = 200;

    CDialog::OnGetMinMaxInfo(lpMMI);
}

void DlgGateAdjacency::AddGateAdjacencyItem(const CGateAdjacency* pGateAdj)
{
    int iNewIndex = m_wndListCtrl.InsertItem(m_wndListCtrl.GetItemCount(), NULL);
    m_wndListCtrl.SetItemData(iNewIndex, reinterpret_cast<DWORD>(pGateAdj));
    SetListItemContent(iNewIndex, pGateAdj);

    CGateAdjacency* pGateAdja = (CGateAdjacency*)m_wndListCtrl.GetItemData(iNewIndex);
}

void DlgGateAdjacency::SetListItemContent(int nIndex, const CGateAdjacency* pGateAdj)
{
    m_wndListCtrl.SetItemText(nIndex, 0, pGateAdj->GetOriginalGate().GetIDString());
    m_wndListCtrl.SetItemText(nIndex, 1, pGateAdj->GetAdjacentGate().GetIDString());
    if(pGateAdj->GetReciprocate())
    {
        m_wndListCtrl.SetItemText(nIndex, 2, "Yes");
    }
    else
    {
        m_wndListCtrl.SetItemText(nIndex, 2, "No");
    }
}

void DlgGateAdjacency::OnOK()
{
    OnButtonSave();
    CToolTipDialog::OnOK();
}

void DlgGateAdjacency::OnCancel()
{
    int nCount = (int)m_vGateAdjas.size();
    for(int i=0; i<nCount; i++)
    {
        if(m_pGateMan->FindGateAdjacencyIndex(m_vGateAdjas.at(i)) == -1)
            delete m_vGateAdjas.at(i);
    }

    nCount = (int)m_vDelGateAdjas.size();
    for(int i=0; i<nCount; i++)
    {
        if(m_pGateMan->FindGateAdjacencyIndex(m_vDelGateAdjas.at(i)) == -1)
            delete m_vDelGateAdjas.at(i);
    }
    CToolTipDialog::OnCancel();
}

void DlgGateAdjacency::OnButtonSave()
{
    *m_pGateMan->GetGateAdjacency() = m_vGateAdjas;
    m_pGateMan->SaveData();
    GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
    int nCount = (int)m_vDelGateAdjas.size();
    for(int i=0; i<nCount; i++)
    {
        delete m_vDelGateAdjas.at(i);
    }
    m_vDelGateAdjas.clear();
}

void DlgGateAdjacency::OnToolbarAdd()
{
    CDlgGateSelect dlg(m_pInputTerm, m_Type);
    while(dlg.DoModal() == IDOK)
    {
        ProcessorID gateID;
        gateID.SetStrDict(m_pInputTerm->inStrDict);
        gateID.setID(dlg.GetSelGateName());
        CGateAdjacency* pGateAdj = new CGateAdjacency();
        pGateAdj->SetOriginalGate(gateID);
        pGateAdj->SetAdjacentGate(gateID);
        pGateAdj->SetReciprocate(true);
        int nResult = FindGateAdjacency(*pGateAdj);
        if(nResult != -1)
        {
            CString strErr;
            strErr.Format("Gate Adjacency %s - %s already exist.", 
                pGateAdj->GetOriginalGate().GetIDString(), pGateAdj->GetAdjacentGate().GetIDString());
            MessageBox(strErr);
            continue;
        }
        m_vGateAdjas.push_back(pGateAdj);
        AddGateAdjacencyItem(pGateAdj);
        GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow();
        break;
    }
}

void DlgGateAdjacency::OnToolbarDelete()
{
    int nSelItem = m_wndListCtrl.GetCurSel();
    if (nSelItem < 0)
        return;

    CGateAdjacency* pGateAdja = (CGateAdjacency*)m_wndListCtrl.GetItemData(nSelItem);
    int nCount = (int)m_vGateAdjas.size();
    for(int i=0; i<nCount; i++)
    {
        if(m_vGateAdjas.at(i) == pGateAdja)
        {
            m_vDelGateAdjas.push_back(pGateAdja);
            m_vGateAdjas.erase(m_vGateAdjas.begin() + i);
        }
    }
    m_wndListCtrl.DeleteItem(nSelItem);
    UpdateToolBarState();
    GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow();
}

void DlgGateAdjacency::OnToolBarEdit() // This action will edit Original Gate.
{
    int nSelItem = m_wndListCtrl.GetCurSel();
    if (nSelItem < 0)
        return;

    CGateAdjacency* pGateAdja = (CGateAdjacency*)m_wndListCtrl.GetItemData(nSelItem);
    if(pGateAdja == NULL)
        return;
    CDlgGateSelect dlg(m_pInputTerm, m_Type);
    while(dlg.DoModal() == IDOK)
    {
        ProcessorID gateID;
        gateID.SetStrDict(m_pInputTerm->inStrDict);
        gateID.setID(dlg.GetSelGateName());

        CGateAdjacency tempGateAdja = *pGateAdja;
        tempGateAdja.SetOriginalGate(gateID);
        int nResult = FindGateAdjacency(tempGateAdja);
        if(nResult != -1 && pGateAdja != m_vGateAdjas.at(nResult))
        {
            CString strErr;
            strErr.Format("Gate Adjacency %s - %s already exist.", 
                tempGateAdja.GetOriginalGate().GetIDString(), tempGateAdja.GetAdjacentGate().GetIDString());
            MessageBox(strErr);
            continue;
        }
        pGateAdja->SetOriginalGate(gateID);
        SetListItemContent(nSelItem, pGateAdja);
        GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow();
        break;
    }
}

LRESULT DlgGateAdjacency::OnCollumnIndex(WPARAM wParam,  LPARAM lParam)
{
    switch((int)lParam)
    {
    case 0: // Edit column is Original Gate
        {
            OnToolBarEdit();
            break;
        }
    case 1: // Edit column is Adjacent Gate
        {
            int nSelItem = m_wndListCtrl.GetCurSel();
            if (nSelItem < 0)
                return -1;
            CGateAdjacency* pGateAdja = (CGateAdjacency*)m_wndListCtrl.GetItemData(nSelItem);
            CDlgGateSelect dlg(m_pInputTerm, m_Type);
            while(dlg.DoModal() == IDOK)
            {
                ProcessorID gateID;
                gateID.SetStrDict(m_pInputTerm->inStrDict);
                gateID.setID(dlg.GetSelGateName());
                CGateAdjacency tempGateAdja = *pGateAdja;
                tempGateAdja.SetAdjacentGate(gateID);
                int nResult = FindGateAdjacency(tempGateAdja);
                if(nResult != -1 && pGateAdja != m_vGateAdjas.at(nResult))
                {
                    CString strErr;
                    strErr.Format("Gate Adjacency %s - %s already exist.", 
                        tempGateAdja.GetOriginalGate().GetIDString(), tempGateAdja.GetAdjacentGate().GetIDString());
                    MessageBox(strErr);
                    continue;
                }

                pGateAdja->SetAdjacentGate(gateID);
                SetListItemContent(nSelItem, pGateAdja);
                GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow();
                break;
            }
            break;
        }
    case 2:
        break;
    default:
        return -1;
    }
    return 0;
}

int DlgGateAdjacency::FindGateAdjacency(const CGateAdjacency& gateAdj)
{
    int nCount = (int)m_vGateAdjas.size();
    for(int i=0; i<nCount; i++)
    {
        if(*m_vGateAdjas.at(i) == gateAdj)
        {
            return i;
        }
    }
    return -1;
}

void DlgGateAdjacency::OnLvnSelItemchangedList(NMHDR *pNMHDR, LRESULT *pResult)
{
    UpdateToolBarState();
}

void DlgGateAdjacency::OnLvnEndlabeleditListContents(NMHDR *pNMHDR, LRESULT *pResult)
{
    m_wndListCtrl.OnEndlabeledit(pNMHDR, pResult);
    LV_DISPINFO* plvDispInfo = (LV_DISPINFO *)pNMHDR;
    LV_ITEM* plvItem = &plvDispInfo->item;

    int nItem = plvItem->iItem;
    if (nItem < 0)
        return;

    CGateAdjacency* pGateAdja = (CGateAdjacency*)m_wndListCtrl.GetItemData(nItem);
    if(pGateAdja == NULL)
        return;
    CString strValue = m_wndListCtrl.GetItemText(nItem, plvItem->iSubItem);
    int i = 0;
    switch(plvItem->iSubItem)
    {
    case 0:
    case 1:
        break;
    case 2:
        {
            if(strValue.Compare(_T("Yes")) == 0)
                pGateAdja->SetReciprocate(true);
            else
                pGateAdja->SetReciprocate(false);
        }
        break;
    default:
        break;
    }

    SetListItemContent(nItem, pGateAdja);
    GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow();

    *pResult = 0;
}


