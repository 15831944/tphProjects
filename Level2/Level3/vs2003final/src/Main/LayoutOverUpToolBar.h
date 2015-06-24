#pragma once
#include "afxcmn.h"

#include <Controls/XTResizeDialog.h>
#include <vector>

// CLayoutOverUpToolBar dialog
#define  ADD_TOOL_NODE(Name,Type) \
         message.m_name = Name ; \
         message.m_toolType = Type ; \
         m_toolMes.push_back(message) ;
class CTermPlanDoc;
class CAircraftFurnishingDataSet ;
class CLayoutOverUpToolBar : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CLayoutOverUpToolBar)

public:
	CLayoutOverUpToolBar(CTermPlanDoc* _TermPlan , CWnd* pParent = NULL);   // standard constructor
	virtual ~CLayoutOverUpToolBar();

// Dialog Data
	enum { IDD = IDD_DIALOG_LAYOUT_TOOL };
   class  ToolMessage
   {
   public:
        CString m_name ;
		int m_toolType ;
   };
   enum {TY_SEAL,TY_STORAGE,TY_DOOR,TY_GALLEY,TY_WASHROOM,TY_EMERGENCY};
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	CTreeCtrl m_tree;
	CTermPlanDoc* m_Doc ;
	CAircraftFurnishingDataSet* m_FurnishingDataSet ;
protected:
	BOOL OnInitDialog() ;
	void InitTree() ;
	afx_msg void OnSize(UINT nType, int cx, int cy) ;
	void AddTypeNode(const CString& _type , HTREEITEM  _item ) ;
protected:
	HTREEITEM m_hItemDragSrc;
    CImageList   m_image ;
	CImageList*   m_pDragImage;
	BOOL m_bDragging;
	std::vector<ToolMessage> m_toolMes ;
public:
	afx_msg void OnNMClickTreeLayout(NMHDR *pNMHDR, LRESULT *pResult);
	void OnMouseMove(UINT nFlags, CPoint point) ;
	void OnLButtonUp(UINT nFlags, CPoint point) ;
	afx_msg void OnTvnSelchangedTreeLayout(NMHDR *pNMHDR, LRESULT *pResult);
	void InitToolNode(HTREEITEM root) ;
	afx_msg void OnTvnBegindragTreeLayout(NMHDR *pNMHDR, LRESULT *pResult);
};
