#pragma once
#include "AirsideObjectBaseDlg.h"
#include "../InputAirside/ReportingArea.h"
#include ".\AirsideLevelList.h"


class CUnitPiece;
class DlgReportingArea:	public CAirsideObjectBaseDlg , public CUnitPiece
{
	DECLARE_DYNAMIC(DlgReportingArea)
public:
	DlgReportingArea(int nAreaID ,int nAirportID,int nProjID,CWnd *pParent = NULL);
	~DlgReportingArea(void);

protected:
	CPath2008 m_path;
	HTREEITEM m_hCtrlPoints;
	HTREEITEM m_hVerticalProfile;

	CAirsideLevelList m_vLevelList;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void LoadTree(void);
	void LoadTreeSubItemCtrlPoints(HTREEITEM preItem,const CPath2008& path );
	virtual void OnContextMenu(CMenu& menuPopup, CMenu *& pMenu);

	virtual bool ConvertUnitFromDBtoGUI(void);
	virtual bool RefreshGUI(void);
	virtual bool ConvertUnitFromGUItoDB(void);

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
};
