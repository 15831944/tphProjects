#pragma once
#include "AirsideObjectBaseDlg.h"
#include "../InputAirside/TrafficLight.h"
#include ".\AirsideLevelList.h"
#include "DlgSelectStretchIntersectionNode.h"

class Stretch;
class StretchIntersectItem;
class CUnitPiece;
class CAirsideTrafficLightDlg :	public CAirsideObjectBaseDlg , public CUnitPiece
{
	DECLARE_DYNAMIC(CAirsideTrafficLightDlg)
public:
	CAirsideTrafficLightDlg(int nTrafficLightID ,int nAirportID,int nProjID,CWnd *pParent = NULL);
	//CAirsideTrafficLightDlg(TrafficLight * pTrafficLight,int nProjID,CWnd * pParent = NULL);
	~CAirsideTrafficLightDlg(void);
public:
	typedef std::vector<StretchIntersectItem> STRETCH;
	typedef std::vector<DistanceUnit> LENGTH;
protected:
	CPath2008 m_path;
	HTREEITEM m_hCtrlPoints;
	HTREEITEM m_hVerticalProfile;
    HTREEITEM m_hIntersection;
	int m_nAirportID;
	CString m_strName;
    int m_nInterNode;

	std::pair<STRETCH,LENGTH> trafficLight;
	std::vector<ARCVector3> vpoints;

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
	void OnSelectStretchIntersectNode(void);
	void SetTrafficLightPoints(void);
	void SetSubTrafficLightPoints(const DistanceUnit& dist1, const DistanceUnit& dist2 ,StretchIntersectItem& vStretch1, StretchIntersectItem& vStretch2);
public:
	virtual BOOL OnInitDialog(void);
	DECLARE_MESSAGE_MAP()
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnOK(void);
	afx_msg void OnEditVerticalProfile(void);
};
