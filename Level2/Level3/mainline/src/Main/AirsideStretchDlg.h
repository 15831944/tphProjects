#pragma once
#include "AirsideObjectBaseDlg.h"
#include "../InputAirside/Stretch.h"
#include "../InputAirside/CircleStretch.h"
#include ".\AirsideLevelList.h"

class CUnitPiece;
class CAirsideStretchDlg :	public CAirsideObjectBaseDlg , public CUnitPiece
{
	DECLARE_DYNAMIC(CAirsideStretchDlg)
public:
	CAirsideStretchDlg(int nStretchID ,int nAirportID,int nProjID,CWnd *pParent = NULL);
	//CAirsideStretchDlg(Stretch * pStretch,int nProjID,CWnd * pParent = NULL);
	~CAirsideStretchDlg(void);
protected:
	CPath2008 m_path;
	DistanceUnit m_dLaneWidth;
	int m_iLaneNum;
	BOOL m_bSortDes;

    int m_nStretchID;
	int m_nAirportID;

	HTREEITEM m_hCtrlPoints;
	HTREEITEM m_hVerticalProfile;
	HTREEITEM m_hLaneNumber;
	HTREEITEM m_hLaneWidth;

	HTREEITEM m_hStretchSort;
	HTREEITEM m_hSortDirect;
	HTREEITEM m_hSortCircle;

	CAirsideLevelList m_vLevelList;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void LoadTree(void);
	virtual void OnDoubleClickPropTree(HTREEITEM hTreeItem);
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
	afx_msg void OnDefineNumber(void);
	afx_msg void OnDefineWidth(void);
	afx_msg void OnSelectStretchDirect(void);
	afx_msg void OnSelectStretchCircle(void);
};
