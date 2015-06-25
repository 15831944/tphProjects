#pragma once
#include "AirsideObjectBaseDlg.h"
#include "..\AirsideGUI\UnitPiece.h"
#include "..\InputAirside\ALTAirport.h"




class AirsideParkSpot;
class CAirsideParkingPosDlg : public CAirsideObjectBaseDlg, public CUnitPiece
{
	DECLARE_DYNAMIC(CAirsideParkingPosDlg)
	DECLARE_MESSAGE_MAP()
public:
	CAirsideParkingPosDlg(int nObjID,int nAirportID,int nProjID,CWnd* pParent = NULL);
	virtual FallbackReason GetFallBackReason();
	virtual void SetObjectPath(CPath2008& path);
	virtual bool UpdateOtherData();
	virtual void OnDoubleClickPropTree(HTREEITEM hTreeItem);


	void ConvertPosToLatLong( const CPoint2008& pos, CLatitude& latitude, CLongitude& longitude );
	void LoadTree();
	BOOL OnInitDialog();
	ALTAirport m_airportInfo;
	HTREEITEM m_hServiceLocation;
	HTREEITEM m_hLeadInLineItem;
	HTREEITEM m_hLeadOutLineItem;
	HTREEITEM m_hBackUpItem;
	std::vector<HTREEITEM> m_vPositionItems;
	std::vector<HTREEITEM> m_vLatLongItems;


	void LoadPathItem(CAirsideObjectTreeCtrl& treeprop,HTREEITEM rootItem, const CPath2008& path,AirsideObjectTreeCtrlItemDataEx& aoidDataEx,ItemStringSectionColor&isscStringColor, ARCUnit_Length lengthUnit );


	bool ConvertUnitFromDBtoGUI(void);

	bool RefreshGUI(void);

	bool ConvertUnitFromGUItoDB(void);

	void ConvertLatLongToPos( const CLatitude& latitude , const CLongitude& longtitude,CPoint2008& pos );

	void OnContextMenu(CMenu& menuPopup, CMenu *& pMenu);

	afx_msg void OnEditLatLong();
	afx_msg void OnProcpropToggledBackup();

	virtual CString GetTitile(BOOL bEdit)const=0;

	AirsideParkSpot* getParkSpot(){ return (AirsideParkSpot*)GetObject(); }
};

//////////////////////////////////////////////////////////////////////////
class CAirsidePaxBusParkingPosDlg : public CAirsideParkingPosDlg
{
	DECLARE_DYNAMIC(CAirsidePaxBusParkingPosDlg)
	
public:
	CAirsidePaxBusParkingPosDlg(int nObjID,int nAirportID,int nProjID,CWnd* pParent = NULL);
	virtual CString GetTitile(BOOL bEdit)const;
};



