#pragma once
#include "AirsideObjectBaseDlg.h"
#include "../InputAirside/YieldSign.h"
#include ".\AirsideLevelList.h"
#include "DlgSelectStretchIntersectionNode.h"
#include "../InputAirside/IntersectItem.h"
#include "../Common/Path2008.h"

class Stretch;
class CUnitPiece;
class CAirsideYieldSignDlg :	public CAirsideObjectBaseDlg , public CUnitPiece
{
	DECLARE_DYNAMIC(CAirsideYieldSignDlg)
public:
	CAirsideYieldSignDlg(int nYieldSignID ,int nAirportID,int nProjID,CWnd *pParent = NULL);
	//CAirsideYieldSignDlg(YieldSign * pYieldSign,int nProjID,CWnd * pParent = NULL);
	~CAirsideYieldSignDlg(void);
public:
	typedef std::vector<StretchIntersectItem> STRETCH;
	typedef std::vector<int> INT;
protected:
	CPath2008 m_path;
	HTREEITEM m_hCtrlPoints;
	HTREEITEM m_hVerticalProfile;
	HTREEITEM m_hIntersection;
	int m_nAirportID;
	int m_nIntersect;
	CString m_strName;

	int m_nStretchID;
	CString m_strStretchName;
    double m_degree;
	std::pair<INT,STRETCH> m_vStretchPath;
	StretchIntersectItem stretchItem;

	CAirsideLevelList m_vLevelList;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void LoadTree(void);
	void LoadTreeSubItemCtrlPoints(HTREEITEM preItem,const CPath2008& path );
	void LoadIntersectionNode(HTREEITEM preItem);
	void CalculateStrechID(void);
	void CalculateRotateDegree(void);
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
	afx_msg void OnSelectYieldSignIntersection(void);
};
