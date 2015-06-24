#pragma once

#include ".\MFCExControl\ARCBaseTree.h"
#include "MSVNodeData.h"


// CEnvironmentMSView
class CTreeCtrlItem;
namespace MSV
{
class CEnvironmentMSView : public CView
{

	enum enumDialog
	{
		enumDialog_0 = 0,
		enumDialog_AirPollutants,
		enumDialog_FuelsProperties,
		enumDialog_SourcesEmissions,
		
		//add new enumeration before this line, and id increment in default(+1)
		enumDialog_Max
	};

	class CEnviromentTreeCtrl
	{
	public:
		CEnviromentTreeCtrl(CARCBaseTree& treem,MSV::CNodeDataAllocator* pNodeDataAlloc );
		~CEnviromentTreeCtrl();
		void LoadImage(CImageList& imglist);
		void InitTree();

	protected:
		CTreeCtrlItem AddItem(const CString& strText,
								CTreeCtrlItem& hParent,								
								MSV::enumNodeType enumType = MSV::NodeType_Normal ,
								enumDialog enumDlg = enumDialog_0);

	protected:
		CARCBaseTree& m_treeCtrl;
		MSV::CNodeDataAllocator* m_pNodeDataAlloc;
		std::vector<MSV::CNodeData *> m_vNodeData;

	};

	DECLARE_DYNCREATE(CEnvironmentMSView)

protected:
	CEnvironmentMSView();
	virtual ~CEnvironmentMSView();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnInitialUpdate();
	
public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif



public:
	CARCBaseTree& GetTreeCtrl(){ return m_wndTreeCtrl;}
protected:
	CImageList				m_imgList;
	//CImageList				m_ilNodes;
	CARCBaseTree			m_wndTreeCtrl;
	HTREEITEM m_hRightClkItem;
	HTREEITEM m_selItem;

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnRClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnLButtonDblClk(WPARAM wParam, LPARAM lParam);


protected:
	//int, enumeration, enumDialog
	void ShowDialog( int nEnum );

	CNodeDataAllocator* mpNodeDataAllocator;
};



}
