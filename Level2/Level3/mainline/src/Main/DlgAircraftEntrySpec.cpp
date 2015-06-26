#include "stdafx.h"
#include "TermPlan.h"
#include "Common\WinMsg.h"
#include "Common\strdict.h"
#include "DlgAircraftEntrySpec.h"
#include "afxdialogex.h"
#include "Engine\PROCLIST.H"
#include "TermPlanDoc.h"
#include "Common\ProbabilityDistribution.h"
#include "DestributionParameterSpecificationDlg.h"
#include "PassengerTypeDialog.h"
#include "Inputs\PROBAB.H"
#include "DlgProbDist.h"

const static COLORREF SELECTED_COLOR = RGB(53, 151, 53);
static UniformDistribution u2_10(2.0f, 10.0f);

IMPLEMENT_DYNAMIC(CAircraftEntryProcessorDlg, CDialog)

    CAircraftEntryProcessorDlg::CAircraftEntryProcessorDlg(InputTerminal* _pInTerm, CWnd* pParent)
    : CDialog(CAircraftEntryProcessorDlg::IDD, pParent)
    , m_pInTerm(_pInTerm)
    , m_pACEntryData(NULL)
    , m_oldCx(0)
    , m_oldCy(0)
    , m_bDragging(false)
{
}

CAircraftEntryProcessorDlg::~CAircraftEntryProcessorDlg()
{
}

void CAircraftEntryProcessorDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TREE_ENTRYPROC, m_procTree);
    DDX_Control(pDX, IDC_LIST_PAXTYPE, m_paxList);
}


BEGIN_MESSAGE_MAP(CAircraftEntryProcessorDlg, CDialog)
    ON_WM_SIZE()
    ON_WM_CREATE()
    ON_WM_GETMINMAXINFO()
    ON_NOTIFY_EX(TTN_NEEDTEXTA, 0, OnToolTipText)
    ON_BN_CLICKED(IDOK, OnOK)
    ON_BN_CLICKED(IDCANCEL, OnCancel)
    ON_BN_CLICKED(IDC_BTN_SAVE, OnSave)
    ON_COMMAND(ID_TOOLBAR_ADD, OnToolbarButtonAdd)
    ON_COMMAND(ID_TOOLBAR_DEL, OnToolbarButtonDel)
    ON_NOTIFY(NM_RCLICK, IDC_LIST_PAXTYPE, OnRightClickListItem)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PAXTYPE, OnSelChangedPaxList)
    ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_ENTRYPROC, OnSelChangedTree)
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

int CAircraftEntryProcessorDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDialog::OnCreate(lpCreateStruct) == -1)
        return -1;

    if (!m_toolbarPaxType.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
        | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
        !m_toolbarPaxType.LoadToolBar(IDR_ADDDELTOOLBAR))
    {
        TRACE0("Failed to create toolbar\n");
        return -1;
    }
    return 0;
}

BOOL CAircraftEntryProcessorDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();

    GetDlgItem(IDC_BTN_SAVE)->EnableWindow(FALSE);

    m_pACEntryData = m_pInTerm->m_pACEntryData;
    m_vIncType.push_back(BridgeConnectorProc);
    LoadProcTree();

    CRect rect;
    m_paxList.GetWindowRect(rect);
    ScreenToClient(rect);
    m_toolbarPaxType.SetWindowPos(NULL, rect.left+1, rect.top-26, rect.Width()-2, 26, NULL);
    DisableAllToolBarButtons();

    DWORD dwStyle = m_paxList.GetExtendedStyle();
    dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
    m_paxList.SetExtendedStyle(dwStyle);

    InitPaxTypeListHeader();
    return TRUE;
}

