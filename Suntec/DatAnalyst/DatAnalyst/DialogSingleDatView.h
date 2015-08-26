#pragma once
#include "afxwin.h"
#include "atlimage.h"
#include "DatParser.h"

class CDialogSingleDatView : public CDialog
{
    DECLARE_DYNAMIC(CDialogSingleDatView)
public:
    CDialogSingleDatView(CWnd* pParent = NULL);
    virtual ~CDialogSingleDatView();
    enum { IDD = IDD_SINGLEDATVIEW };

public:
    CImage m_image;
    CButton m_btnPrev;
    CButton m_btnNext;
    short m_curShowPic;
    DatParser m_datParser;
protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    DECLARE_MESSAGE_MAP()
    afx_msg void OnPaint();
    afx_msg void OnBnClickedBtnPrev();
    afx_msg void OnBnClickedBtnNext();
    afx_msg void OnDropFiles(HDROP hDropInfo);
    afx_msg void OnBnClickedButton1();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnBnClickedBtnGetpath();

private:
    void IncreaseCurShowPic(short iIdx);

private:
    /* the resize adjustment manager. */
    int m_oldCx;
    int m_oldCy;
    typedef enum {TopLeft, TopRight, BottomLeft, BottomRight} LayoutRef;
    void LayoutControl(CWnd* pCtrl, LayoutRef refTopLeft, LayoutRef refBottomRight, int cx, int cy);
};
