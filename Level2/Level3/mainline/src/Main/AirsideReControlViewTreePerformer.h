#pragma once

#include "AirsideReport\AirsideReportNode.h"
#include "AirsideReport\TypeDefs.h"
#include "..\Common\Flt_cnst.h"
#include "..\Common\ALTObjectID.h"
#include "AirsideReport\AirsideIntersectionReportParam.h"
#include "TermPlanDoc.h"
#include "MFCExControl\CoolTree.h"
class CTreeCtrl;
class CParameters;
//this class uses for  airside report control view tree view operation
namespace AirsideReControlView
{
	class CTreePerformer
	{
	public:
		class BaseTreeItemData
		{
		public:
			BaseTreeItemData(){}
			virtual ~BaseTreeItemData(){}
		protected:
		private:
		};
	public:
		CTreePerformer(CTermPlanDoc* pTermDoc, CCoolTree *pTreeCtrl, CParameters *pParam);
		virtual ~CTreePerformer(void);

	public:
		virtual void InitTree(){}
		virtual void LoadData(){}
		virtual void SaveData(){}
		virtual reportType GetType() {return UnknownType;}
		void SetParamter(CParameters* pPara) {m_pParam = pPara;}

	public:
		//tool bar add
		virtual void OnToolBarAdd(HTREEITEM hTreeItem){}
		//tool bar del
		virtual void OnToolBarDel(HTREEITEM hTreeItem){}
		//toolbar edit
		virtual void OnToolBarEdit(HTREEITEM hTreeItem){}

		//observer the message from airside report control view, state changed
		virtual void OnUpdateToolBarAdd(CCmdUI *pCmdUI);
		virtual void OnUpdateToolBarDel(CCmdUI *pCmdUI);
		
		virtual void OnTreeItemDoubleClick(HTREEITEM hTreeItem){}
		virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

		HTREEITEM InsertItem(HTREEITEM hParentItem, const CString& strName, BaseTreeItemData *pItemData);
		
	protected:

		std::vector<BaseTreeItemData *> m_vPoiterNeedDel;
		CCoolTree *m_pTreeCtrl;
		CParameters *m_pParam;
		CTermPlanDoc* m_pTermDoc;
	};

	enum TreeItemType
	{
		Item_RunwayRoot = 0,
		Item_Runway,
		Item_Operation,
		Item_AirRouteRoot,
		Item_AirRoute,
		Item_FlightRoot,
		Item_Flight,
		Item_Root,
		Item_Item,
        Item_MultiRunRoot,
        Item_Runs
	};

	class TreeItemData : public CTreePerformer::BaseTreeItemData
	{
	public:
		TreeItemData()
		{
			itemType = Item_RunwayRoot;
			nOperation = 0;
			nObjID = -1;
		}
		~TreeItemData()
		{

		}

	public:
		TreeItemType itemType;
		std::vector<CAirsideReportRunwayMark> vRunwayMark;
		int nOperation;

		int nObjID;
		CString strObjName;

		ALTObjectID altObjID;

		FlightConstraint fltCons;

	};

	class CRunwayUtilizationTreePerformer : public CTreePerformer
	{
	public:

	public:
		CRunwayUtilizationTreePerformer(CTermPlanDoc* pDoc, CCoolTree *pTreeCtrl, CParameters *pParam);
		~CRunwayUtilizationTreePerformer();

	public:
		virtual void InitTree();
		virtual void LoadData();
		virtual void SaveData();
		virtual reportType GetType() {return Airside_RunwayUtilization;}

	public:
		void OnToolBarAdd(HTREEITEM hTreeItem);
		void OnToolBarDel(HTREEITEM hTreeItem);
		void OnToolBarEdit(HTREEITEM hTreeItem);

		virtual void OnTreeItemDoubleClick(HTREEITEM hTreeItem);

		virtual void OnUpdateToolBarAdd(CCmdUI *pCmdUI);
		virtual void OnUpdateToolBarDel(CCmdUI *pCmdUI);

	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	protected:
		void InitDefaltTree();
		void InitDefaltRunway(HTREEITEM hRunwayItem);
	protected:
		HTREEITEM m_hItemRunwayRoot;
		HTREEITEM m_hOperationItemSelected;

		CString GetRunwayNames(std::vector<CAirsideReportRunwayMark> & vRunwayMark);

		CString GetOperationName(int nOperation);
	};


	class CRunwayDelayParaTreePerformer : public CTreePerformer
	{
	public:
	public:
		CRunwayDelayParaTreePerformer(CTermPlanDoc* pDoc, CCoolTree *pTreeCtrl, CParameters *pParam);
		~CRunwayDelayParaTreePerformer();

