#pragma once
#include "parameters.h"
#include "TaxiwayItem.h"

class AIRSIDEREPORT_API CAirsideIntersectionReportParam :
	public CParameters
{
public:
	CAirsideIntersectionReportParam(void);
	~CAirsideIntersectionReportParam(void);


protected:
	CTaxiwayItemList m_vTaxiwayItems;
	//in summary report, need to select node id in chart
	int m_nSummaryNodeID;
	bool m_bAllTaxiway;

public:

	int getSummaryNodeID() const { return m_nSummaryNodeID; }
	void setSummaryNodeID(int val) { m_nSummaryNodeID = val; }

	bool GetUseAllTaxiway() const { return m_vTaxiwayItems.GetUseAllTaxiway(); }
	void SetUseAllTaxiway(bool val) { m_vTaxiwayItems.SetUseAllTaxiway(val); }

	std::vector<CTaxiwayItem::CTaxiwayNodeItem > getAllNodes(){return m_vTaxiwayItems.getAllNodes();}
	CString GetNodeName(int nNodeID){return m_vTaxiwayItems.GetNodeName(nNodeID);}

	//make a index start from 1 to mark node
	void InitTaxiNodeIndex(){m_vTaxiwayItems.InitTaxiNodeIndex();}
	int GetNodeIndex(int nNodeID){return m_vTaxiwayItems.GetNodeIndex(nNodeID);}


	int getItemCount(){return m_vTaxiwayItems.getItemCount();}
	CTaxiwayItem* getItem(int nIndex){return m_vTaxiwayItems.getItem(nIndex);}

	void AddItem(const CTaxiwayItem& taxiwayItem){m_vTaxiwayItems.AddItem(taxiwayItem);}

	void ClearTaxiwayItems(){m_vTaxiwayItems.ClearTaxiwayItems();}
public:
	virtual CString GetReportParamName();

	virtual void LoadParameter();
	virtual void UpdateParameter();
public:
	//write the parameter data
	virtual void WriteParameter(ArctermFile& _file);
	virtual void ReadParameter(ArctermFile& _file);


public:
	virtual BOOL ExportFile(ArctermFile& _file);
	virtual BOOL ImportFile(ArctermFile& _file);
public:
	bool IsNodeSelected(int nNodeID);

};
