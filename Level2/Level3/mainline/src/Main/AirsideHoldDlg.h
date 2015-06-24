#pragma once
#include "airsideobjectbasedlg.h"
#include <map>
#include "../AirsideGUI/UnitPiece.h"

class CUnitPiece;
class AirHold;
class CAirsideHoldDlg :
	public CAirsideObjectBaseDlg , public CUnitPiece
{
	DECLARE_DYNAMIC(CAirsideHoldDlg)
public:
	CAirsideHoldDlg(int nHoldID ,int nAirportID,int nProjID,CWnd* pParent = NULL);
	//CAirsideHoldDlg(AirHold * phold ,int nProjID,CWnd * pParent = NULL);
	~CAirsideHoldDlg(void);

protected:

	HTREEITEM	m_hWayPoint;
	HTREEITEM	m_hInBound;
	HTREEITEM	m_hOutBoundLegnm;
	HTREEITEM	m_hOutBoundLegmin;
	HTREEITEM	m_hRight;
	HTREEITEM	m_hMaxAlt;
	HTREEITEM	m_hMinAlt;
	HTREEITEM	m_hOutbItem;

	virtual bool ConvertUnitFromDBtoGUI(void);
	virtual bool RefreshGUI(void);
	virtual bool ConvertUnitFromGUItoDB(void);

	std::vector<std::pair<CString,int> > m_vWaypoint;
public:
	virtual BOOL OnInitDialog();
	void LoadTree();
	virtual void OnDoubleClickPropTree(HTREEITEM hTreeItem);
	virtual void OnContextMenu(CMenu& menuPopup, CMenu *& pMenu);

	virtual FallbackReason GetFallBackReason();
	virtual void SetObjectPath(CPath2008& path);
	virtual bool UpdateOtherData(){return true;}
	DECLARE_MESSAGE_MAP()
	afx_msg void OnHoldargumentEdit();
	afx_msg void OnHoldProcPropCombox();
protected:

	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

};