	public:
		virtual void InitTree();
		virtual void LoadData();
		virtual void SaveData();
		virtual reportType GetType() {return Airside_RunwayDelay;}

	public:
		void OnToolBarAdd(HTREEITEM hTreeItem);
		void OnToolBarDel(HTREEITEM hTreeItem);

		virtual void OnTreeItemDoubleClick(HTREEITEM hTreeItem);

		virtual void OnUpdateToolBarAdd(CCmdUI *pCmdUI);
		virtual void OnUpdateToolBarDel(CCmdUI *pCmdUI);

		virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	protected:
		void InitDefaltTree();
		void InitDefaltRunway(HTREEITEM hRunwayItem);
	protected:
		HTREEITEM m_hItemRunwayRoot;
		HTREEITEM m_hOperationItemSelected;

		CString GetRunwayNames(std::vector<CAirsideReportRunwayMark> & vRunwayMark);

		CString GetOperationName(int nOperation);
	};

	class CRunwayCrossingsTreePerformer : public CTreePerformer
	{
	public:
		enum RunwayCrossingsTreeItemType
		{
			Item_NonType = 0,
			Item_RunwayRoot,
			Item_Runway,
			Item_TaxiwayRoot,
			Item_Taxiway
		};

		class RunwayCrossingsTreeItemData
		{
		public:
			RunwayCrossingsTreeItemData()
				:itemType(Item_NonType)
				,sObjName(_T(""))
			{

			}
	
			RunwayCrossingsTreeItemType itemType;
			ALTObjectID sObjName;
		};
	public:
		CRunwayCrossingsTreePerformer(CTermPlanDoc* pDoc, CCoolTree *pTreeCtrl, CParameters *pParam);
		~CRunwayCrossingsTreePerformer();
	public:
		virtual void InitTree();
		virtual void LoadData();
		virtual void SaveData();
		virtual reportType GetType() {return Airside_RunwayCrossings;}

	public:
		void OnToolBarAdd(HTREEITEM hTreeItem);
		void OnToolBarDel(HTREEITEM hTreeItem);

		virtual void OnUpdateToolBarAdd(CCmdUI *pCmdUI);
		virtual void OnUpdateToolBarDel(CCmdUI *pCmdUI);

	protected:
		void InitDefaltTree();
	};

	class CStandOperationsTreePerformer : public CTreePerformer
	{
		public:
			CStandOperationsTreePerformer(CTermPlanDoc* pDoc, CCoolTree *pTreeCtrl, CParameters *pParam);
			~CStandOperationsTreePerformer();
		public:
			virtual void InitTree();
			virtual void LoadData();
			virtual void SaveData();
			virtual reportType GetType() {return Airside_StandOperations;}

		public:
			void OnToolBarAdd(HTREEITEM hTreeItem);
			void OnToolBarDel(HTREEITEM hTreeItem);

			virtual void OnUpdateToolBarAdd(CCmdUI *pCmdUI);
			virtual void OnUpdateToolBarDel(CCmdUI *pCmdUI);

		protected:
			void InitDefaltTree();
	};


	class CAirsideIntersectionTreePerformer : public CTreePerformer
	{
	public:
		enum TreeItemType
		{
			Item_NonType = 0,
			Item_Root,
			Item_TaxiwayRoot,
			Item_NodeFrom,
			Item_NodeTo
		};

		class TreeItemData :public BaseTreeItemData
		{
		public:
			TreeItemData()
				:itemType(Item_NonType)
				,strObjName(_T(""))
				,m_nNodeID(-1)
				,m_nTaxiwayID(-1)
			{

			}

			TreeItemType itemType;

			//if taxiway, taxiway name
			//if node, node name
			CString strObjName;
			//if taxiway, taxiway id
			int m_nTaxiwayID;
			//if node, node id
			int m_nNodeID;
		};
	public:
		CAirsideIntersectionTreePerformer(CTermPlanDoc* pDoc, CCoolTree *pTreeCtrl, CParameters *pParam);
		~CAirsideIntersectionTreePerformer();
	public:
		virtual void InitTree();

		virtual void LoadData();
		virtual void SaveData();

		void InitTaxiwayItemNodes(CTaxiwayItem &taxiwayItem );
		virtual reportType GetType() {return Airside_IntersectionOperation;}

	public:
		virtual void OnToolBarAdd(HTREEITEM hTreeItem);
		void OnToolBarDel(HTREEITEM hTreeItem);

		virtual void OnUpdateToolBarAdd(CCmdUI *pCmdUI);
		virtual void OnUpdateToolBarDel(CCmdUI *pCmdUI);
		virtual void OnTreeItemDoubleClick(HTREEITEM hTreeItem);
	protected:
		void InitDefaltTree();
		virtual void InsertAllTaxiwayItem();
		//root item, Intersections
		HTREEITEM m_hRootItem;
		HTREEITEM m_hFromOrToNodeSelected;
		HTREEITEM m_hAllTaxiwayItem;


