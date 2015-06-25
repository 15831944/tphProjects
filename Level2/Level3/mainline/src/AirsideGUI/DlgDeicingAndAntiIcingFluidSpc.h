#pragma once

#include "../MFCExControl/CoolTree.h"
#include "../MFCExControl/XTResizeDialog.h"
#include "NodeViewDbClickHandler.h"

// CDlgDeicingAndAntiIcingFluidSpc dialog
class CAircraftDeicingAndAnti_icingsSettings ;
class CAircraftDeicingAndAnti_icingsSetting ;
class CAircraftSurfacecondition ;
class CPrecipitationtype ;
class InputTerminal ;

class CDlgDeicingAndAntiIcingFluidSpc : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgDeicingAndAntiIcingFluidSpc)

public:
	CDlgDeicingAndAntiIcingFluidSpc(InputTerminal* inputterminal ,InputAirside* _pInputAirside , PSelectProbDistEntry pSelectProbDistEntry ,PSelectFlightType	_pSelectFlightType,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgDeicingAndAntiIcingFluidSpc();

// Dialog Data
	enum { IDD = IDD_DIALOG_SURFACE_CONDITION };
	enum { TY_HOLDOVER_TIME ,TY_Inspection_time , TY_DE_ICE , TY_Anti_ICE};
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
protected:
	CCoolTree m_treeCtrl ;
	CToolBar m_toolBar ;
    InputTerminal* m_Terminal ;
	CAircraftDeicingAndAnti_icingsSettings* m_Setting ;
	PSelectProbDistEntry m_pSelectProbDistEntry ;
	InputAirside* m_InputAirside ;
	PSelectFlightType	m_pSelectFlightType ;
protected:
	BOOL OnInitDialog() ;
	void OnSize(UINT nType, int cx, int cy) ;
	int OnCreate(LPCREATESTRUCT lpCreateStruct) ;
    LRESULT  DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) ;
	void OnTvnSelchangedTreeHoldShortline(NMHDR *pNMHDR, LRESULT *pResult) ;
	void OnNewButton() ;
	void OnDelButton() ;
	void OnOK() ;
	void OnCancel() ;
	void OnSave() ;
	void OnSaveAs() ;
	void OnLoad() ;

protected:
    void OnInitToolBar() ;
	void OnInitTree()  ;

	void AddFlightTyTreeNode(CAircraftDeicingAndAnti_icingsSetting* _setting) ;
	void DelFlightTyTreeNode(HTREEITEM node) ;

	void AddConditionTreeNode(CAircraftSurfacecondition* condition,HTREEITEM _item) ;
    
	BOOL CheckFlightTyIsDefined(FlightConstraint& _constraint) ;
	void AddPrecipitationTypeNode(CPrecipitationtype* type,HTREEITEM _item) ;
protected:
	CString FormatFlightTyNodeName(CString& str) ;
	CString FormatConditionNodeName(CString& str) ;
	CString FormatPrecipitationtypeNodeName(CString& str) ;
	CString FormatHoldoverTimeNodeName(float time) ;
	CString FormatInspectionNodeName(const CString& time) ;
	CString FormatDe_ICENodeName(float time) ;
	CString FormatAnti_ICENodeName(float time) ;

	void HandleEditLable(CString str , int _Type ,HTREEITEM _item) ;
};
