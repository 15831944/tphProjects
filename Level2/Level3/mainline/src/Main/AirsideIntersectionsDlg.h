#pragma once
#include "AirsideObjectBaseDlg.h"
#include "../InputAirside/intersections.h"

// CAirsideIntersectionsDlg dialog

class CAirsideIntersectionsDlg : public CAirsideObjectBaseDlg
{
	DECLARE_DYNAMIC(CAirsideIntersectionsDlg)

public:
	CAirsideIntersectionsDlg(int nIntersectionsID ,int nAirportID,int nProjID,CWnd *pParent = NULL);
	//CAirsideIntersectionsDlg(Intersections * pIntersections,int nProjID,CWnd * pParent = NULL);
	~CAirsideIntersectionsDlg(void);

protected:
	HTREEITEM m_hLinkStretches;
	InsecObjectPartVector m_vrLinkStretches;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void LoadTree(void);
	virtual void OnContextMenu(CMenu& menuPopup, CMenu *& pMenu);

	virtual FallbackReason GetFallBackReason(void);
	virtual bool DoTempFallBackFinished(WPARAM wParam, LPARAM lParam);

	virtual void SetObjectPath(CPath2008& path);
	virtual bool UpdateOtherData(void);
public:
	virtual BOOL OnInitDialog(void);
	DECLARE_MESSAGE_MAP()
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnOK(void);
};
