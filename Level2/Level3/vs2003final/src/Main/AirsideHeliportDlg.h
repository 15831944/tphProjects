#pragma once
#include "AirsideObjectBaseDlg.h"
#include "../InputAirside/ALTAirport.h"
#include "../AirsideGUI/UnitPiece.h"

class CUnitPiece;
class Heliport;
class CAirsideHeliportDlg:
	public CAirsideObjectBaseDlg ,public CUnitPiece
{
	DECLARE_DYNAMIC(CAirsideHeliportDlg)
public:
	CAirsideHeliportDlg(int nHeliportID ,int nAirportID,int nProjID,CWnd *pParent = NULL);
	//CAirsideHeliportDlg(Heliport* pHeliport,int nProjID,CWnd * pParent = NULL);
	~CAirsideHeliportDlg(void);

	ALTAirport m_airportInfo;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	HTREEITEM m_hServiceLocation;
	HTREEITEM m_hRadius;
	std::vector<HTREEITEM> m_vLatLongItems;
	std::vector<HTREEITEM> m_vPositionItems;
	//HTREEITEM m_hMarking1;
	//HTREEITEM m_hMarking2;

	//HTREEITEM m_hMarking1LandingThreshold;
	//HTREEITEM m_hMarking1TakeOffThreshold;

	//HTREEITEM m_hMarking2LandingThreshold;
	//HTREEITEM m_hMarking2TakeOffThreshold;

	void LoadTree();
	virtual void OnDoubleClickPropTree(HTREEITEM hTreeItem);
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
//	afx_msg void OnRunwaymarkModify();
	afx_msg void OnModifyRadius();
	afx_msg void OnEditVerticalProfile(void);
	afx_msg void OnEditLatLong();
	afx_msg void OnEditPosition();
	afx_msg void OnDeleteLatLog();
	afx_msg void OnDeletePosition();
//	afx_msg void OnModifyThreshold();
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnOK();
	void autoCalRunwayMark(CPath2008& path);
	void ConvertPosToLatLong( const CPoint2008& pos, CLatitude& latitude, CLongitude& longitude );
	void ConvertLatLongToPos( const CLatitude& latitude , const CLongitude& longtitude,CPoint2008& pos );
};
