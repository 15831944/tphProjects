#pragma once
#include ".\landsideroundaboutpropdlgimpl.h"

//////////////////////////////////////////////////////////////////////////
class LandsideIntersectionNode;
class LandsideIntersectionPropDlgImpl : public ILayoutObjectPropDlgImpl
{
public:
	LandsideIntersectionPropDlgImpl(LandsideFacilityLayoutObject* pObj,CARCLayoutObjectDlg* pDlg );
	virtual CString getTitle();
	virtual void LoadTree();
	virtual int getBitmapResourceID();
	virtual FallbackReason GetFallBackReason();	

	virtual void OnContextMenu(CMenu& menuPopup, CMenu *&pMenu);	
	virtual void OnDoubleClickPropTree(HTREEITEM hTreeItem);
	//virtual void OnLButtonDownPropTree(HTREEITEM hTreeItem);
	virtual void OnSelChangedPropTree(HTREEITEM hTreeItem);
	virtual void OnPropDelete();
	virtual void DoFallBackFinished( WPARAM wParam, LPARAM lParam );
	LandsideIntersectionNode* getIntersection() { return (LandsideIntersectionNode*)m_pObject; }
protected:
	HTREEITEM m_hLaneLinkages;
	HTREEITEM m_hNodeType;	
	std::vector<HTREEITEM> m_hLinkageItems;
	BOOL OnDefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	void LoadTreeSubItemLaneLinkages( HTREEITEM preItem,LandsideIntersectionNode* path );

	void OnNodeTypeCombo();
	void OnEditGroupID(HTREEITEM& item);


	bool isInInitTree;

};