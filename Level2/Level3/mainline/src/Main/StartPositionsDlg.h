#pragma once
#include "AirsideObjectBaseDlg.h"
#include "../InputAirside/ALTAirport.h"
#include "../AirsideGUI/UnitPiece.h"

class CUnitPiece;
class Runway;
class CStartPositionDlg:
	public CAirsideObjectBaseDlg ,public CUnitPiece
{
	DECLARE_DYNAMIC(CStartPositionDlg)
public:
	CStartPositionDlg(int nStartPositionID ,int nAirportID,int nProjID,CWnd *pParent = NULL);
	~CStartPositionDlg(void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnDoubleClickPropTree(HTREEITEM hTreeItem);
	virtual void OnContextMenu(CMenu& menuPopup, CMenu *& pMenu);

	virtual FallbackReason GetFallBackReason();
	virtual void SetObjectPath(CPath2008& path);
	virtual bool UpdateOtherData();

	virtual bool ConvertUnitFromDBtoGUI(void);
	virtual bool RefreshGUI(void);
	virtual bool ConvertUnitFromGUItoDB(void);

	virtual bool DoTempFallBackFinished(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnOK();
public:
	// to refresh the tree from data
	void LoadTree(void);

private:
	HTREEITEM m_hServiceLocationItem;
	HTREEITEM m_hServiceLocationData;
	HTREEITEM m_hRadiusItem;
	HTREEITEM m_hRadiusData;
	HTREEITEM m_hMarkingItem;
	HTREEITEM m_hMarkingData;
public:
	void OnMarkingModify(void);
	// Insert color definition of service point item (" x = *.*,  y = *.*,  z = *.* ")
	void InsertColoredSubItem();
	void OnRadiusModify(void);
	afx_msg void OnProcpropDelete(void);
};
