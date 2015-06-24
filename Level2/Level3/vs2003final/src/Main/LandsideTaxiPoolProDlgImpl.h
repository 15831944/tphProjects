#pragma once
#include "layoutobjectpropdlgimpl.h"
#include "TreeCtrlItemInDlg.h"
class LandsideTaxiPool;

class LandsideTaxiPoolProDlgImpl :
	public ILayoutObjectPropDlgImpl
{
public:
	LandsideTaxiPoolProDlgImpl(LandsideFacilityLayoutObject* pObj,CARCLayoutObjectDlg* pDlg);
	~LandsideTaxiPoolProDlgImpl(void);

	CString					getTitle();
	void					LoadTree();
	virtual int				getBitmapResourceID() { return IDB_BITMAP_LANDSIDE_PARKINGLOT; }
	virtual FallbackReason	GetFallBackReason();	

	void					OnContextMenu(CMenu& menuPopup, CMenu *&pMenu);	
	void					OnDoubleClickPropTree(HTREEITEM hTreeItem);

	void					DoFallBackFinished( WPARAM wParam, LPARAM lPara);
	BOOL					OnDefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	bool					OnOK( CString& errorMsg );
	LandsideTaxiPool*		getTaxiPool();

private:
	void OnEditWidth(HTREEITEM hItem);

private:
	HTREEITEM	m_hQueue;
	HTREEITEM	m_hArea;
	HTREEITEM	m_hWidth;
	HTREEITEM	m_hEnterStretch;
	HTREEITEM	m_hExitStretch;
};
