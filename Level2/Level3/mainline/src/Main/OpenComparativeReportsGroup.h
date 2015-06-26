#pragma once
#include "printablelistctrl.h"
#include "../MFCExControl/ListCtrlEx.h"
#include "../MFCExControl/sortableheaderctrl.h"

class CComparativeProject;
class CDlgOpenComparativeReport : public CDialog
{
public:
    CDlgOpenComparativeReport(CWnd* pParent = NULL);
    enum { IDD = IDD_OPENCOMPARATIVEREPGROUPS };

    int GetSelIndex();
    void InitListCtrl();

    CString GetName() const { return m_strName; }
    void SetName(CString val) { m_strName = val; }
    CString GetDesc() const { return m_strDesc; }
    void SetDesc(CString val) { m_strDesc = val; }
public:
    CPrintableListCtrl m_ListReportsGroup;
    CSortableHeaderCtrl m_headerCtl;
    CToolBar m_toolbar;
protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual void OnOK();
    virtual void OnCancel();
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnDblclkProjlist(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnColumnclickListreportgroups(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnLvnItemchangedListreportgroups(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
    afx_msg void OnAddProject();
    afx_msg void OnEditProject();
    afx_msg void OnDelProject();
    DECLARE_MESSAGE_MAP()
private:
    void FillProjectList();
    void AddProjInfoToListEnd(CComparativeProject* pProject);

	bool IsCmpReportProjectOpen(const CString& strCmpProjectName);

protected:
    CString m_strName;
    CString m_strDesc;
private:
    int m_oldCx; // for resize
    int m_oldCy; // for resize
    typedef enum {TopLeft, TopRight, BottomLeft, BottomRight} LayoutRef;
    void LayoutControl(CWnd* pCtrl, LayoutRef refTopLeft, LayoutRef refBottomRight, int cx, int cy);
};
