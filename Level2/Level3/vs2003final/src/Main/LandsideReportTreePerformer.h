#pragma once

#include "../LandsideReport/LandsideBaseParam.h"
#include "../Common/HierachyName.h"


class CARCTreeCtrlEx;
class LandsideBaseParam;
class InputTerminal;
class InputLandside;

class LandsideReportTreeBasePerformer
{
public:

	//------------------------------------------------------------------------------------------
	//Summary:
	//		OnboardReportPerformer instance
	//------------------------------------------------------------------------------------------
	LandsideReportTreeBasePerformer(CARCTreeCtrlEx *pTreeCtrl,
		InputTerminal* pInTerm, 
		InputLandside *pInLandside,
		LandsideBaseParam *pParam,
		LandsideReportType enumReportType );

	//-----------------------------------------------------------------------------------------
	//Summary:
	//		Destroy class object
	//-----------------------------------------------------------------------------------------
	virtual ~LandsideReportTreeBasePerformer();

	enum TreeNodeType
	{
		VEHICLE_ITEM_ROOT = 0,
		VEHICLE_ITEM,
		PASSGENERTYPE_ITEM_ROOT,
		PASSENGERTYPE_ITEM,

		OBJECT_ROOT,
		FROM_ROOT, //from object root
		FROM_ITEM,
		TO_ROOT,//to object root
		TO_ITEM,
		FILTER_ITEM




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
			nVehicleType = -1;
		}
		virtual ~TreeNodeData()
		{

		}

		DWORD   m_dwData;//tree node data
		TreeNodeType m_nodeType;//tree node type

		int nVehicleType;//vehicle type 
		CHierachyName vehicleName;//vehicleName;

		ALTObjectID altObj;
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
	LandsideReportType GetReportType() const;
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
	void UpdateOnboardParameter(LandsideBaseParam* pParameter);
protected:
	CARCTreeCtrlEx				*m_pTreeCtrl;//pointer of tree
	LandsideBaseParam			*m_pParameter;//user input Landside parameter
	std::vector<TreeNodeData*>	m_vNodeDataList;
	InputTerminal				*m_pInTerm;
	LandsideReportType			m_enumReportType;
	InputLandside				*m_pLandside;
};


class LandsideReportTreeDefaultPerformer : public LandsideReportTreeBasePerformer
{
public:
	LandsideReportTreeDefaultPerformer(CARCTreeCtrlEx *pTreeCtrl,
		InputTerminal* pInTerm, 
		InputLandside *pInLandside,
		LandsideBaseParam *pParam,
		LandsideReportType enumReportType);
	virtual ~LandsideReportTreeDefaultPerformer();

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


protected:
	HTREEITEM m_hVehicleeRootItem;//Vehicle type root item

};




class LSReportTreeVehicleDelayPerformer : public LandsideReportTreeDefaultPerformer
{
public:
	LSReportTreeVehicleDelayPerformer(CARCTreeCtrlEx *pTreeCtrl,
		InputTerminal* pInTerm, 
		InputLandside *pInLandside,
		LandsideBaseParam *pParam,
		LandsideReportType enumReportType);
	virtual ~LSReportTreeVehicleDelayPerformer();

	
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
	//virtual void OnToolBarEdit(HTREEITEM hTreeItem);

	//-----------------------------------------------------------------------------------------
	//Summary:
	//		default message handle
	//Parameter:
	//		message[in]: message type
	//		wParam[in out]:	pass object
	//		lParam[in out]: pass object
	//-----------------------------------------------------------------------------------------
	//virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

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
	//virtual void LoadData();
	virtual void SaveData();
	//-----------------------------------------------------------------------------------------


	//-----------------------------------------------------------------------------------------
	//Summary:
	//		init tree with default data
	//-----------------------------------------------------------------------------------------
	//void InitDefaultTree();


protected:
	HTREEITEM m_hObjectRoot;
	HTREEITEM m_hFromRoot;//From object root item
	HTREEITEM m_hToRoot;//From object root item

};

class LSReportTreeResQueuePerformer : public LandsideReportTreeBasePerformer
{
public:
	LSReportTreeResQueuePerformer(CARCTreeCtrlEx *pTreeCtrl,
		InputTerminal* pInTerm, 
		InputLandside *pInLandside,
		LandsideBaseParam *pParam,
		LandsideReportType enumReportType);
	virtual ~LSReportTreeResQueuePerformer();


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
	//virtual void LoadData();
	virtual void SaveData();
	//-----------------------------------------------------------------------------------------


	//-----------------------------------------------------------------------------------------
	//Summary:
	//		init tree with default data
	//-----------------------------------------------------------------------------------------
	void InitDefaultTree();

	//-----------------------------------------------------------------------------------------
	//Summary:
	//		default message handle
	//Parameter:
	//		message[in]: message type
	//		wParam[in out]:	pass object
	//		lParam[in out]: pass object
	//-----------------------------------------------------------------------------------------
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	virtual void LoadData();
protected:
	HTREEITEM m_hObjectRoot;
	HTREEITEM m_hVehicleeRootItem;

};


class LSReportTreeResThroughputPerformer : public LandsideReportTreeDefaultPerformer
{
public:
	LSReportTreeResThroughputPerformer(CARCTreeCtrlEx *pTreeCtrl,
		InputTerminal* pInTerm, 
		InputLandside *pInLandside,
		LandsideBaseParam *pParam,
		LandsideReportType enumReportType);

	virtual ~LSReportTreeResThroughputPerformer();

	virtual void OnToolBarAdd(HTREEITEM hTreeItem);
	virtual void OnToolBarDel(HTREEITEM hTreeItem);
	virtual void OnToolBarEdit(HTREEITEM hTreeItem);

	virtual void OnUpdateToolBarAdd(CCmdUI *pCmdUI);
	virtual void OnUpdateToolBarDel(CCmdUI *pCmdUI);

	virtual void InitTree();
	virtual void SaveData();

	void InitDefaultTree();

	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	virtual void LoadData();

protected:
	HTREEITEM m_hObjectRoot;
};






















































































































