#pragma once
#include ".\landsideroundaboutpropdlgimpl.h"

//////////////////////////////////////////////////////////////////////////
class LandsideExternalNode;
class LandsideExternalNodePropDlgImpl : public ILayoutObjectPropDlgImpl
{
public:
	LandsideExternalNodePropDlgImpl(LandsideFacilityLayoutObject* pObj,CARCLayoutObjectDlg* pDlg );
	virtual CString getTitle();
	virtual void LoadTree();

	//virtual void OnDoubleClickPropTree(HTREEITEM hTreeItem);
	virtual int				getBitmapResourceID();

	LandsideExternalNode *GetNodeData(){ return (LandsideExternalNode*)m_pObject; }
	
	virtual FallbackReason GetFallBackReason();
	virtual void OnContextMenu(CMenu& menuPopup, CMenu *&pMenu);
	virtual void DoFallBackFinished( WPARAM wParam, LPARAM lParam );

	virtual bool OnOK(CString& errorMsg);
protected:
	//HTREEITEM m_hLinkStretch;
	//HTREEITEM m_hStertchPort;
	HTREEITEM m_hPos;

	std::vector<HTREEITEM> m_hLinkageItems;
	//BOOL OnDefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	//void OnNodeStretchCombo(HTREEITEM hTreeItem);
	//void OnPortCombo(HTREEITEM hTreeItem);

	//std::vector<LandsideLayoutObject*> mStretchList;
};
