#pragma once
#include "AirsideObjectBaseDlg.h"
#include "../InputAirside/lineparking.h"

// CAirsideLineParkingDlg

class CAirsideLineParkingDlg : public CAirsideObjectBaseDlg
{
	DECLARE_DYNAMIC(CAirsideLineParkingDlg)

public:
	CAirsideLineParkingDlg(int nLineParkingID ,int nAirportID,int nProjID,CWnd *pParent = NULL);
// 	CAirsideLineParkingDlg(LineParking * pLineParking,int nProjID,CWnd * pParent = NULL);
	~CAirsideLineParkingDlg(void);

protected:
	HTREEITEM m_hSpotNumber;
	HTREEITEM m_hSpotWidth;	
	HTREEITEM m_hLinkStretches;
	HTREEITEM m_hCtrlPoints;
	HTREEITEM m_hVerticalProfile;
	CPath2008 m_path;
	DistanceUnit m_dSpotWidth; 
	int m_iSpotNum;
	InsecObjectPartVector m_vrLinkStretches;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void LoadTreeSubItemCtrlPoints(HTREEITEM preItem,const CPath2008& path );
	void LoadTree(void);
	virtual void OnContextMenu(CMenu& menuPopup, CMenu *& pMenu);

	virtual FallbackReason GetFallBackReason(void);
	virtual void SetObjectPath(CPath2008& path);
	virtual bool UpdateOtherData(void);
public:
	virtual BOOL OnInitDialog(void);
	DECLARE_MESSAGE_MAP()
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnOK(void);
	afx_msg void OnEditVerticalProfile(void); 
	afx_msg void OnDefineNumber(void);
	afx_msg void OnDefineWidth(void);
};


