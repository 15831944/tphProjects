#pragma once
#include "AirsideObjectBaseDlg.h"
#include "../InputAirside/laneadapter.h"

// CAirsideLaneAdapterDlg dialog

class CAirsideLaneAdapterDlg : public CAirsideObjectBaseDlg
{
	DECLARE_DYNAMIC(CAirsideLaneAdapterDlg)

public:
	CAirsideLaneAdapterDlg(int nLaneAdapterID ,int nAirportID,int nProjID,CWnd *pParent = NULL);
// 	CAirsideLaneAdapterDlg(LaneAdapter * pLaneAdapter,int nProjID,CWnd * pParent = NULL);
	~CAirsideLaneAdapterDlg(void);

protected:
	HTREEITEM m_hLinkStretches;
	InsecObjectPartVector m_vrLinkStretches;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
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
};
