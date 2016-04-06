#pragma once
#include "ApiRsp.h"
#include "afxcmn.h"
#include "afxwin.h"

class Cdemo_serverapiDlg : public CDialogEx
{
public:
    Cdemo_serverapiDlg(CWnd* pParent = NULL);
    enum { IDD = IDD_DEMO_SERVERAPI_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:
    HICON m_hIcon;
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnClose();
    afx_msg void OnBnClickedStop();
    afx_msg void OnBnClickedSendFile();
    void InsertInfo(char *pInfo);
public:
    CApiRsp *m_pRsp;
    CProgressCtrl m_Progress;
    CButton m_Button;
    CListCtrl m_ListCtrl;
};
