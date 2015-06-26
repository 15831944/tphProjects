#include "stdafx.h"
#include "TermPlan.h"
#include "Common\strdict.h"
#include "DlgAircraftEntrySpec.h"
#include "afxdialogex.h"
#include "Engine\PROCLIST.H"
#include "TermPlanDoc.h"
#include "Common\ProbabilityDistribution.h"
#include "DestributionParameterSpecificationDlg.h"
#include "PassengerTypeDialog.h"

const static COLORREF SELECTED_COLOR = RGB(53, 151, 53);
static UniformDistribution u2_10(2.0f, 10.0f);

IMPLEMENT_DYNAMIC(CAircraftEntryProcessorDlg, CDialog)

    CAircraftEntryProcessorDlg::CAircraftEntryProcessorDlg(InputTerminal* _pInTerm, CWnd* pParent)
    : CDialog(CAircraftEntryProcessorDlg::IDD, pParent)
    , m_pInTerm(_pInTerm)
    , m_pBCPaxData(NULL)
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
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_PAXTYPE, OnDblClickListItem)
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

    m_pBCPaxData = m_pInTerm->m_pACEntryData;
    m_vIncType.push_back(BridgeConnectorProc);
    LoadProcTree();

    CRect rect;
    m_paxList.GetWindowRect(rect);
    ScreenToClient(rect);
    m_toolbarPaxType.SetWindowPos(NULL,rect.left,rect.top-26,rect.Width(),26,NULL);
    DisableAllToolBarButtons();

    DWORD dwStyle = m_paxList.GetExtendedStyle();
    dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
    m_paxList.SetExtendedStyle(dwStyle);
    m_paxList.InsertColumn(0, _T("Passenger Type"), LVCFMT_LEFT, 180);
    m_paxList.InsertColumn(1, _T("Probability Distribution(SECONDS)"), LVCFMT_LEFT, 320);

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
    ACEntryTimeDistDatabase* pEntryTimeDB = m_pBCPaxData->getEntryTimeDB();
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

void CAircraftEntryProcessorDlg::OnDblClickListItem(NMHDR *pNMHDR, LRESULT *pResult)
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
        AircraftEntryProcsEntry* pEntry = (AircraftEntryProcsEntry*)m_paxList.GetItemData(pNMItemActivate->iItem);
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
    *pResult = 0;
}

void CAircraftEntryProcessorDlg::OnSelChangedPaxList(NMHDR *pNMHDR, LRESULT *pResult)
{
    DisableAllToolBarButtons();
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    m_toolbarPaxType.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD, TRUE);

    if(m_paxList.GetSelectedCount() != 0)
        m_toolbarPaxType.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL, TRUE);

    *pResult = 0;
}

void CAircraftEntryProcessorDlg::OnToolbarButtonAdd()
{
    while(true)
    {
        CPassengerTypeDialog dlg(m_pParentWnd);
        if(dlg.DoModal() == IDOK)
        {
            HTREEITEM hItem = m_procTree.GetSelectedItem();
            int iItem = (int)m_procTree.GetItemData(hItem);
            ProcessorID id = m_vProcs.at(iItem);
            std::vector<AircraftEntryProcsEntry*> vEntry = m_pBCPaxData->getEntryTimeDB()->FindEntryByProcID(id);
            size_t nCount = vEntry.size();
            bool bFound = false;
            for(size_t i=0; i<nCount; i++)
            {
                CMobileElemConstraint* pConst = (CMobileElemConstraint*)vEntry[i]->getConstraint();
                if(*pConst == dlg.GetMobileSelection())
                {
                    bFound = true;
                    break;
                }
            }
            if(bFound)
            {
                CString strMsg, strTemp;
                dlg.GetMobileSelection().screenPrint(strTemp);
                strMsg.Format(_T("Pax type \"%s\" already exists."), strTemp);
                MessageBox(strMsg);
                continue;
            }
            else
            {
                CMobileElemConstraint* pNewConst = new CMobileElemConstraint(dlg.GetMobileSelection());
                pNewConst->SetInputTerminal(m_pInTerm);
                ProbabilityDistribution* pNewProb = ProbabilityDistribution::CopyProbDistribution(&u2_10);


                AircraftEntryProcsEntry* pNewEntry = new AircraftEntryProcsEntry();
                pNewEntry->initialize(pNewConst, pNewProb, id);
                m_pBCPaxData->getEntryTimeDB()->addEntry(pNewEntry);

                COOLTREENODEINFO* pCni = m_procTree.GetItemNodeInfo(hItem);
                pCni->clrItem = SELECTED_COLOR;
                pCni->lfontItem.lfWeight = 700;
                CRect recItem;
                m_procTree.GetItemRect(hItem, recItem, FALSE);
                m_procTree.InvalidateRect(&recItem);
                ReloadPaxTypeList(hItem);
                GetDlgItem(IDC_BTN_SAVE)->EnableWindow(TRUE);
                break;
            }
        }
        else
        {
            break;
        }
    }
}

