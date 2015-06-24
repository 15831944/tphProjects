#pragma once
#include "airsideobjectbasedlg.h"
#include "../AirsideGUI/UnitPiece.h"
#include "../Common/latlong.h"
#include "../InputAirside/ALTAirport.h"

class CUnitPiece;
class AirSector;
class CAirsideSectorDlg :
	public CAirsideObjectBaseDlg , public CUnitPiece
{
	DECLARE_DYNAMIC(CAirsideSectorDlg)
public:
	CAirsideSectorDlg(int nSectorID ,int nAirportID,int nProjID,CWnd* pParent = NULL);
	//CAirsideSectorDlg(AirSector* pSector, int nProjID,CWnd* pParent = NULL);
	~CAirsideSectorDlg(void);

protected:
	HTREEITEM m_hServiceLocation;
	HTREEITEM m_hVertices;


	HTREEITEM m_hLowAlt;
	HTREEITEM m_hHighAlt;
	HTREEITEM m_hInterval;
	HTREEITEM m_hFixLL;
	std::vector<HTREEITEM> m_hFixLList;

	std::vector<CLatitude> m_fixlatitude;
	std::vector<CLongitude> m_fixlongitude;

	int m_iTransfer;

//	bool m_bPathModified;

public:
	virtual BOOL OnInitDialog();
	void LoadTree();
	virtual void OnDoubleClickPropTree(HTREEITEM hTreeItem);
	virtual void OnContextMenu(CMenu& menuPopup, CMenu *& pMenu);	

	virtual bool ConvertUnitFromDBtoGUI(void);
	virtual bool RefreshGUI(void);
	virtual bool ConvertUnitFromGUItoDB(void);

	void ConvertPosToLatLong(const CPoint2008& pos, 	CLatitude& latitude, CLongitude& longitude );
	void ConvertLatLongToPos(const CLatitude& latitude , const CLongitude& longtitude,CPoint2008& pos);

	virtual FallbackReason GetFallBackReason();
	virtual void SetObjectPath(CPath2008& path);
	virtual bool UpdateOtherData();
	DECLARE_MESSAGE_MAP()

	afx_msg void OnModifyLowAltitude();
	afx_msg void OnModifyHighAltitude();
	afx_msg void OnModifyBandInterval();
	afx_msg void OnPropModifyLatLong();

protected:
	ALTAirport m_AirportInfo;
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnOK();
};
