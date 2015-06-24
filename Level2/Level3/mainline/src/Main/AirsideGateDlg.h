#pragma once
#include "airsideobjectbasedlg.h"
#include "../AirsideGUI/UnitPiece.h"
#include "../InputAirside/ALTAirport.h"

class CUnitPiece;
class Stand;

class CAirsideGateDlg :
	public CAirsideObjectBaseDlg , public CUnitPiece
{
	DECLARE_DYNAMIC(CAirsideGateDlg)

public:
	CAirsideGateDlg(int nGateID,int nAirportID,int nProjID,CWnd *pParent = NULL);
	~CAirsideGateDlg(void);
	//CAirsideGateDlg(Stand* pStand,int nProjID, CWnd * pParent = NULL);
	ALTAirport m_airportInfo;
protected:
	HTREEITEM m_hServiceLocation;
	//HTREEITEM m_hInConstraint;
	//HTREEITEM m_hOutConstraint;

	HTREEITEM m_hLeadInLineItem;
	HTREEITEM m_hLeadOutLineItem;
	std::vector<HTREEITEM> m_vhLeadInLines;
	std::vector<HTREEITEM> m_vhLeadOutLines;
	std::vector<HTREEITEM> m_vLatLongItems;
	std::vector<HTREEITEM> m_vPositionItems;
	bool	m_bOnRelease;
	//bool m_bInConsModified;
	//bool m_bOutConsModified;


	void LoadTree();
	virtual void OnContextMenu(CMenu& menuPopup, CMenu *& pMenu);

	virtual bool ConvertUnitFromDBtoGUI(void);
	virtual bool RefreshGUI(void);
	virtual bool ConvertUnitFromGUItoDB(void);
	virtual void OnDoubleClickPropTree(HTREEITEM hTreeItem);

	virtual FallbackReason GetFallBackReason();
	virtual void SetObjectPath(CPath2008& path);
	virtual bool UpdateOtherData();
	void OnAddNewLeadInLine();
	void OnAddNewLeadOutLine();
	void OnDeleteLeadLine();
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnProcpropToggledBackup();
	afx_msg void OnAddReleasePoint();
	afx_msg void RemoveReleasePoint();
	afx_msg void OnEditVerticalProfile(void);
	afx_msg void OnEditLatLong();
	afx_msg void OnEditPosition();
	afx_msg void OnDeleteLatLog();
	afx_msg void OnDeletePosition();
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnOK();
	void ConvertPosToLatLong( const CPoint2008& pos, CLatitude& latitude, CLongitude& longitude );
	void ConvertLatLongToPos( const CLatitude& latitude , const CLongitude& longtitude,CPoint2008& pos );
	void LoadPathItem(CAirsideObjectTreeCtrl& treeprop,HTREEITEM rootItem, const CPath2008& path,AirsideObjectTreeCtrlItemDataEx& aoidDataEx,ItemStringSectionColor&isscStringColor, ARCUnit_Length lengthUnit );
};
