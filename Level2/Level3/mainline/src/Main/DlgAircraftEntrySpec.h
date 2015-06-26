#pragma once
#include "afxcmn.h"
#include "MFCExControl\CoolTree.h"
#include "MFCExControl\ListCtrlEx.h"

class CDlgAircraftEntrySpec : public CDialog
{
    DECLARE_DYNAMIC(CDlgAircraftEntrySpec)

public:
    CDlgAircraftEntrySpec(CWnd* pParent = NULL);
    virtual ~CDlgAircraftEntrySpec();
    enum { IDD = IDD_DIALOG_AIRCRAFTENTRYSPEC };
protected:
    CCoolTree m_treeProcs;
    CListCtrlEx m_listPaxTYpe;
protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
    DECLARE_MESSAGE_MAP()
private:
    int m_oldCx;
    int m_oldCy;
    typedef enum {TopLeft, TopRight, BottomLeft, BottomRight} LayoutRef;
    void LayoutControl(CWnd* pCtrl, LayoutRef refTopLeft, LayoutRef refBottomRight, int cx, int cy);
};
