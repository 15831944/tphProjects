#pragma once
#include "../MFCExControl/CoolTree.h"

// CDlgAircraftConfigurationSpecView dialog
class CAircaftLayOut ;
class InputTerminal ;
class CDeck ;
class CTermPlanDoc;
class CDlgAircraftConfigurationSpecView : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgAircraftConfigurationSpecView)
protected:
	class CNodeData
	{
	public:
		int m_Ty ;
		void* m_DwData ;
	public:
		CNodeData():m_Ty(0),m_DwData(NULL) {} ;
		~CNodeData() {} ;
	};
public:
	CDlgAircraftConfigurationSpecView(CAircaftLayOut* _layout ,InputTerminal* _terminal ,CTermPlanDoc* doc,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAircraftConfigurationSpecView();

// Dialog Data
	enum { IDD = IDD_DIALOG_AIRCAFT_CONFIG_DEFINE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	enum {TY_NODE_LAYOUT,TY_NODE_DECK};
	DECLARE_MESSAGE_MAP()
protected:
	CAircaftLayOut* m_layout ;
	InputTerminal* m_Terminal ;
	std::vector<CNodeData*> m_treeNodeData ;
	CRect m_oldRect ;
	float m_modelHeight ;
	float m_modelWidth ;
	float m_xPercent ;
	float m_ypercent ;
	double m_XmodelHeight ;
	double m_XmodelWidth ;
	double m_YmodelHeight ;
	double m_YmodeWidth ;
	CTermPlanDoc*  m_Doc ;
protected:
	CCoolTree m_tree ;
	HTREEITEM m_LayoutItem ;
	HTREEITEM m_Rclick ;
protected:
	BOOL OnInitDialog() ;
	void OnSize(UINT nType, int cx, int cy) ;
     void OnPaint() ;
	 LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) ;
protected:
	void InitDialogCaption() ;
	void InitConfigTree() ;
	void InitModelView() ;
protected:
	void AddDeckTreeNode(HTREEITEM _item , CDeck* _deck) ;
   void  OnContextMenu(CWnd* /*pWnd*/, CPoint point) ;
protected:
	void OnAddDeck() ;
	void OnDeckComment() ;
	void OnDeckHelp() ;
	void OnDelDeck() ;
	void OnEditDeck() ;
protected:
	void OnSave() ;
	void OnSaveAs() ;
	void OnOK() ;
	void OnCancel() ;

	void DisplayAllBmpInCtrl() ;
	void DisplayBmpInCtrl(HBITMAP hBmp,UINT nID) ;
	
	void AddLineToPic(CPaintDC* pDC ,CDeck* _deck) ;
	void DelLineFromPic() ;
	int GetPenWidth(CDeck* _deck) ;
};
