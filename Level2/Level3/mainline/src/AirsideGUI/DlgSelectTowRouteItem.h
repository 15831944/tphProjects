#pragma once

#include <iostream>
#include <vector>
#include "..\InputAirside\IntersectionNodesInAirport.h"
#include "../InputAirside/IntersectionNode.h"
#include "../InputAirside/TowOperationRoute.h"
class RunwayExit;
class IntersectNode;
class ALTObject;
class ALTObjectGroup;
class CTowingRoute;
class CAirportTowRouteNetwork;


// CDlgSelectTowRouteItem dialog

class CDlgSelectTowRouteItem : public CDialog
{
	DECLARE_DYNAMIC(CDlgSelectTowRouteItem)

public:
	CDlgSelectTowRouteItem(int nProjID,CTowOperationRouteItem* pParentItem, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSelectTowRouteItem();

// Dialog Data
	//enum { IDD = IDD_DIALOG_SELECTTOWROUTEITEM };

public:
	afx_msg void OnDestroy();

public:
	int GetSelALTObjID();
	void SetSelALTObjID(int nSelALTObjID);
	int GetIntersectNodeID();
	void SetIntersectNodeID(int nIntersectNodeID);
	CString GetSelALTObjName();
	void SetSelALTObjName(CString strALTObjName);


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()


private:
	void SetListCtrlContent();
	void ClearListCtrlContent();


	BOOL IntersectNodeInList(IntersectionNode *pNode, IntersectionNodeList *pNodeList);

	void SetALTObjIntersectTaxiway(ALTObject *pALTObj);
	void SetALTObjIntersectGroundRoute(ALTObject *pALTObj);

	void SetRunwayIntersectTaxiway(Runway *pRunway);
	void SetRunwayIntersectGroundRoute(Runway *pRunway);

	void SetTaxiwayIntersectTaxiway(Taxiway *pTaxiway);
	void SetTaxiwayIntersectGroundRoute(Taxiway *pTaxiway);

	void SetGroundRouteIntersectTaxiway(CTowingRoute *pGroundRoute);
	void SetGroundRouteIntersectGroundRoute(CTowingRoute *pGroundRoute);

	//insert taxiway item which intersect with parentObject,
	//pNode is the node pTaxiway intersect with pParentObj, pNodeList is all of the pTaxiway intersect node 
	void InsertTaxiwayIntersectItem( Taxiway *pTaxiway, IntersectionNode *pNode, IntersectionNodeList *pNodeList);
	void InsertRunwayIntersectItem( Runway *pRunway, IntersectionNode *pNode, IntersectionNodeList *pNodeList);
	void InsertGroundRouteIntersectItem(CTowingRoute* pGroundRoute, IntersectionNode *pNode, IntersectionNodeList *pNodeList);
	void InsertGroundRouteIntersectItem(CTowingRoute* pGroundRoute, IntersectionNodeList *pNodeList);

	void InsertGroundRoutes(std::vector<CTowingRoute *>& vGroundRoutes);
	void InsertTaxiways(std::set<ALTObject*>& vTaxiways);
	void InsertRunways(std::set<ALTObject*>& vRunways);


public:
	CTreeCtrl m_ALTObjTreeCtrl;
	HTREEITEM m_TaxiwayItem;
	HTREEITEM m_GroundRouteItem;
	HTREEITEM m_RunwayItem;

	int m_nProjID;
	int m_nRunwayExitID;
	int m_nALTObjID;
	int m_nIntersectNodeIDInALTObj;
	int m_nSelALTObjID;
	int m_nIntersectNodeID;
	CTowOperationRouteItem::ItemType m_enumItemType;
	CString m_strSelALTObjName;
	CAirportTowRouteNetwork* m_pAirportGroundNetwork;
	 
	ALTObject* m_pParentObj;
public:
	afx_msg void OnTvnSelchangedTreeAltobjlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();




};
