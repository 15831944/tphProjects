#pragma once
#include "airsideobjectbasedlg.h"
#include "../AirsideGUI/UnitPiece.h"
#include "../InputAirside/ALTAirport.h"
#include "../Common/latlong.h"

class CUnitPiece;
class Structure;	
class CPath2008;

class CAirsideStructureDlg :
	public CAirsideObjectBaseDlg , public CUnitPiece
{
	DECLARE_DYNAMIC(CAirsideStructureDlg)
public:
	CAirsideStructureDlg(int nStructureID ,int nAirportID,int nProjID, CWnd* pParent = NULL );
	//CAirsideStructureDlg(Structure* pStructure,int nProjID,  CWnd* pParent = NULL);
	~CAirsideStructureDlg(void);

	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	HTREEITEM m_hServiceLocation;
	HTREEITEM m_hRotation;
//	CListCtrl m_wndShapeList;
	HTREEITEM m_hSurfaceTopPic;
	std::vector<HTREEITEM> m_vhSurfaceSidePic;

	virtual bool ConvertUnitFromDBtoGUI(void);
	virtual bool RefreshGUI(void);
	virtual bool ConvertUnitFromGUItoDB(void);
	void resetTree();
public:
	virtual BOOL OnInitDialog();
	void InitShapleList();
	void LoadTree();
	virtual void OnDoubleClickPropTree(HTREEITEM hTreeItem);
	virtual void OnContextMenu(CMenu& menuPopup, CMenu *& pMenu);

	virtual FallbackReason GetFallBackReason();
	virtual void SetObjectPath(CPath2008& path);
	virtual bool UpdateOtherData();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnRotationEdit();
	afx_msg void OnShowSides();
	afx_msg void OnRemovePic();

protected:
	virtual void OnOK();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	void ConvertLatLongToPos( const CLatitude& latitude , const CLongitude& longtitude,CPoint2008& pos );
	void ConvertPosToLatLong( const CPoint2008& pos, CLatitude& latitude, CLongitude& longitude );

private:
	ALTAirport m_AirportInfo;
	BOOL m_bShow;
};
