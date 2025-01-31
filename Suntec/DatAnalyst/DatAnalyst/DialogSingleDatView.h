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
    Gdiplus::Bitmap* m_pGdiplusBitmap;
    CButton m_btnPrev;
    CButton m_btnNext;
    short m_curShowPic; // shown picture's index in dat file.
    CString m_curShowFilePath; // showing file's full path.
    DatParser m_datParser;
protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    DECLARE_MESSAGE_MAP()
    afx_msg void OnPaint();
    afx_msg void OnBnClickedBtnPrev();
    afx_msg void OnBnClickedBtnNext();
    afx_msg void OnDropFiles(HDROP hDropInfo);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnBnClickedBtnGetpath();
    afx_msg void OnBnClickedBtnPrevdat();
    afx_msg void OnBnClickedBtnNextdat();
    afx_msg void OnBnClickedBtnResetsize();
private:
    virtual BOOL OnInitDialog();
    void SetCurShowPic(short iIdx);
    void LoadBmpFromDatDataByIndex(short iIdx);
    void ResizeWindowToFitImage(); // reset main window size to fit the image.
    CString GetPictureInfo(); // get the jpg/png information from m_datParser.
    void UpdateInfoOfCurrentShowingDat();
    void ShowFile(const CString& curShowFilePath);
private:
    /* the resize adjustment manager. */
    int m_oldCx;
    int m_oldCy;
    typedef enum {TopLeft, TopRight, BottomLeft, BottomRight} LayoutRef;
    void LayoutControl(CWnd* pCtrl, LayoutRef refTopLeft, LayoutRef refBottomRight, int cx, int cy);

};
