#pragma once
#include "AirsideObjectBaseDlg.h"
#include "../InputAirside/turnoff.h"

// CAirsideRoundaboutDlg

class CAirsideTurnoffDlg : public CAirsideObjectBaseDlg
{
	DECLARE_DYNAMIC(CAirsideTurnoffDlg)

public:
	CAirsideTurnoffDlg(int nTurnoffID ,int nAirportID,int nProjID,CWnd *pParent = NULL);
	//CAirsideTurnoffDlg(Turnoff * pTurnoff,int nProjID,CWnd * pParent = NULL);
	~CAirsideTurnoffDlg(void);

protected:
	HTREEITEM m_hLaneWidth;	
	HTREEITEM m_hLinkStretches;
	HTREEITEM m_hCtrlPoints;
	HTREEITEM m_hVerticalProfile;
	CPath2008 m_path;
	DistanceUnit m_dLaneWidth; 
	InsecObjectPartVector m_vrLinkStretches;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void LoadTreeSubItemCtrlPoints(HTREEITEM preItem,const CPath2008& path );
	void LoadTree(void);
	virtual void OnDoubleClickPropTree(HTREEITEM hTreeItem);
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
	afx_msg void OnDefineWidth(void);
};


