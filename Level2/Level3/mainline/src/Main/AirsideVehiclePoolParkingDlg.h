#pragma once
#include "AirsideObjectBaseDlg.h"
#include "../InputAirside/VehiclePoolParking.h"
#include ".\AirsideLevelList.h"

class CUnitPiece;
class CAirsideVehiclePoolParkingDlg :	public CAirsideObjectBaseDlg , public CUnitPiece
{
	DECLARE_DYNAMIC(CAirsideVehiclePoolParkingDlg)
public:
	CAirsideVehiclePoolParkingDlg(int nVehiclePoolParkingID ,int nAirportID,int nProjID,CWnd *pParent = NULL);
	//CAirsideVehiclePoolParkingDlg(VehiclePoolParking * pVehiclePoolParking,int nProjID,CWnd * pParent = NULL);
	~CAirsideVehiclePoolParkingDlg(void);
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
