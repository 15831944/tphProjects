#pragma once
#include "airsideobjectbasedlg.h"
#include "../InputAirside/Taxiway.h"
#include "../AirsideGUI/UnitPiece.h"
#include "../InputAirside/ALTAirport.h"

class CUnitPiece;
class CAirsideTaxiwayDlg :
	public CAirsideObjectBaseDlg , public CUnitPiece
{
	DECLARE_DYNAMIC(CAirsideTaxiwayDlg)
public:
	CAirsideTaxiwayDlg(int nTaxiwayID ,int nAirportID,int nProjID,CWnd* pParent = NULL );
	//CAirsideTaxiwayDlg(Taxiway* pTaxiway,int nProjID, CWnd* pParent = NULL);
	~CAirsideTaxiwayDlg(void);

	virtual bool ConvertUnitFromDBtoGUI(void);
	virtual bool RefreshGUI(void);
	virtual bool ConvertUnitFromGUItoDB(void);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	HTREEITEM m_hServiceLocation;
	HTREEITEM m_hWidth;
	HTREEITEM m_hMarking;
	HTREEITEM m_hMarkPosition;
	std::vector<HTREEITEM> m_vLatLongItems;
	std::vector<HTREEITEM> m_vPositionItems;
	ALTAirport m_airportInfo;
	
public:
	virtual BOOL OnInitDialog();
	void LoadTree();
	virtual void OnDoubleClickPropTree(HTREEITEM hTreeItem);
	virtual void OnContextMenu(CMenu& menuPopup, CMenu *& pMenu);
	afx_msg void OnTaxiwayMarkModify();
	afx_msg void OnModifyWidth();
	afx_msg void OnModifyPosition();
	afx_msg void OnModifyHoldPosition();
	virtual FallbackReason GetFallBackReason();
	virtual void SetObjectPath(CPath2008& path);
	virtual bool UpdateOtherData();
	afx_msg void OnEditVerticalProfile(void);
	afx_msg void OnEditLatLong();
	afx_msg void OnEditPosition();
	afx_msg void OnDeleteLatLog();
	afx_msg void OnDeletePosition();
	DECLARE_MESSAGE_MAP()
protected:
	virtual void OnOK();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	void ConvertPosToLatLong( const CPoint2008& pos, CLatitude& latitude, CLongitude& longitude );
	void ConvertLatLongToPos( const CLatitude& latitude , const CLongitude& longtitude,CPoint2008& pos );

	void UpdateHoldPositions();
	//hold positons
	//Runway postions 
	//Taxiway::HoldPositionList m_vHoldList;
	std::vector<HTREEITEM> m_vhHoldItemlist;
	std::vector<HTREEITEM> m_vhIntersectionItemList;

	ALTObjectList m_vRunwayList;

	Taxiway * GetTaxiway()const;

};
