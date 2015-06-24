#pragma once
#include "AirsideObjectBaseDlg.h"
#include "../InputAirside/ALTAirport.h"
#include "../AirsideGUI/UnitPiece.h"
#include "../InputAirside/ParkingPlace.h"

class AirsideParkingPlaceDlg:
	public CAirsideObjectBaseDlg ,public CUnitPiece
{
	DECLARE_DYNAMIC(AirsideParkingPlaceDlg)

public:
	AirsideParkingPlaceDlg(int nObjID,int nParentID, int nAirportID,int nProjID,CWnd *pParent = NULL);
	~AirsideParkingPlaceDlg(void);

protected:

	HTREEITEM m_hServiceLocation;
	HTREEITEM m_hSpotWidth;
	HTREEITEM m_hSpotLength;
	HTREEITEM m_hParkingType;
	HTREEITEM m_hSpotAngle;

	bool m_bPathModified;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void LoadTree();
	virtual void OnDoubleClickPropTree(HTREEITEM hTreeItem);
	void LoadTreeSubItemCtrlPoints(HTREEITEM preItem,const CPath2008& path );
	virtual void OnContextMenu(CMenu& menuPopup, CMenu *& pMenu);

	virtual FallbackReason GetFallBackReason();
	virtual void SetObjectPath(CPath2008& path);
	virtual bool UpdateOtherData();

	virtual bool ConvertUnitFromDBtoGUI(void);
	virtual bool RefreshGUI(void);
	virtual bool ConvertUnitFromGUItoDB(void);
	//bool m_bPathModified;
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnEditParameter();

	void ModifySpotWidth();
	void ModifySpotAngle();
	void ModifySpotLength();
	void ModifyParkingType();
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnOK();
};
