#pragma once
#include "AirsideObjectBaseDlg.h"
#include ".\AirsideLevelList.h"
#include "../InputAirside/DriveRoad.h"
#include "../AirsideGUI/UnitPiece.h"

class CUnitPiece;

// AirsideDriveRoadDlg dialog
class AirsideDriveRoadDlg :	public CAirsideObjectBaseDlg , public CUnitPiece
{
	DECLARE_DYNAMIC(AirsideDriveRoadDlg)

public:
	AirsideDriveRoadDlg(int nRoadID,int nParentID, int nAirportID,int nProjID,CWnd *pParent = NULL);  
	virtual ~AirsideDriveRoadDlg();

protected:
	CPath2008 m_path;
	DistanceUnit m_dLaneWidth;
	int m_nLaneNum;
	RoadDirection m_enumDir;

	int m_nRoadID;
	int m_nAirportID;

	HTREEITEM m_hCtrlPoints;
	HTREEITEM m_hVerticalProfile;
	HTREEITEM m_hLaneNumber;
	HTREEITEM m_hLaneWidth;
	HTREEITEM m_hDirect;


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
	void ModifyDirection(void);
};
