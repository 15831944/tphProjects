#pragma once
#include "layoutobjectpropdlgimpl.h"
#include "TreeCtrlItemInDlg.h"
class LandsideTaxiQueue;
class LandsideTaxiQueuePropDlgImpl :
	public ILayoutObjectPropDlgImpl
{
public:
	LandsideTaxiQueuePropDlgImpl(LandsideFacilityLayoutObject* pObj,CARCLayoutObjectDlg* pDlg);
	~LandsideTaxiQueuePropDlgImpl(void);

	CString					getTitle();
	void					LoadTree();
	virtual int				getBitmapResourceID() { return IDB_BITMAP_LANDSIDE_BUSSTATION; }
	virtual FallbackReason	GetFallBackReason();	

	void					OnContextMenu(CMenu& menuPopup, CMenu *&pMenu);	
	void					OnDoubleClickPropTree(HTREEITEM hTreeItem);

	void					DoFallBackFinished( WPARAM wParam, LPARAM lPara);
	BOOL					OnDefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	bool					OnOK( CString& errorMsg );
	LandsideTaxiQueue*		getTaxiQueue();

protected:
	HTREEITEM m_hPath;
	HTREEITEM m_hStretch;
	HTREEITEM m_hWaitingPath;
	HTREEITEM m_hLaneFrom;
	HTREEITEM m_hLaneTo;
	HTREEITEM m_hAngle;
	HTREEITEM m_hDegree;

	void OnEditLaneFrom(HTREEITEM hItem);
	void OnEditLaneTo(HTREEITEM hItem);
	void OnEditDegree(HTREEITEM hItem);
	void OnNodeTypeCombo();
};
