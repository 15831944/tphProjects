#pragma once
#include "afxcmn.h"
#include "Inputs\IN_TERM.H"
#include "CommonData\PROCID.H"
#include "MFCExControl\CoolTree.h"
#include "Inputs\AircraftEntryProcessorData.h"
#include "MFCExControl\ListCtrlEx.h"

class CAircraftEntryProcessorDlg : public CDialog
{
    DECLARE_DYNAMIC(CAircraftEntryProcessorDlg)

public:
    CAircraftEntryProcessorDlg(InputTerminal* _pInputTerm, CWnd* pParent = NULL);
    virtual ~CAircraftEntryProcessorDlg();
    enum { IDD = IDD_DIALOG_AIRCRAFTENTRYPROC };
protected:
    InputTerminal* m_pInTerm;
    CToolBar m_toolbarPaxType;
    CCoolTree m_procTree;
    CListCtrlEx m_paxList;
private:
    std::vector<int> m_vIncType; // included processor types
    std::vector<ProcessorID> m_vProcs;
    AircraftEntryProcessorData* m_pBCPaxData;
protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnToolbarButtonAdd();
    afx_msg void OnToolbarButtonDel();
    virtual BOOL OnInitDialog();
    afx_msg void OnOK();
    afx_msg void OnSave();
    afx_msg void OnCancel();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
    afx_msg void OnDblClickListItem(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnSelChangedPaxList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg BOOL OnToolTipText(UINT id, NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnSelChangedTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    DECLARE_MESSAGE_MAP()
private:
    CString GetProjPath();
    void LoadProcTree();
    void ReloadPaxTypeList(HTREEITEM hSelItem);
    void DisableAllToolBarButtons();
    void InitPaxTypeListHeader();
private:
    int m_oldCx;
    int m_oldCy;
    int m_bDragging;
    typedef enum {TopLeft, TopRight, BottomLeft, BottomRight} LayoutRef;
    void LayoutControl(CWnd* pCtrl, LayoutRef refTopLeft, LayoutRef refBottomRight, int cx, int cy);
};

