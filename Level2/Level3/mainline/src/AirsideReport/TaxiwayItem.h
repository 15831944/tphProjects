#pragma once
#include "parameters.h"

//CTaxiwayItem may a group of taxiway or a taxiway
class AIRSIDEREPORT_API CTaxiwayItem
{
public:
	class CTaxiwayNodeItem
	{
	public:
		CTaxiwayNodeItem():	m_nNodeID(-1),m_nIndex(-1)
		{

		}
		~CTaxiwayNodeItem()
		{

		}

	public:
		//the index of node, start from 1. 1,2,3,4.....
		int m_nIndex;
		int m_nNodeID;//node id, unique id
		CString m_strName;

	};

	CTaxiwayItem()
	{
		//taxiway
		m_nTaxiwayID = -1;
		m_strTaxiwayName = _T("");

		m_nodeFrom.m_strName = _T("START");
		m_nodeTo.m_strName = _T("END");
	}
	~CTaxiwayItem()
	{

	}

public:
	//taxiway
	int m_nTaxiwayID;
	CString m_strTaxiwayName;


	//from intersection
	CTaxiwayNodeItem m_nodeFrom;

	//to intersection
	CTaxiwayNodeItem m_nodeTo;

	//all the nodes which from nodefrom to nodeto
	std::vector<CTaxiwayNodeItem> m_vNodeItems;

	bool IsNodeSelected(int nNodeID);

	int InitTaxiNodeIndex(int nStartIndex);

	//check the node list whether in the same taxiway
	bool fit(const std::vector<CString>& nodeNameList);
};

class AIRSIDEREPORT_API CTaxiwayItemList : public std::vector<CTaxiwayItem*>
{
public:
	CTaxiwayItemList();
	virtual ~CTaxiwayItemList();

	std::vector<CTaxiwayItem::CTaxiwayNodeItem > getAllNodes();
	CString GetNodeName(int nNodeID);

	//make a index start from 1 to mark node
	void InitTaxiNodeIndex();
	int GetNodeIndex(int nNodeID);
	bool IsNodeSelected( int nNodeID );

	int getItemCount();
	CTaxiwayItem* getItem(int nIndex);
	bool IfNodeListInSameTaxiwayAndGetTaxiwayList(const std::vector<CString>& nodeNameList,CTaxiwayItemList& taxiwayList);

	void AddItem(const CTaxiwayItem& taxiwayItem);

	void ClearTaxiwayItems();

	bool GetUseAllTaxiway() const { return m_bAllTaxiway; }
	void SetUseAllTaxiway(bool val) { m_bAllTaxiway = val; }

	void WriteParameter(ArctermFile& _file);
	void ReadParameter(ArctermFile& _file);

protected:
	bool m_bAllTaxiway;
};