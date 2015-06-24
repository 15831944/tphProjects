#pragma once
#include "airsideobjectbasedlg.h"
#include "../AirsideGUI/UnitPiece.h"

class CUnitPiece;
class DeicePad;
class CAirsideDeicePadDlg :
	public CAirsideObjectBaseDlg , public CUnitPiece
{
	DECLARE_DYNAMIC(CAirsideDeicePadDlg)
public:
	CAirsideDeicePadDlg(int nDeicePadID,int nAirportID,int nProjID,CWnd *pParent = NULL);
// 	CAirsideDeicePadDlg(DeicePad *pDeicePad ,int nProjID, CWnd * pParent = NULL);
	~CAirsideDeicePadDlg(void);

protected:
	HTREEITEM m_hServiceLocation;
	HTREEITEM m_hInConstraint;
	HTREEITEM m_hOutConstraint;
	HTREEITEM m_hWingSpan;
	HTREEITEM m_hLength;
	HTREEITEM m_hAsStand;

	bool m_bInConsModified;
	bool m_bOutConsModified;
	void LoadTree();
	virtual void OnDoubleClickPropTree(HTREEITEM hTreeItem);
	virtual void OnContextMenu(CMenu& menuPopup, CMenu *& pMenu);

	virtual bool ConvertUnitFromDBtoGUI(void);
	virtual bool RefreshGUI(void);
	virtual bool ConvertUnitFromGUItoDB(void);

	virtual FallbackReason GetFallBackReason();
	virtual void SetObjectPath(CPath2008& path);
	virtual bool UpdateOtherData();
	virtual void OnModifyAsStand();
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

	afx_msg void OnProcpropToggledBackup();
	afx_msg void OnModifyWingspan();
	afx_msg void OnModifyLength();


protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);


	virtual void OnOK();
};
