#pragma once
#include "../InputAirside/ALTObject.h"
#include "AirsideReportNode.h"
// CDlgReportSelectObject dialog

class CDlgReportSelectObject : public CDialog
{
	DECLARE_DYNAMIC(CDlgReportSelectObject)


	enum enumObjNodeType
	{
		objNodeType_Unkown = 0,
		objNodeType_StandRoot,
		objNodeType_Stand ,
		objNodeType_WaypointRoot,
		objNodeType_Waypoint,
		objNodeType_InterNodeRoot,
		objNodeType_InterNode,
		objNodeType_Airport,
		objNodeType_Airspace
	};

	class nodeData
	{
	public:
		nodeData(enumObjNodeType type = objNodeType_Unkown,int id = -1)
		{
			nodeType = type;
			nNodeID = id;
		}
	enumObjNodeType nodeType;
	int nNodeID;


	};



public:
	CDlgReportSelectObject(int nProjID, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgReportSelectObject();

// Dialog Data
	//enum { IDD = IDD_DIALOG_SELECTALTOBJECT };


	const ALTObjectID& GetALTObject() const { return m_selectedALTObjectID; }
	//int GetObjectID(){ return m_nSelObjID;}
	//ALTOBJECT_TYPE GetALTObjectType(void)const{return (m_objectType);} 
	int GetProjectID(void){return (m_nProjectID);}

	//int GetSelectedObjectType(){ return m_nSelObjType;}
	CAirsideReportNode GetSelectNode(){ return m_selNode;}

protected:

	//int m_nSelObjID;
	//int m_nSelObjType;//0,object;1,intersection node
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	afx_msg void OnSelChangedTree(NMHDR *pNMHDR, LRESULT *pResult);
	void InitTreeContentsForAirport(HTREEITEM hAirport, int nAirportID);
	HTREEITEM FindTreeChildItemByName(LPCTSTR lpszName, HTREEITEM hParentItem);
	int GetTreeItemDepthIndex(HTREEITEM hTreeItem);
	void InitTreeContentsForAirspace();
	void InitIntersectionNodeToAirport(HTREEITEM hAirport, int nAirportID);

	CTreeCtrl m_wndALTObjectTree;
	//ALTOBJECT_TYPE m_objectType;
	int m_nProjectID;
	ALTObjectID m_selectedALTObjectID;

	CAirsideReportNode m_selNode;
};
