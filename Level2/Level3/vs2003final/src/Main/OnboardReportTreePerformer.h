//OnboardReportPerformer.h declare tree ctrl in report contorl view interface
#pragma once
#include "OnboardReport/ReportType.h"
#include "Common/FLT_CNST.H"
#include "Common/NewPassengerType.h"

class CARCTreeCtrlEx;
class OnboardBaseParameter;
class InputTerminal;
//----------------------------------------------------------------------------------------------
//Summary:
//		Tree in onboard report control view operation
//----------------------------------------------------------------------------------------------
class COnboardReportPerformer
{
public:
	//------------------------------------------------------------------------------------------
	//Summary:
	//		OnboardReportPerformer instance
	//------------------------------------------------------------------------------------------
	COnboardReportPerformer(CARCTreeCtrlEx *pTreeCtrl,InputTerminal* pInTerm, OnboardBaseParameter *pParam,OnboardReportType enumReportType );

	//-----------------------------------------------------------------------------------------
	//Summary:
	//		Destroy class object
	//-----------------------------------------------------------------------------------------
	virtual ~COnboardReportPerformer();

	enum TreeNodeType
	{
		FLIGHTTYPE_ITEM_ROOT,
		PASSGENERTYPE_ITEM_ROOT,
		FLIGHTTYPE_ITEM,
		PASSENGERTYPE_ITEM
	};
	//---------------------------------------------------------------------------------------
	//Summary:
	//		tree node data
	//----------------------------------------------------------------------------------------
	class TreeNodeData
	{
	public:
		TreeNodeData()
		{

		}
		virtual ~TreeNodeData()
		{

		}
	
		DWORD   m_dwData;//tree node data
		TreeNodeType m_nodeType;//tree node type
	};
	
public:
	//------------------------------------------------------------------------------------------
	//Summary:
	//		handle report control view tool bar add message
	//Parameter:
	//		hTreeItem[in]: node of tree
	//------------------------------------------------------------------------------------------
	virtual void OnToolBarAdd(HTREEITEM hTreeItem) = 0;
	//------------------------------------------------------------------------------------------
	//Summary:
	//		handle report control view tool bar del message
	//Parameter:
	//		hTreeItem[in]: node of tree
	//------------------------------------------------------------------------------------------
	virtual void OnToolBarDel(HTREEITEM hTreeItem) = 0;
	//------------------------------------------------------------------------------------------
	//Summary:
	//		handle report control view tool bar edit message
	//Parameter:
	//		hTreeItem[in]: node of tree
	//-----------------------------------------------------------------------------------------
	virtual void OnToolBarEdit(HTREEITEM hTreeItem)= 0;

	//-----------------------------------------------------------------------------------------
	//Summary:
	//		default message handle
	//Parameter:
	//		message[in]: message type
	//		wParam[in out]:	pass object
	//		lParam[in out]: pass object
	//-----------------------------------------------------------------------------------------
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) = 0;

	//-----------------------------------------------------------------------------------------
	//Summary:
	//		handle control update message
	//Parameter:
	//		CCmdUI[in]: control state
	//-----------------------------------------------------------------------------------------
	virtual void OnUpdateToolBarAdd(CCmdUI *pCmdUI) = 0;
	//-----------------------------------------------------------------------------------------
	//Summary:
	//		handle control update message
	//Parameter:
	//		CCmdUI[in]: control state
	//-----------------------------------------------------------------------------------------
	virtual void OnUpdateToolBarDel(CCmdUI *pCmdUI) = 0;

	//-----------------------------------------------------------------------------------------
	//Summary:
	//		Data operation
	//-----------------------------------------------------------------------------------------
	virtual void InitTree() = 0;
	virtual void LoadData() = 0;
	virtual void SaveData() = 0;
	OnboardReportType GetReportType() const;
	//-----------------------------------------------------------------------------------------

	//-----------------------------------------------------------------------------------------
	//Summary:
	//		tree operation
	//-----------------------------------------------------------------------------------------
	HTREEITEM InsertItem(HTREEITEM hItem,const CString& sNode,TreeNodeData* pNodeData);

	//-----------------------------------------------------------------------------------------
	//Summary:
	//		refresh parameter data when necessary
	//-----------------------------------------------------------------------------------------
	void UpdateOnboardParameter(OnboardBaseParameter* pParameter);