void CAircraftEntryProcessorDlg::LoadProcTree()
{
    AfxGetApp()->BeginWaitCursor();
    if(m_pInTerm == NULL)
        return;
    ProcessorList *procList = m_pInTerm->GetTerminalProcessorList();
    if (procList == NULL)
        return;

    COOLTREENODEINFO cni;
    CCoolTree::InitNodeInfo(this, cni);
    ProcessorID id;
    id.SetStrDict(m_pInTerm->inStrDict);
    CString strAll = _T("ALL PROCESSORS");
    id.setID(strAll);
    m_vProcs.push_back(id);
    ACEntryTimeDistDatabase* pEntryTimeDB = m_pACEntryData->getEntryTimeDB();
    std::vector<AircraftEntryProcsEntry*> vData = pEntryTimeDB->FindEntryByProcID(id);
    if(!vData.empty())
    {
        cni.clrItem = SELECTED_COLOR;
        cni.lfontItem.lfWeight = 700;
    }
    else
    {
        CCoolTree::InitNodeInfo(this, cni);
    }
    HTREEITEM hRoot = m_procTree.InsertItem(strAll, cni, FALSE);
    m_procTree.SetItemData(hRoot, (DWORD)(m_vProcs.size()-1));

    StringList strDict;
    for(size_t level1=0; level1<m_vIncType.size(); level1++)
        procList->getAllGroupNames(strDict, m_vIncType[level1]);

    for(int level1=0; level1<strDict.getCount(); level1++)
    {
        CString strLevel1 = strDict.getString(level1);
        strLevel1.Trim();
        if (strLevel1.IsEmpty())
            continue;
        
        CString strProcID = strLevel1;
        id.setID(strProcID);
        m_vProcs.push_back(id);

        vData = pEntryTimeDB->FindEntryByProcID(id);
        if(!vData.empty())
        {
            cni.clrItem = SELECTED_COLOR;
            cni.lfontItem.lfWeight = 700;
        }
        else
        {
            CCoolTree::InitNodeInfo(this, cni);
        }
        HTREEITEM hLevel1 = m_procTree.InsertItem(strProcID, cni, FALSE, FALSE, hRoot);
        m_procTree.SetItemData(hLevel1, (DWORD)(m_vProcs.size()-1));

        StringList strDictLevel2;
        for(int level2=0; level2<static_cast<int>(m_vIncType.size()); level2++)
            procList->getMemberNames(id, strDictLevel2, m_vIncType[level2]);

        int bExpandL1 = TVE_COLLAPSE;
        for(int level2=0; level2<strDictLevel2.getCount(); level2++)
        {
            CString strLevel2 = strDictLevel2.getString (level2);
            strLevel2.Trim();
            if (strLevel2.IsEmpty())
                continue;

            strProcID = strLevel1 + "-" + strLevel2;
            id.setID((LPCSTR)strProcID);
            m_vProcs.push_back(id);

            vData = pEntryTimeDB->FindEntryByProcID(id);
            if(!vData.empty())
            {
                bExpandL1 = TVE_EXPAND;
                cni.clrItem = SELECTED_COLOR;
                cni.lfontItem.lfWeight = 700;
            }
            else
            {
                CCoolTree::InitNodeInfo(this, cni);
            }
            HTREEITEM hLevel2 = m_procTree.InsertItem(strProcID, cni, FALSE, FALSE, hLevel1);
            m_procTree.SetItemData(hLevel2, (DWORD)(m_vProcs.size()-1));

            StringList strDictLevel3;
            for(int level3=0; level3<static_cast<int>(m_vIncType.size()); level3++)
                procList->getMemberNames(id,strDictLevel3, m_vIncType[level3]);

            int bExpandL2 = TVE_COLLAPSE;
            for(int level3=0; level3<strDictLevel3.getCount(); level3++)
            {
                CString strLevel3 = strDictLevel3.getString(level3);
                strLevel3.Trim();
                if (strLevel3.IsEmpty())
                    continue;

                strProcID = strLevel1 + "-" + strLevel2 + "-" + strLevel3;
                id.setID((LPCSTR)strProcID);
                m_vProcs.push_back(id);

                vData = pEntryTimeDB->FindEntryByProcID(id);
                if(!vData.empty())
                {
                    bExpandL1 = TVE_EXPAND;
                    bExpandL2 = TVE_EXPAND;
                    cni.clrItem = SELECTED_COLOR;
                    cni.lfontItem.lfWeight = 700;
                }
                else
                {
                    CCoolTree::InitNodeInfo(this, cni);
                }
                HTREEITEM hLevel3 = m_procTree.InsertItem(strProcID, cni, FALSE, FALSE, hLevel2);
                m_procTree.SetItemData(hLevel3, (DWORD)(m_vProcs.size()-1));

                StringList strDictL4;
                for(int level4=0; level4<static_cast<int>(m_vIncType.size()); level4++)
                    procList->getMemberNames(id, strDictL4, m_vIncType[level4]);

                int bExpandL3 = TVE_COLLAPSE;
                for (int level4=0; level4<strDictL4.getCount(); level4++)
                {
                    CString strLevel4 = strDictL4.getString(level4);
                    strLevel4.Trim();
                    if (strLevel4.IsEmpty())
                        continue;

                    strProcID = strLevel1 + "-" + strLevel2 + "-" + strLevel3 + "-" + strLevel4;
                    id.setID((LPCSTR)strProcID);
                    m_vProcs.push_back(id);

                    vData = pEntryTimeDB->FindEntryByProcID(id);
                    if(!vData.empty())
                    {
                        bExpandL1 = TVE_EXPAND;
                        bExpandL2 = TVE_EXPAND;
                        bExpandL3 = TVE_EXPAND;
                        cni.clrItem = SELECTED_COLOR;
                        cni.lfontItem.lfWeight = 700;
                    }
                    else
                    {
                        CCoolTree::InitNodeInfo(this, cni);
                    }
                    HTREEITEM hLevel4 = m_procTree.InsertItem(strProcID, cni, FALSE, FALSE, hLevel3);
                    m_procTree.SetItemData(hLevel4, (DWORD)(m_vProcs.size()-1));
                }
                m_procTree.Expand(hLevel3, bExpandL3);
            }
            m_procTree.Expand(hLevel2, bExpandL2);
        }
        m_procTree.Expand(hLevel1, bExpandL1);
    }
    m_procTree.Expand(hRoot, TVE_EXPAND);
    AfxGetApp()->EndWaitCursor();
    return;
}

void CAircraftEntryProcessorDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);
    CWnd* pCtrl = GetDlgItem(IDOK);
    if(pCtrl == NULL)
    {
        if(nType != SIZE_MINIMIZED)
        {
            m_oldCx = cx;
            m_oldCy = cy;
        }
        return;
    }

    LayoutControl(&m_procTree, TopLeft,  BottomLeft, cx, cy);
    LayoutControl(&m_paxList, TopLeft,  BottomRight, cx, cy);
    LayoutControl(&m_toolbarPaxType, TopLeft,  TopRight, cx, cy);
    LayoutControl(GetDlgItem(IDC_STATIC_ENTRYPROC), TopLeft, TopLeft, cx, cy);
    LayoutControl(GetDlgItem(IDC_STATIC_PASSENGER), TopLeft, TopLeft, cx, cy);
    LayoutControl(GetDlgItem(IDC_STATIC_PAXLIST), TopLeft,  BottomRight, cx, cy);
    LayoutControl(GetDlgItem(IDC_BTN_SAVE), BottomRight,  BottomRight, cx, cy);
    LayoutControl(GetDlgItem(IDOK), BottomRight,  BottomRight, cx, cy);
    LayoutControl(GetDlgItem(IDCANCEL), BottomRight,  BottomRight, cx, cy);

    if(nType != SIZE_MINIMIZED)
    {
        m_oldCx = cx;
        m_oldCy = cy;
    }
    InvalidateRect(NULL);
}
void CAircraftEntryProcessorDlg::LayoutControl(CWnd* pCtrl, LayoutRef refTopLeft, LayoutRef refBottomRight, int cx, int cy)
{
    CRect rcS;
    pCtrl->GetWindowRect(&rcS);
    ScreenToClient(&rcS);
    int deltaX = cx - m_oldCx;
    int deltaY = cy - m_oldCy;
    if(refTopLeft == TopLeft && refBottomRight == TopLeft)
    {
        pCtrl->MoveWindow(&rcS);
    }
    else if(refTopLeft == TopLeft && refBottomRight == TopRight)
    {
        pCtrl->MoveWindow(rcS.left, rcS.top, rcS.Width()+deltaX, rcS.Height());
    }
    else if(refTopLeft == TopLeft && refBottomRight == BottomLeft)
    {
        pCtrl->MoveWindow(rcS.left, rcS.top, rcS.Width(), rcS.Height()+deltaY);
    }
    else if(refTopLeft == TopLeft && refBottomRight == BottomRight)
    {
        pCtrl->MoveWindow(rcS.left, rcS.top, rcS.Width()+deltaX, rcS.Height()+deltaY);
    }
    else if(refTopLeft == TopRight && refBottomRight == TopRight)
    {
        pCtrl->MoveWindow(rcS.left+deltaX, rcS.top, rcS.Width(), rcS.Height());
    }
    else if(refTopLeft == TopRight && refBottomRight == BottomRight)
    {
        pCtrl->MoveWindow(rcS.left+deltaX, rcS.top, rcS.Width(), rcS.Height()+deltaY);
    }
    else if(refTopLeft == BottomLeft && refBottomRight == BottomLeft)
    {
        pCtrl->MoveWindow(rcS.left, rcS.top+deltaY, rcS.Width(), rcS.Height());
    }
    else if(refTopLeft == BottomLeft && refBottomRight == BottomRight)
    {
        pCtrl->MoveWindow(rcS.left, rcS.top+deltaY, rcS.Width()+deltaX, rcS.Height());
    }
    else if(refTopLeft == BottomRight && refBottomRight == BottomRight)
    {
        pCtrl->MoveWindow(rcS.left+deltaX, rcS.top+deltaY, rcS.Width(), rcS.Height());
    }
}

