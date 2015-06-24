#pragma once
#include "AirsideObjectBaseDlg.h"
#include "../InputAirside/StopSign.h"
#include ".\AirsideLevelList.h"
#include "DlgSelectStretchIntersectionNode.h"
#include "../InputAirside/IntersectItem.h"
#include "../Common/Path2008.h"

class Stretch;
class CUnitPiece;
class CAirsideStopSignDlg :	public CAirsideObjectBaseDlg , public CUnitPiece
{
	DECLARE_DYNAMIC(CAirsideStopSignDlg)
public:
	CAirsideStopSignDlg(int nStopSignID ,int nAirportID,int nProjID,CWnd *pParent = NULL);
	//CAirsideStopSignDlg(StopSign * pStopSign,int nProjID,CWnd * pParent = NULL);
	~CAirsideStopSignDlg(void);
public:
	typedef std::vector<StretchIntersectItem> STRETCH;
	typedef std::vector<int> INT;
protected:
	CPath2008 m_path;
	HTREEITEM m_hCtrlPoints;
	HTREEITEM m_hVerticalProfile;
	HTREEITEM m_hIntersection;
	int m_nAirportID;
	int m_InterNode;
	int m_StretchID;
	CString m_strName;
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
	afx_msg void OnSelectStopSignIntersection(void);
};
