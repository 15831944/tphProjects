#pragma once

#include "airsideobjectbasedlg.h"
#include "../InputAirside/ALTAirport.h"
#include "../Common/latlong.h"
#include "../AirsideGUI/UnitPiece.h"

class CUnitPiece;
class AirWayPoint;
class CAirsideWaypointDlg :
	public CAirsideObjectBaseDlg , public CUnitPiece
{
	DECLARE_DYNAMIC(CAirsideWaypointDlg)
public:
	CAirsideWaypointDlg(int nWayPointID,int nAirportID,int nProjID,CWnd *pParent = NULL);
	//CAirsideWaypointDlg(AirWayPoint * pWayPoint,int nProjID, CWnd * pParent = NULL);
	~CAirsideWaypointDlg(void);


protected:
	HTREEITEM m_hServiceLocation;
	HTREEITEM m_hxyz;
	HTREEITEM m_hFixLL;
	HTREEITEM m_hWayPointBearingDistance;
	HTREEITEM m_hOtherWayPoint;
	HTREEITEM m_hBearing;
	HTREEITEM m_hDistance;
	HTREEITEM m_hWayPointBearingAndWayPointBearing;
	HTREEITEM m_hOtherWayPoint1;
	HTREEITEM m_hBearing1;
	HTREEITEM m_hOtherWayPoint2;
	HTREEITEM m_hBearing2;
	HTREEITEM m_hHighLimit;
	HTREEITEM m_hLowLimit;

	CLatitude m_fixlatitude;
	CLongitude m_fixlongitude;
	std::vector<std::pair<CString,int> > m_vWaypoint;
	std::vector<std::pair<CString,int> > m_vOtherWaypoint;
	std::vector<std::pair<CString,int> > m_vOtherWaypoint1;
	std::vector<std::pair<CString,int> > m_vOtherWaypoint2;
	std::vector<AirWayPoint> m_vWaypointlist;

	void hItemClear();
	void LoadTree();
	virtual void OnDoubleClickPropTree(HTREEITEM hTreeItem);
	virtual void OnContextMenu(CMenu& menuPopup, CMenu *& pMenu);

	void ConvertPosToLatLong(const CPoint2008& pos, 	CLatitude& latitude, CLongitude& longitude );
	void ConvertLatLongToPos(const CLatitude& latitude , const CLongitude& longtitude,CPoint2008& pos);

	virtual bool ConvertUnitFromDBtoGUI(void);
	virtual bool RefreshGUI(void);
	virtual bool ConvertUnitFromGUItoDB(void);

	virtual FallbackReason GetFallBackReason();
	virtual void SetObjectPath(CPath2008& path);
	virtual bool UpdateOtherData();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPropModifyLowLimitValue();
	afx_msg void OnPropModifyHighLimitValue();
	afx_msg void OnPropModifyLatLong();
	afx_msg void OnWayPointPositon();
	afx_msg void OnWayPointBearingDistance();
	afx_msg void OnWayPointBearing();
	afx_msg void OnWayPointDistance();
	afx_msg void OnOtherWayPoint();
	afx_msg void OnWayPointBearingWayPointBearing();
	afx_msg void OnOtherWayPoint1();
	afx_msg void OnWayPointBearing1();
	afx_msg void OnOtherWayPoint2();
	afx_msg void OnWayPointBearing2();
	bool IsInWaypointDependencies(int WaypointID,int OtherWaypointID);
public:
	virtual BOOL OnInitDialog();
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);



	ALTAirport m_AirportInfo;

	virtual void OnOK();
};
