#pragma once
#include "../InputAirside/ALTObject.h"
#include "../InputAirside/ALTAirport.h"
#include "../AirsideReport/AirsideReportNode.h"

// CDLgSelectAirsideNode dialog

class CDlgSelectAirsideNode : public CDialog
{
	DECLARE_DYNAMIC(CDlgSelectAirsideNode)
public:
	enum enumObjNodeType
	{
		objNodeType_Unkown = 0,
		objNodeType_StandRoot,
		objNodeType_StandLeaf,
		objNodeType_Stand ,
		objNodeType_WaypointRoot,
		objNodeType_WaypointLeaf,
		objNodeType_Waypoint,
		objNodeType_InterNodeRoot,
		objNodeType_InterNode,
		objNodeType_Airport,
		objNodeType_Airspace,
		objNodeType_TaxiwayLeaf,
		objNodeType_Taxiway
	};

	class nodeData
	{
	public:
		nodeData(enumObjNodeType type = objNodeType_Unkown,CString name = "",int id = -1)
		{
			nodeType = type;
			nNodeID = id;
			strName = name;
		}
		enumObjNodeType nodeType;
		int nNodeID;
		CString strName;


	};
public:
	CDlgSelectAirsideNode(int nProjID, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSelectAirsideNode();

	// Dialog Data
	enum { IDD = IDD_DIALOG_REPORT_SELECTALTOBJECT };

	const ALTObjectID& GetALTObject() const { return m_selectedALTObjectID; }
	CAirsideReportNode GetSelectNode(){ return m_selNode;}


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	virtual afx_msg void OnSelChangedTree(NMHDR *pNMHDR, LRESULT *pResult);
	virtual void InitTreeContentsForAirport(HTREEITEM hAirport, int nAirportID);
	HTREEITEM FindTreeChildItemByName(LPCTSTR lpszName, HTREEITEM hParentItem);
	int GetTreeItemDepthIndex(HTREEITEM hTreeItem);
	void InitTreeContentsForAirspace();
	void InitIntersectionNodeToAirport(HTREEITEM hAirport, int nAirportID);

	CTreeCtrl m_wndALTObjectTree;
	//ALTOBJECT_TYPE m_objectType;
	int m_nProjectID;
	ALTObjectID m_selectedALTObjectID;

	CAirsideReportNode m_selNode;
	virtual void OnOK();

	//get all object id of hCurItem
	void GetObjNodes(std::vector<int>& vObjID,HTREEITEM hCurItem);
	void SearchChildItem(std::vector<int>& vObjID, HTREEITEM hSibingItem);
};
class CDlgSelectAirsideNodeByType : public CDlgSelectAirsideNode
{
public:
	CDlgSelectAirsideNodeByType(int nProjID,Airsdie_Resource_ID _ResID , CWnd* pParent = NULL); 
	~CDlgSelectAirsideNodeByType() ;
protected:
	Airsdie_Resource_ID m_ResType ;
protected:
	BOOL OnInitDialog() ;
protected:
	void OnOK() ;
	 void InitTreeContentsForAirport(HTREEITEM hAirport, int nAirportID);
};
class CDlgTaxiWaySelect  : public CDlgSelectAirsideNode
{
public:
	CDlgTaxiWaySelect(int nProjID, CWnd* pParent = NULL);   // standard constructor
protected:
protected:
	BOOL OnInitDialog() ;
};