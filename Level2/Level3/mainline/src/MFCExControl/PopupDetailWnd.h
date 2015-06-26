#pragma once


// CPopupDetailWnd
#define WM_TRACKWND_DESTROY	WM_USER+1001
class MFCEXCONTROL_API CPopupDetailWnd : public CWnd
{
public:
	DECLARE_DYNAMIC(CPopupDetailWnd)

public:
	CPopupDetailWnd(CWnd* pParent,int nItem,int nSubItem);
	virtual ~CPopupDetailWnd();

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();


public:
	BOOL m_bClickSave;
	int m_nSubItem;
	int m_nItem;

	CButton m_btnOK;
	CButton m_btnSave;

	CStatic m_staticPaxID;
	CString			m_strTitle;
	CListCtrl m_listTrack;

protected:
	//{{AFX_MSG(CPopupDetailWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnButtonSave();
	afx_msg void OnOK();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnClose();
	//}}AFX_MSG

private:

	//bool Init( Terminal* _pTerm, const CString& _strPath  );
	void InitCtrl( void );
	void LoadDataToList( void );
	void SaveTrackToFile( const CString& _strName );
};