protected:
	CARCTreeCtrlEx				*m_pTreeCtrl;//pointer of tree
	OnboardBaseParameter		*m_pParameter;//user input onboard parameter
	std::vector<TreeNodeData*>	m_vNodeDataList;
	InputTerminal				*m_pInTerm;
	OnboardReportType			m_enumReportType;
};


//---------------------------------------------------------------------------------------------
//Summary:
//		Duration report tree performer
//---------------------------------------------------------------------------------------------
class COnboardReportTreeDefaultPerformer : public COnboardReportPerformer
{
public:
	COnboardReportTreeDefaultPerformer(CARCTreeCtrlEx *pTreeCtrl,InputTerminal* pInTerm, OnboardBaseParameter *pParam, OnboardReportType enumReportType);
	virtual ~COnboardReportTreeDefaultPerformer();

	class DefaultTreeNodeData : public COnboardReportPerformer::TreeNodeData
	{
	public:
		DefaultTreeNodeData()
			:m_paxtype(NULL)
		{

		}
		virtual ~DefaultTreeNodeData()
		{

		}

		FlightConstraint m_flconst;
		CPassengerType	 m_paxtype;
	};
public:
	//------------------------------------------------------------------------------------------
	//Summary:
	//		handle report control view tool bar add message
	//Parameter:
	//		hTreeItem[in]: node of tree
	//------------------------------------------------------------------------------------------
	virtual void OnToolBarAdd(HTREEITEM hTreeItem);
	//------------------------------------------------------------------------------------------
	//Summary:
	//		handle report control view tool bar del message
	//Parameter:
	//		hTreeItem[in]: node of tree
	//------------------------------------------------------------------------------------------
	virtual void OnToolBarDel(HTREEITEM hTreeItem);
	//------------------------------------------------------------------------------------------
	//Summary:
	//		handle report control view tool bar edit message
	//Parameter:
	//		hTreeItem[in]: node of tree
	//-----------------------------------------------------------------------------------------
	virtual void OnToolBarEdit(HTREEITEM hTreeItem);

	//-----------------------------------------------------------------------------------------
	//Summary:
	//		default message handle
	//Parameter:
	//		message[in]: message type
	//		wParam[in out]:	pass object
	//		lParam[in out]: pass object
	//-----------------------------------------------------------------------------------------
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	//-----------------------------------------------------------------------------------------
	//Summary:
	//		handle control update message
	//Parameter:
	//		CCmdUI[in]: control state
	//-----------------------------------------------------------------------------------------
	virtual void OnUpdateToolBarAdd(CCmdUI *pCmdUI);
	//-----------------------------------------------------------------------------------------
	//Summary:
	//		handle control update message
	//Parameter:
	//		CCmdUI[in]: control state
	//-----------------------------------------------------------------------------------------
	virtual void OnUpdateToolBarDel(CCmdUI *pCmdUI);

	//-----------------------------------------------------------------------------------------
	//Summary:
	//		Data operation
	//-----------------------------------------------------------------------------------------
	virtual void InitTree();
	virtual void LoadData();
	virtual void SaveData();
	//-----------------------------------------------------------------------------------------


	//-----------------------------------------------------------------------------------------
	//Summary:
	//		init tree with default data
	//-----------------------------------------------------------------------------------------
	void InitDefaultTree();

	//----------------------------------------------------------------------------------------
	//Summary:
	//		check flight constrain valid
	//----------------------------------------------------------------------------------------
	bool ValidFlightConstraint(void);
protected:
	HTREEITEM m_hPaxTypeRootItem;//passenger type root item
	HTREEITEM m_hFltTypeRootItem;//flight type root item
};

