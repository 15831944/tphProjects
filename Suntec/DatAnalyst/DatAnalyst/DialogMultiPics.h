#pragma once
#include <gdiplus.h>
#include <vector>

class CDialogMultiPics : public CDialog
{
    DECLARE_DYNAMIC(CDialogMultiPics)

public:
    CDialogMultiPics(CWnd* pParent = NULL);
    virtual ~CDialogMultiPics();
    enum { IDD = IDD_MULTIPICTURES };
public:
    afx_msg void OnPaint();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnDropFiles(HDROP hDropInfo);
    afx_msg void OnBnClickedBtnReset();
    afx_msg void OnBnClickedBtnFitsizeMulpic();

protected:
    virtual BOOL OnInitDialog();
    virtual void DoDataExchange(CDataExchange* pDX);
    DECLARE_MESSAGE_MAP()

private:
    void ShowAllPictures();
    void ResizeWindowToFitImage();
    CString GetPictureNameListString();

private:
    std::vector<CString> m_vecCurFilePaths;
    short m_picLoadedIdx;
    Gdiplus::Bitmap* m_pGdiplusBitmap;
private:
    /* the resize adjustment manager. */
    int m_oldCx;
    int m_oldCy;
    typedef enum {TopLeft, TopRight, BottomLeft, BottomRight} LayoutRef;
    void LayoutControl(CWnd* pCtrl, LayoutRef refTopLeft, LayoutRef refBottomRight, int cx, int cy);

};