CString CAircraftEntryProcessorDlg::GetProjPath()
{
    CTermPlanDoc* pDoc = (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
    return pDoc->m_ProjInfo.path;
}

void CAircraftEntryProcessorDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    lpMMI->ptMinTrackSize.x = 600;
    lpMMI->ptMinTrackSize.y = 360;
    CDialog::OnGetMinMaxInfo(lpMMI);
}

void CAircraftEntryProcessorDlg::DisableAllToolBarButtons()
{
    m_toolbarPaxType.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD, FALSE);
    m_toolbarPaxType.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL, FALSE);
}

void CAircraftEntryProcessorDlg::InitPaxTypeListHeader()
{
    LV_COLUMNEX lvc;
    CStringList strList;
    strList.RemoveAll();
    lvc.csList = &strList;
    lvc.mask = LVCF_WIDTH | LVCF_TEXT;
    // column 0
    lvc.fmt = LVCFMT_NOEDIT;
    lvc.cx = 160;
    lvc.pszText = _T("Passenger Type");
    m_paxList.InsertColumn(0, &lvc);

    // column 1
    lvc.fmt = LVCFMT_DROPDOWN;
    CString s;
    s.LoadString(IDS_STRING_NEWDIST);
    strList.InsertAfter(strList.GetTailPosition(), s);
    int nCount = _g_GetActiveProbMan(m_pInTerm)->getCount();
    for( int m=0; m<nCount; m++ )
    {
        CProbDistEntry* pPBEntry = _g_GetActiveProbMan(m_pInTerm)->getItem(m);
        strList.AddTail(pPBEntry->m_csName);
    }
    lvc.cx = 200;
    lvc.pszText = _T("Probability Distribution(SECONDS)");
    m_paxList.InsertColumn(1, &lvc);

    // column 2
    lvc.fmt = LVCFMT_DATETIME;
    lvc.cx = 80;
    lvc.pszText = _T("Begin Time");
    m_paxList.InsertColumn(2, &lvc);

    // column 3
    lvc.fmt = LVCFMT_DATETIME;
    lvc.cx = 80;
    lvc.pszText = _T("End Time");
    m_paxList.InsertColumn(3, &lvc);
}

BOOL CAircraftEntryProcessorDlg::OnToolTipText(UINT id, NMHDR *pNMHDR, LRESULT *pResult)
{
    TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
    UINT nID = pNMHDR->idFrom;
    if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND))
    {
        nID = ::GetDlgCtrlID((HWND)nID);
    }

    if(nID == 0)
        return FALSE;

    if (pNMHDR->code == TTN_NEEDTEXTA)
    {
        CString strTipText;
        switch(nID)
        {
        case ID_TOOLBAR_ADD:
            strTipText = _T("add");
            break;
        case ID_TOOLBAR_DEL:
            strTipText = _T("delete");
            break;
        default:
            break;
        }
        lstrcpyn(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
    }
    *pResult = 0; 
    ::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE| SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER);
    return TRUE;
}

void CAircraftEntryProcessorDlg::OnSelChangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    HTREEITEM hSelItem = m_procTree.GetSelectedItem();
    if(hSelItem == NULL)
    {
        m_paxList.DeleteAllItems();
        m_toolbarPaxType.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD, FALSE);
        *pResult = 0;
        return;
    }
    else
    {
        m_toolbarPaxType.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD, TRUE);
        // reload pax type list
        ReloadPaxTypeList(hSelItem);
    }
    *pResult = 0;
}

