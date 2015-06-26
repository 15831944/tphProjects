#pragma once
#include "afxcmn.h"
#include "Inputs\IN_TERM.H"
#include "CommonData\PROCID.H"
#include "MFCExControl\CoolTree.h"
#include "Inputs\BridgeConnectorPaxData.h"

class CAircraftEntryProcessorDlg : public CDialog
{
    DECLARE_DYNAMIC(CAircraftEntryProcessorDlg)

public:
    CAircraftEntryProcessorDlg(InputTerminal* _pInputTerm, CWnd* pParent = NULL);
    virtual ~CAircraftEntryProcessorDlg();
    enum { IDD = IDD_DIALOG_AIRCRAFTENTRYPROC };
protected:
    BridgeConnectorPaxData* m_pPaxData;
    InputTerminal* m_pInTerm;
    CToolBar m_toolbarPaxType;
    CCoolTree m_procTree;
    CListCtrl m_listPaxType;
private:
    std::vector<int> m_vIncType; // included processor types
    std::vector<ProcessorID> m_vProcs;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
    afx_msg BOOL OnToolTipText(UINT id, NMHDR *pNMHDR, LRESULT *pResult);
    DECLARE_MESSAGE_MAP()
private:
    void InitACEntryProcTree();
    void DisableAllToolBarButtons();
private:
    int m_oldCx;
    int m_oldCy;
    typedef enum {TopLeft, TopRight, BottomLeft, BottomRight} LayoutRef;
    void LayoutControl(CWnd* pCtrl, LayoutRef refTopLeft, LayoutRef refBottomRight, int cx, int cy);
};
