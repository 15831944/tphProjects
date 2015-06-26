#include "stdafx.h"
#include "TermPlan.h"
#include "Common\strdict.h"
#include "DlgAircraftEntrySpec.h"
#include "afxdialogex.h"
#include "Engine\PROCLIST.H"

#define SELECTED_COLOR RGB(53,151,53)

IMPLEMENT_DYNAMIC(CAircraftEntryProcessorDlg, CDialog)

    CAircraftEntryProcessorDlg::CAircraftEntryProcessorDlg(InputTerminal* _pInTerm, CWnd* pParent)
    : CDialog(CAircraftEntryProcessorDlg::IDD, pParent)
    , m_pInTerm(_pInTerm)
{
}

CAircraftEntryProcessorDlg::~CAircraftEntryProcessorDlg()
{
}

void CAircraftEntryProcessorDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TREE_ENTRYPROC, m_procTree);
    DDX_Control(pDX, IDC_LIST_PAXTYPE, m_listPaxType);
}


BEGIN_MESSAGE_MAP(CAircraftEntryProcessorDlg, CDialog)
    ON_WM_SIZE()
    ON_WM_GETMINMAXINFO()
    ON_WM_CREATE()
    ON_NOTIFY_EX(TTN_NEEDTEXTA, 0, OnToolTipText)
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

    m_vIncType.push_back(PointProc);
    m_vIncType.push_back(DepSourceProc);
    m_vIncType.push_back(DepSinkProc);
    m_vIncType.push_back(LineProc);
    m_vIncType.push_back(BaggageProc);
    m_vIncType.push_back(HoldAreaProc);
    m_vIncType.push_back(GateProc);
    m_vIncType.push_back(FloorChangeProc);
    m_vIncType.push_back(BarrierProc);
    m_vIncType.push_back(IntegratedStationProc);
    m_vIncType.push_back(MovingSideWalkProc);
    m_vIncType.push_back(Elevator);
    m_vIncType.push_back(ConveyorProc);
    m_vIncType.push_back(StairProc);
    m_vIncType.push_back(EscalatorProc);
    m_vIncType.push_back(BillboardProcessor);
    m_vIncType.push_back(BridgeConnectorProc);
    m_vIncType.push_back(RetailProc);
    InitACEntryProcTree();

    CRect rect;
    m_listPaxType.GetWindowRect(rect);
    ScreenToClient(rect);
    m_toolbarPaxType.SetWindowPos(NULL,rect.left,rect.top-26,rect.Width(),26,NULL);
    DisableAllToolBarButtons();

    DWORD dwStyle = m_listPaxType.GetExtendedStyle();
    dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
    m_listPaxType.SetExtendedStyle(dwStyle);
    for(int i=0; i<5; i++)
    {
        CString strTitle;
        strTitle.Format(_T("%d"), i+1);
        m_listPaxType.InsertColumn(i, strTitle, LVCFMT_CENTER, 100);
    }

    for(int i=0; i<6; i++)
    {
        CString strItem;
        strItem.Format(_T("%d-1"), i+1);
        m_listPaxType.InsertItem(i, strItem);
        strItem.Format(_T("%d-2"), i+1);
        m_listPaxType.SetItemText(i, 1, strItem);
        strItem.Format(_T("%d-3"), i+1);
        m_listPaxType.SetItemText(i, 2, strItem);
        strItem.Format(_T("%d-4"), i+1);
        m_listPaxType.SetItemText(i, 3, strItem);
        strItem.Format(_T("%d-5"), i+1);
        m_listPaxType.SetItemText(i, 4, strItem);
    }
    return TRUE;
}

void CAircraftEntryProcessorDlg::InitACEntryProcTree()
{
    AfxGetApp()->BeginWaitCursor();
    if(m_pInTerm == NULL)
        return;
    ProcessorList *procList = m_pInTerm->GetTerminalProcessorList();
    if (procList == NULL)
        return;

    ProcessorID id;
    id.SetStrDict(m_pInTerm->inStrDict);
    StringList strDict;
    for(size_t level1=0; level1<m_vIncType.size(); level1++)
        procList->getAllGroupNames(strDict, m_vIncType[level1]);

    COOLTREENODEINFO cni;
    CCoolTree::InitNodeInfo(this, cni);
    for(int level1=0; level1<strDict.getCount(); level1++)
    {
        CString strLevel1 = strDict.getString(level1);
        strLevel1.Trim();
        if (strLevel1.IsEmpty())
            continue;
        
        CString strProcID = strLevel1;
        id.setID(strProcID);
        m_vProcs.push_back(id);

        if(m_pPaxData->ProcIDHasData(id))
        {
            cni.clrItem = SELECTED_COLOR;
            cni.lfontItem.lfWeight = 700;
        }
        else
        {
            CCoolTree::InitNodeInfo(this, cni);
        }
        HTREEITEM hLevel1 = m_procTree.InsertItem(strProcID, cni, FALSE);
        m_procTree.SetItemData(hLevel1, (DWORD)(m_vProcs.size()-1));

        StringList strDictLevel2;
        for(int level2=0; level2<static_cast<int>(m_vIncType.size()); level2++)
            procList->getMemberNames(id, strDictLevel2, m_vIncType[level2]);

        for(int level2=0; level2<strDictLevel2.getCount(); level2++)
        {
            CString strLevel2 = strDictLevel2.getString (level2);
            strLevel2.Trim();
            if (strLevel2.IsEmpty())
                continue;

            strProcID = strLevel1 + "-" + strLevel2;
            id.setID((LPCSTR)strProcID);
            m_vProcs.push_back(id);

            if(m_pPaxData->ProcIDHasData(id))
            {
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

            for(int level3=0; level3<strDictLevel3.getCount(); level3++)
            {
                CString strLevel3 = strDictLevel3.getString(level3);
                strLevel3.Trim();
                if (strLevel3.IsEmpty())
                    continue;

                strProcID = strLevel1 + "-" + strLevel2 + "-" + strLevel3;
                id.setID((LPCSTR)strProcID);
                m_vProcs.push_back(id);

                if(m_pPaxData->ProcIDHasData(id))
                {
                    cni.clrItem = SELECTED_COLOR;
                    cni.lfontItem.lfWeight = 700;
                }
                else
                {
                    CCoolTree::InitNodeInfo(this, cni);
                }
                HTREEITEM hLevel3 = m_procTree.InsertItem(strLevel3, cni, FALSE, FALSE, hLevel2);
                m_procTree.SetItemData(hLevel3, (DWORD)(m_vProcs.size()-1));

                StringList strDictL4;
                for(int level4=0; level4<static_cast<int>(m_vIncType.size()); level4++)
                    procList->getMemberNames(id, strDictL4, m_vIncType[level4]);

                for (int level4=0; level4<strDictL4.getCount(); level4++)
                {
                    CString strLevel4 = strDictL4.getString(level4);
                    strLevel4.Trim();
                    if (strLevel4.IsEmpty())
                        continue;

                    strProcID = strLevel1 + "-" + strLevel2 + "-" + strLevel3 + "-" + strLevel4;
                    id.setID((LPCSTR)strProcID);
                    m_vProcs.push_back(id);

//                     //check AC Stand Gate Flag
//                     if(!CheckACStandFlag(strProcID))
//                         continue;
//                     if(!CheckPusherConveyor(strProcID))
//                         continue;

                    if(m_pPaxData->ProcIDHasData(id))
                    {
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
            }
        }
    }
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
    LayoutControl(&m_listPaxType, TopLeft,  BottomRight, cx, cy);
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
    ::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,SWP_NOACTIVATE| SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER); 
    return TRUE;
}

