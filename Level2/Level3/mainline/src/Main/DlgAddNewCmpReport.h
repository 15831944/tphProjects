#pragma once
#include "compare\ComparativeProject.h"

class DlgAddNewCmpReport : public CDialog
{
public:
	DlgAddNewCmpReport(CWnd* pParent = NULL);

	enum { IDD = IDD_CMPREPORTADDNEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnOK();
    virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
public:
	CString m_strName;
	CString m_strDesc;
private:
    int m_oldCx;
    int m_oldCy;
    typedef enum {TopLeft, TopRight, BottomLeft, BottomRight} LayoutRef;
    void LayoutControl(CWnd* pCtrl, LayoutRef refTopLeft, LayoutRef refBottomRight, int cx, int cy);
public:
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
};

class DlgEditCmpReport : public DlgAddNewCmpReport
{
public:
    DlgEditCmpReport(CWnd* pParent = NULL);

    void SetCmpProj(const CComparativeProject* val);
protected:
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    const CComparativeProject* m_pCmpProj;
};

