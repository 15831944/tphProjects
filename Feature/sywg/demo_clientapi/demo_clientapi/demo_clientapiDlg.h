#pragma once
#include "ApiRsp.h"
#include "afxcmn.h"
#include "afxwin.h"
#include <vector>
class Cdemo_clientapiDlg : public CDialogEx
{
public:
    Cdemo_clientapiDlg(CWnd* pParent = NULL);
    enum { IDD = IDD_DEMO_CLIENTAPI_DIALOG };
protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:
    HICON m_hIcon;
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    LRESULT OnShowMsg(WPARAM wpara,LPARAM lpara);
    DECLARE_MESSAGE_MAP()
public:
    CApiRsp *m_pRsp;
    CListCtrl m_ListCtrl;
    CProgressCtrl m_Progress;
    afx_msg void OnClose();
    afx_msg void OnBnClickedButton3();
    afx_msg void OnBnClickedButton6();
    afx_msg void OnBnClickedButtonLogin();
    afx_msg void OnBnClickedButtonQrymatch();
    afx_msg void OnBnClickedCancelSendFile();
    afx_msg void OnBnClickedButtonQryposi();
    afx_msg void OnBnClickedButtonQryfund();
    afx_msg void OnBnClickedSendFile();
    afx_msg void OnBnClickedRecvFile();
    CEdit m_EditUser;
    CEdit m_EditPassword;
    CString m_Username;
    CString m_Password;

public:
    void AddFieldValue(int nCol,char *strvalue);
    void InsertInfo(char *pInfo);
    void ClearList();
    void SetListHeader(vector<FIELDINFO> * pFieldInfo);
    void SetQryTitle(char *title);
    CListCtrl m_ListQry;
    CStatic m_QryName;
    CButton m_LoginButton;
    CBitmap bitmap;
    CBitmap bitmap_SendFile;
    CBitmap bitmap_QryPosi;
    CBitmap bitmap_QryMatch;
    CBitmap bitmap_QryFund;
    CBitmap bitmap_Cancel;
    CBitmap bitmap_Exit;
    CBitmap bitmap_List;
    CBitmap bitmap_ListQry;
    CBitmap bitmap_Settlement;

    CButton m_SendFile;
    CButton m_QryMatch;
    CButton m_QryPosi;
    CButton m_QryFund;
    CButton m_Settlement;
    afx_msg void OnBnClickedButtonCancel();
    CButton m_ButtonCancel;
    CButton m_Exit;
    CImageList m_ImageList;
    CImageList m_ImageList_Qry;
    afx_msg void OnBnClickedButtonSettlement();
    afx_msg void OnBnClickedOk();
    CEdit m_EditDate;
};
