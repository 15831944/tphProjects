#pragma once
#include "CoolTree.h"
#include "CARCTipEdit.h"

class CCoolTreeEditSpin : public CDialog
{
public:
    void SetSpinRange(int nMin,int nMax);
    CWnd* m_pParent;
    CCoolTreeEditSpin(CWnd* pParent = NULL);
    CWnd* SetParent(CWnd* pWndNewParent);

    virtual BOOL PreTranslateMessage(MSG* pMsg);

    CSpinButtonCtrl m_spinValue;
    CEdit m_editValue;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    DECLARE_MESSAGE_MAP()

    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnKillfocusEdit1();

private:
    BOOL m_bESC;
};