void CAircraftEntryProcessorDlg::OnRightClickListItem(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    if(pNMItemActivate->iItem == -1)
        return;

    if(pNMItemActivate->iSubItem == 0)
    {
        return;
    }
    else if(pNMItemActivate->iSubItem == 1)
    {
        ASSERT(pNMItemActivate->iItem != -1);
        int iItemData = (int)m_paxList.GetItemData(pNMItemActivate->iItem);
        if(iItemData == -1)
        {
            MessageBox(_T("Use toolbar button \"add\" please."));
            return;
        }
        else
        {
            AircraftEntryProcsEntry* pEntry = (AircraftEntryProcsEntry*)m_paxList.GetItemData(pNMItemActivate->iItem);
            ASSERT(pEntry != NULL);
            ProbabilityDistribution* pProb = pEntry->getValue();
            CDestributionParameterSpecificationDlg dlg(pProb, this);
            if(dlg.DoModal() == IDOK)
            {
                CProbDistEntry* pPDEntry = dlg.GetSelProbEntry();
                if(pPDEntry != NULL)
                {
                    ProbabilityDistribution* pProbDist = ProbabilityDistribution::CopyProbDistribution(pPDEntry->m_pProbDist);
                    ASSERT(pProbDist);
                    delete pProb;
                    pEntry->setValue(pProbDist);
                    CString strItem = pProbDist->screenPrint();
                    m_paxList.SetItemText(pNMItemActivate->iItem, pNMItemActivate->iSubItem, strItem);
                    GetDlgItem(IDC_BTN_SAVE)->EnableWindow(TRUE);
                }
            }
        }
    }
    *pResult = 0;
}

void CAircraftEntryProcessorDlg::OnSelChangedPaxList(NMHDR *pNMHDR, LRESULT *pResult)
{
    DisableAllToolBarButtons();
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    m_toolbarPaxType.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD, TRUE);

    POSITION pos = m_paxList.GetFirstSelectedItemPosition();
    if(pos != NULL)
    {
        int iItemData = (int)m_paxList.GetItemData((int)pos-1);
        if(iItemData != -1 && iItemData != 0)
            m_toolbarPaxType.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL, TRUE);
    }

    *pResult = 0;
}

void CAircraftEntryProcessorDlg::OnToolbarButtonAdd()
{
    CPassengerTypeDialog dlg(m_pParentWnd);
    if(dlg.DoModal() == IDOK)
    {
        HTREEITEM hItem = m_procTree.GetSelectedItem();
        ProcessorID id = m_vProcs.at((int)m_procTree.GetItemData(hItem));
        CMobileElemConstraint* pNewConst = new CMobileElemConstraint(dlg.GetMobileSelection());
        pNewConst->SetInputTerminal(m_pInTerm);
        ProbabilityDistribution* pNewProb = ProbabilityDistribution::CopyProbDistribution(&u2_10);
        AircraftEntryProcsEntry* pNewEntry = new AircraftEntryProcsEntry();
        pNewEntry->initialize(pNewConst, pNewProb, id, ElapsedTime(0L), ElapsedTime(WholeDay-60L));
        m_pACEntryData->getEntryTimeDB()->addEntry(pNewEntry);
        COOLTREENODEINFO* pCni = m_procTree.GetItemNodeInfo(hItem);
        pCni->clrItem = SELECTED_COLOR;
        pCni->lfontItem.lfWeight = 700;
        CRect recItem;
        m_procTree.GetItemRect(hItem, recItem, FALSE);
        m_procTree.InvalidateRect(&recItem);
        ReloadPaxTypeList(hItem);
        GetDlgItem(IDC_BTN_SAVE)->EnableWindow(TRUE);
    }
}