void CAircraftEntryProcessorDlg::OnToolbarButtonDel()
{
    POSITION pos = m_paxList.GetFirstSelectedItemPosition();
    ASSERT(pos > 0);
    AircraftEntryProcsEntry* pEntry = (AircraftEntryProcsEntry*)m_paxList.GetItemData((int)(pos-1));
    CMobileElemConstraint* pConst = (CMobileElemConstraint*)pEntry->getConstraint();
    CString strMsg, strTem;
    pConst->screenPrint(strTem);
    strMsg.Format(_T("Delete pax type \"%s\"?"), strTem);
    if(MessageBox(strMsg, NULL, MB_YESNO) == IDYES)
    {
        m_pBCPaxData->getEntryTimeDB()->DeleteEntry(pEntry);
        m_paxList.DeleteItem((int)(pos-1));

        HTREEITEM hSelItem = m_procTree.GetSelectedItem();
        ASSERT(hSelItem != NULL);
        int iProcID = (int)m_procTree.GetItemData(hSelItem);
        ProcessorID id = m_vProcs.at(iProcID);
        if(m_pBCPaxData->getEntryTimeDB()->getEntryCountByProcID(id) == 0)
        {
            ProcessorID idDefault;
            idDefault.init();
            if(id == idDefault)
            {
                AircraftEntryProcsEntry* pEntry = new AircraftEntryProcsEntry();
                pEntry->useDefaultValue(m_pInTerm);
                m_pBCPaxData->getEntryTimeDB()->addEntry(pEntry);
                ReloadPaxTypeList(hSelItem);
            }
            else
            {
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
}

void CAircraftEntryProcessorDlg::OnSave()
{
    m_pBCPaxData->saveDataSet(GetProjPath(), false);
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
        HCURSOR hCur = LoadCursor(NULL , IDC_SIZEWE);
        ::SetCursor(hCur);
        recList.left = point.x+(recList.left-recTree.right)-3;
        recTree.right = point.x-3;
        if(recTree.Width()>180 && recList.Width()>180)
        {
            m_procTree.MoveWindow(recTree);
            m_paxList.MoveWindow(recList);
            m_toolbarPaxType.MoveWindow(recList.left,recList.top-26,recList.Width(),26);
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
        m_pBCPaxData->loadDataSet(GetProjPath());
        AfxGetApp()->EndWaitCursor();
    }
    CDialog::OnCancel();
}

void CAircraftEntryProcessorDlg::ReloadPaxTypeList(HTREEITEM hSelItem)
{
    m_paxList.DeleteAllItems();
    int iProcID = (int)m_procTree.GetItemData(hSelItem);
    ProcessorID id = m_vProcs.at(iProcID);
    std::vector<AircraftEntryProcsEntry*> vEntry = m_pBCPaxData->getEntryTimeDB()->FindEntryByProcID(id);
    std::sort(vEntry.begin(), vEntry.end(), AircraftEntryProcsEntry::sortByPaxTypeString);
    size_t nCount = vEntry.size();
    for(size_t i=0; i<nCount; i++)
    {
        CString strItem;
        CMobileElemConstraint* pConst = (CMobileElemConstraint*)vEntry[i]->getConstraint();
        pConst->screenPrint(strItem);
        m_paxList.InsertItem(i, strItem);
        m_paxList.SetItemData(i, (DWORD)vEntry[i]);
        ProbabilityDistribution* pProb = vEntry[i]->getValue();
        m_paxList.SetItemText(i, 1, pProb->screenPrint());
    }
}
