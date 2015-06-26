#pragma once
#include "NumericEdit.h"
#include "FloatEdit.h"
#include "AutoFillComboBox.h"

class CTabSubView1 : public CDialog
{
    DECLARE_DYNAMIC(CTabSubView1)

public:
    CTabSubView1(CWnd* pParent = NULL);
    virtual ~CTabSubView1();
    enum { IDD = IDD_TABSUBVIEW1 };

protected:
    virtual BOOL OnInitDialog();
    virtual void DoDataExchange(CDataExchange* pDX);
    DECLARE_MESSAGE_MAP()
public:
    CEdit m_editRead;
    CEdit m_editWrite;
    CNumericEdit m_editFloat;
    CAutoFillComboBox m_cbTest;
    afx_msg void OnBnClickedButton1();
    afx_msg void OnBnClickedBtnRead();
    afx_msg void OnBnClickedBtnWrite();
    afx_msg void OnCbnEditchangeComboTest();
    afx_msg void OnDeltaposSpinFloat(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnSize(UINT nType, int cx, int cy);
private:
    int m_oldCx;
    int m_oldCy;
    typedef enum {TopLeft, TopRight, BottomLeft, BottomRight} LayoutRef;
    void LayoutControl(CWnd* pCtrl, LayoutRef refTopLeft, LayoutRef refBottomRight, int cx, int cy);
};