		virtual void AddTaxiwayItem(const CTaxiwayItem& taxiwayItem);

		virtual void OnEditIntersectionFrom(HTREEITEM hTreeItem, int nTaxiwayID);
		virtual void OnEditIntersectionTo(HTREEITEM hTreeItem, int nTaxiwayID);
		virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
		
	protected:
		class NodeInfo
		{
		public:
			NodeInfo(){nNodeID = -1; dDistFromStart = 0.0;}
			~NodeInfo(){}

			int nNodeID;
			CString strNodeName;
			double dDistFromStart;

			bool operator < (const NodeInfo& info)const
			{
				return dDistFromStart < info.dDistFromStart ? true : false ;
			}
		protected:
		private:
		};

		std::map<int, std::vector<NodeInfo> > m_mapTaxiwayNodes;

		virtual std::vector<NodeInfo> GetTaxiwayNodes(int nTaxiwayID);
	};

	class CAirsideTaxiwayUtilizationTreePerformer : public CAirsideIntersectionTreePerformer
	{
	public:
		CAirsideTaxiwayUtilizationTreePerformer(CTermPlanDoc* pDoc, CCoolTree *pTreeCtrl, CParameters *pParam);
		~CAirsideTaxiwayUtilizationTreePerformer();
	public:
		virtual void InitTree();
		virtual void SaveData();
		virtual reportType GetType() {return Airside_TaxiwayUtilization;}
		virtual std::vector<NodeInfo> GetTaxiwayNodes(int nTaxiwayID); 
		virtual void OnEditIntersectionFrom(HTREEITEM hTreeItem, int nTaxiwayID);
		virtual void OnEditIntersectionTo(HTREEITEM hTreeItem, int nTaxiwayID);
		virtual void AddTaxiwayItem(const CTaxiwayItem& taxiwayItem);
		virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
		HTREEITEM InitDefaltTree();
		virtual void InsertAllTaxiwayItem();
		void OnToolBarAdd(HTREEITEM hTreeItem);
		void OnToolBarDel(HTREEITEM hTreeItem);
		void OnUpdateToolBarDel( CCmdUI *pCmdUI );

		void InitTaxiwaySpecifiedItemNodes(CTaxiwayItem &taxiwayItem);
	};
	class CAirsideControllerWorkloadTreePerformer : public CTreePerformer
	{
	public:
		CAirsideControllerWorkloadTreePerformer(CTermPlanDoc* pDoc, CCoolTree *pTreeCtrl, CParameters *pParam);
		~CAirsideControllerWorkloadTreePerformer();
	public:
		virtual void InitTree();
		virtual void LoadData();
		virtual void SaveData();
		virtual reportType GetType();
		virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
		HTREEITEM InitDefaltTree();
		void OnToolBarAdd(HTREEITEM hTreeItem);
		void OnToolBarDel(HTREEITEM hTreeItem);
		void OnUpdateToolBarDel( CCmdUI *pCmdUI );

		void OnTreeItemDoubleClick(HTREEITEM hTreeItem);
	protected:
		TreeItemData *NewItemData(TreeItemType enumType, ALTObjectID objName);
		TreeItemData *NewItemData(TreeItemType enumType, CString strWeightName ,int nWeight);
		BOOL IsObjectSelected(HTREEITEM hObjRoot,const ALTObjectID& altID);

		CString FormatWeightText(CString strWeightName ,int nWeight) const;
	protected:
		HTREEITEM m_hSectorRoot;
		HTREEITEM m_hAreaRoot;
		HTREEITEM m_hWeightRoot;

		//only the double click item
		HTREEITEM m_hItemSelected;
	};

	class CTakeoffProcessTreePerformer : public CTreePerformer
	{
	public:
		CTakeoffProcessTreePerformer(CTermPlanDoc* pDoc, CCoolTree *pTreeCtrl, CParameters *pParam);
		~CTakeoffProcessTreePerformer();
	public:
		virtual void InitTree();
		virtual void LoadData();
		virtual void SaveData();
		virtual reportType GetType() {return Airside_TakeoffProcess;}

	public:
		void OnToolBarAdd(HTREEITEM hTreeItem);
		void OnToolBarDel(HTREEITEM hTreeItem);

		virtual void OnUpdateToolBarAdd(CCmdUI *pCmdUI);
		virtual void OnUpdateToolBarDel(CCmdUI *pCmdUI);

	protected:
		void InitDefaltTree();

	private:
		std::vector<TreeItemData*> m_vData;
	};
}