void CAircraftEntryProcessorDlg::OnToolbarButtonDel()
{
    POSITION pos = m_paxList.GetFirstSelectedItemPosition();
    ASSERT(pos > 0);
    int iItemData = (int)m_paxList.GetItemData((int)(pos-1));
    ASSERT(iItemData != -1);

    AircraftEntryProcsEntry* pEntry = (AircraftEntryProcsEntry*)m_paxList.GetItemData((int)(pos-1));
    ASSERT(pEntry != NULL);
    CMobileElemConstraint* pConst = (CMobileElemConstraint*)pEntry->getConstraint();
    m_pACEntryData->getEntryTimeDB()->DeleteEntry(pEntry);
    m_paxList.DeleteItem((int)(pos-1));
    int nItemCount = m_paxList.GetItemCount();
    if(nItemCount >= (int)pos)
    {
        m_paxList.SetItemState((int)pos-1,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
        m_paxList.SetFocus();
    }
    else if(nItemCount > 0)
    {
        m_paxList.SetItemState(nItemCount-1,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
        m_paxList.SetFocus();
    }

    HTREEITEM hSelItem = m_procTree.GetSelectedItem();
    ASSERT(hSelItem != NULL);
    int iProcID = (int)m_procTree.GetItemData(hSelItem);
    ProcessorID id = m_vProcs.at(iProcID);
    if(m_pACEntryData->getEntryTimeDB()->getEntryCountByProcID(id) == 0)
    {
        ProcessorID idDefault;
        idDefault.init();
        if(id == idDefault)
        {
            AircraftEntryProcsEntry* pEntry = new AircraftEntryProcsEntry();
            pEntry->useDefaultValue(m_pInTerm);
            m_pACEntryData->getEntryTimeDB()->addEntry(pEntry);
            ReloadPaxTypeList(hSelItem);
            m_paxList.SetItemState(0,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
            m_paxList.SetFocus();
        }
        else
        {
            ReloadPaxTypeList(hSelItem);
            COOLTREENODEINFO* pCni = m_procTree.GetItemNodeInfo(hSelItem);
            DWORD iData = m_procTree.GetItemData(hSelItem);
            CCoolTree::InitNodeInfo(this, *pCni);
            m_procTree.SetItemData(hSelItem, iData);
            CRect recItem;
            m_procTree.GetItemRect(hSelItem, recItem, FALSE);
            m_procTree.InvalidateRect(&recItem);
        }
    }
    GetDlgItem(IDC_BTN_SAVE)->EnableWindow(TRUE);
}

void CAircraftEntryProcessorDlg::OnSave()
{
    m_pACEntryData->saveDataSet(GetProjPath(), false);
    GetDlgItem(IDC_BTN_SAVE)->EnableWindow(FALSE);
}

void CAircraftEntryProcessorDlg::OnMouseMove(UINT nFlags, CPoint point)
{
    CRect recTree;
    m_procTree.GetWindowRect(&recTree);
    ScreenToClient(&recTree);

    CRect recList;
    m_paxList.GetWindowRect(&recList);
    ScreenToClient(&recList);
    if(!m_bDragging)
    {
        if(recTree.right<point.x && point.x<recList.left &&
            recTree.top<point.y && point.y<recTree.bottom)
        {
            HCURSOR hCur = LoadCursor(NULL , IDC_SIZEWE);
            ::SetCursor(hCur);
        }
        else
        {
            HCURSOR hCur = LoadCursor(NULL , IDC_ARROW);
            ::SetCursor(hCur);
        }
    }
    else
    {
        CRect recPaxStatic, recPaxListStatic;
        GetDlgItem(IDC_STATIC_PASSENGER)->GetWindowRect(&recPaxStatic);
        GetDlgItem(IDC_STATIC_PAXLIST)->GetWindowRect(&recPaxListStatic);
        ScreenToClient(recPaxStatic);
        ScreenToClient(recPaxListStatic);
        HCURSOR hCur = LoadCursor(NULL , IDC_SIZEWE);
        ::SetCursor(hCur);
        recList.left = point.x+(recList.left-recTree.right)-3;
        recTree.right = point.x-3;
        if(recTree.Width()>180 && recList.Width()>180)
        {
            m_procTree.MoveWindow(recTree);
            m_paxList.MoveWindow(recList);
            GetDlgItem(IDC_STATIC_PASSENGER)->MoveWindow(recList.left, recPaxStatic.top, recPaxStatic.Width(), recPaxStatic.Height());
            GetDlgItem(IDC_STATIC_PAXLIST)->MoveWindow(recList.left, recPaxListStatic.top, recList.Width(), recPaxListStatic.Height());
            m_toolbarPaxType.MoveWindow(recList.left+1, recList.top-26, recList.Width()-2, 26);
        }
    }
    CDialog::OnMouseMove(nFlags, point);
}


void CAircraftEntryProcessorDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    CRect recTree;
    m_procTree.GetWindowRect(&recTree);
    ScreenToClient(&recTree);

    CRect recList;
    m_paxList.GetWindowRect(&recList);
    ScreenToClient(&recList);

    if(recTree.right<point.x && point.x<recList.left &&
        recTree.top<point.y && point.y<recTree.bottom)
    {
        HCURSOR hCur = LoadCursor(NULL , IDC_SIZEWE);
        ::SetCursor(hCur);
        m_bDragging = true;
        SetCapture();
    }
    CDialog::OnLButtonDown(nFlags, point);
}


void CAircraftEntryProcessorDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    m_bDragging = false;
    ReleaseCapture();
    CDialog::OnLButtonUp(nFlags, point);
}


void CAircraftEntryProcessorDlg::OnOK()
{
    if(GetDlgItem(IDC_BTN_SAVE)->IsWindowEnabled())
    {
        AfxGetApp()->BeginWaitCursor();
        OnSave();
        AfxGetApp()->EndWaitCursor();
    }
    CDialog::OnOK();
}


void CAircraftEntryProcessorDlg::OnCancel()
{
    if(GetDlgItem(IDC_BTN_SAVE)->IsWindowEnabled())
    {
        AfxGetApp()->BeginWaitCursor();
        m_pACEntryData->loadDataSet(GetProjPath());
        AfxGetApp()->EndWaitCursor();
    }
    CDialog::OnCancel();
}

void CAircraftEntryProcessorDlg::ReloadPaxTypeList(HTREEITEM hSelItem)
{
    m_paxList.DeleteAllItems();
    int iProcID = (int)m_procTree.GetItemData(hSelItem);
    ProcessorID id = m_vProcs.at(iProcID);
    std::vector<AircraftEntryProcsEntry*> vEntry = m_pACEntryData->getEntryTimeDB()->FindEntryByProcID(id);
    HTREEITEM hParentItem = m_procTree.GetParentItem(hSelItem);

    bool isOwnData = true;
    while(vEntry.empty() && hParentItem!=NULL)
    {
        iProcID = (int)m_procTree.GetItemData(hParentItem);
        id = m_vProcs.at(iProcID);
        vEntry = m_pACEntryData->getEntryTimeDB()->FindEntryByProcID(id);
        isOwnData = false;
        hParentItem = m_procTree.GetParentItem(hParentItem);
    }

    std::sort(vEntry.begin(), vEntry.end(), AircraftEntryProcsEntry::sortEntry);
    size_t nCount = vEntry.size();
    for(size_t i=0; i<nCount; i++)
    {
        CString strItem;
        AircraftEntryProcsEntry* pEntry = vEntry[i];
        CMobileElemConstraint* pConst = (CMobileElemConstraint*)pEntry->getConstraint();
        pConst->screenPrint(strItem);
        m_paxList.InsertItem(i, strItem);
        if(isOwnData)
        {
            m_paxList.SetItemData(i, (DWORD)pEntry);
        }
        else
        {
            m_paxList.SetItemData(i, (DWORD)(-1));
        }
        ProbabilityDistribution* pProb = pEntry->getValue();
        m_paxList.SetItemText(i, 1, pProb->screenPrint());
        pEntry->GetBeginTime().printTime(strItem.GetBuffer(32));
        strItem.ReleaseBuffer();
        m_paxList.SetItemText(i, 2, strItem);
        pEntry->GetEndTime().printTime(strItem.GetBuffer(32));
        strItem.ReleaseBuffer();
        m_paxList.SetItemText(i, 3, strItem);
    }
}

LRESULT CAircraftEntryProcessorDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    if(message == WM_COLLUM_INDEX)
    {
        int iItem = int(wParam);
        int itemData = (int)m_paxList.GetItemData(iItem);
        if(itemData == -1)
        {
            MessageBox(_T("Use toolbar button \"add\" please."));
        }
    }
    else if(message == WM_INPLACE_COMBO_KILLFOUCUS)
    {
        int nCBSel = (int)wParam;
        LV_DISPINFO* pDispinfo = (LV_DISPINFO*)lParam;
        int iItem = pDispinfo->item.iItem;
        int iSubItem = pDispinfo->item.iSubItem;
        ASSERT(nCBSel>=0 && nCBSel<=(int)_g_GetActiveProbMan(m_pInTerm)->getCount());
        AircraftEntryProcsEntry* pEntry = (AircraftEntryProcsEntry*)m_paxList.GetItemData(iItem);
        ASSERT(pEntry != NULL);
        CProbDistEntry* pPBEntry = NULL;
        if(nCBSel == 0) // NEW PROBABILITY DISTRIBUTION
        {
            CDlgProbDist dlg(m_pInTerm->m_pAirportDB, true);
            if(dlg.DoModal()==IDOK)
            {
                int iSelPD = dlg.GetSelectedPDIdx();
                ASSERT(iSelPD!=-1);
                pPBEntry = _g_GetActiveProbMan( m_pInTerm )->getItem(iSelPD);
            }
            else
            {
                CString strItem = pEntry->getValue()->screenPrint();
                m_paxList.SetItemText(iItem, iSubItem, strItem);
                return CDialog::DefWindowProc(message, wParam, lParam);
            }
        }
        else
        {
            pPBEntry = _g_GetActiveProbMan(m_pInTerm)->getItem(nCBSel-1);
        }

        if(pEntry->getValue()->isEqual(pPBEntry->m_pProbDist))
        {
            CString strItem = pEntry->getValue()->screenPrint();
            m_paxList.SetItemText(iItem, iSubItem, strItem);
            return CDialog::DefWindowProc(message, wParam, lParam);
        }
        else// not equal, replace it.
        {
            delete pEntry->getValue();
            pEntry->setValue(ProbabilityDistribution::CopyProbDistribution(pPBEntry->m_pProbDist));
            CString strItem = pEntry->getValue()->screenPrint();
            m_paxList.SetItemText(iItem, iSubItem, strItem);
            GetDlgItem(IDC_BTN_SAVE)->EnableWindow(TRUE);
        }
    }
    else if(message == WM_INPLACE_DATETIME)
    {
        static LV_DISPINFO* pDispinfo = NULL;
        int iFlag = (int)wParam;
        if(iFlag == 1)
        {
            ASSERT(pDispinfo == NULL);
            pDispinfo = (LV_DISPINFO*)lParam;
        }
        else if(iFlag == 2)
        {
            ASSERT(pDispinfo != NULL);
            COleDateTime *pOleTime = (COleDateTime *)lParam;
            int iItem = pDispinfo->item.iItem;
            int iSubItem = pDispinfo->item.iSubItem;
            pDispinfo = NULL;

            ElapsedTime tempTime;
            tempTime.set(*pOleTime);

            AircraftEntryProcsEntry* pEntry = (AircraftEntryProcsEntry*)m_paxList.GetItemData(iItem);
            ASSERT(pEntry != NULL);
            if(iSubItem == 2) // edit begin time
            {
                if(tempTime > pEntry->GetEndTime())
                {
                    MessageBox("Begin Time can't be more than End Time", NULL,MB_OK|MB_ICONINFORMATION);
                    CString strItem = pEntry->GetBeginTime().printTime();
                    m_paxList.SetItemText(iItem, iSubItem, strItem);
                    pDispinfo = NULL;
                    return CDialog::DefWindowProc(message, wParam, lParam);;
                }
                else
                {
                    pEntry->SetBeginTime(tempTime);
                    ReloadPaxTypeList(m_procTree.GetSelectedItem());
                    GetDlgItem(IDC_BTN_SAVE)->EnableWindow(TRUE);
                }
            }
            else if(iSubItem == 3) // edit end time
            {
                if(tempTime < pEntry->GetBeginTime())
                {
                    MessageBox("End Time can't be less than Begin Time", NULL,MB_OK|MB_ICONINFORMATION);
                    CString strItem = pEntry->GetEndTime().printTime();
                    m_paxList.SetItemText(iItem, iSubItem, strItem);
                    pDispinfo = NULL;
                    return CDialog::DefWindowProc(message, wParam, lParam);
                }
                else
                {
                    pEntry->SetEndTime(tempTime);
                    ReloadPaxTypeList(m_procTree.GetSelectedItem());
                    GetDlgItem(IDC_BTN_SAVE)->EnableWindow(TRUE);
                }
            }
            else
            {
                ASSERT(0);
            }
        }
        else
        {
            ASSERT(0);
        }
    }
    return CDialog::DefWindowProc(message, wParam, lParam);
}
