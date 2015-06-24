#pragma once
#include "airsideobjectbasedlg.h"
#include "../AirsideGUI/UnitPiece.h"

class CUnitPiece;
class Contour;
class CAirsideContourDlg :
	public CAirsideObjectBaseDlg , public CUnitPiece
{
	DECLARE_DYNAMIC(CAirsideContourDlg)
public:
	CAirsideContourDlg(int nContourID,int nAirportID, int nParentID,int nProjID, CWnd* pParent = NULL );
	//CAirsideContourDlg(Contour * pContour,int nProjID, CWnd * pParent = NULL);
	~CAirsideContourDlg(void);

		
protected:
	HTREEITEM m_hServiceLocation;

	HTREEITEM m_hAltitude;

	DistanceUnit m_dAptAltitdude;
	virtual bool ConvertUnitFromDBtoGUI(void);
	virtual bool RefreshGUI(void);
	virtual bool ConvertUnitFromGUItoDB(void);

public:
	virtual BOOL OnInitDialog();
	void LoadTree();
	virtual void OnDoubleClickPropTree(HTREEITEM hTreeItem);
	virtual void OnContextMenu(CMenu& menuPopup, CMenu *& pMenu);

	virtual FallbackReason GetFallBackReason();
	virtual void SetObjectPath(CPath2008& path);
	virtual bool UpdateOtherData();
	DECLARE_MESSAGE_MAP()

	afx_msg void OnModifyAltitude();

protected:
	//virtual void OnOK();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	virtual void OnOK();
};
