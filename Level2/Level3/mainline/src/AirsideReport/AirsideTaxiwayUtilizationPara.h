#pragma once
#include "Parameters.h"
#include "../Common/termfile.h"
#include "TaxiwayItem.h"
#include "TaxiwayUtilizationData.h"
class AIRSIDEREPORT_API CAirsideTaxiwayUtilizationPara : public CParameters
{
public:
	CAirsideTaxiwayUtilizationPara();
	virtual ~CAirsideTaxiwayUtilizationPara();

public:
	virtual void LoadParameter();
	virtual void UpdateParameter();
	virtual CString GetReportParamName();

	virtual BOOL ExportFile(ArctermFile& _file);
	virtual BOOL ImportFile(ArctermFile& _file);

	void setSubType(int nSubType){m_nSubType = nSubType ;}
	int getSubType(){ return m_nSubType;}

	void SetCurrentSelect(CTaxiwayUtilizationData* pSelect){m_pCurrentSelectTaxiway = pSelect;}
	CTaxiwayUtilizationData* GetCurrentSelect(){return m_pCurrentSelectTaxiway;}

	std::vector<CTaxiwayItem::CTaxiwayNodeItem > getAllNodes(){return m_vTaxiwayItems.getAllNodes();}
	CString GetNodeName(int nNodeID){return m_vTaxiwayItems.GetNodeName(nNodeID);}

	//make a index start from 1 to mark node
	void InitTaxiNodeIndex(){m_vTaxiwayItems.InitTaxiNodeIndex();}
	int GetNodeIndex(int nNodeID){return m_vTaxiwayItems.GetNodeIndex(nNodeID);}

	bool IsNodeSelected(int nNodeID){return m_vTaxiwayItems.IsNodeSelected(nNodeID);}


	int getItemCount(){return m_vTaxiwayItems.getItemCount();}
	CTaxiwayItem* getItem(int nIndex){return m_vTaxiwayItems.getItem(nIndex);}

	void AddItem(const CTaxiwayItem& taxiwayItem){m_vTaxiwayItems.AddItem(taxiwayItem);}

	void ClearTaxiwayItems(){m_vTaxiwayItems.ClearTaxiwayItems();}

	CTaxiwayItemList& GetResultTaxiwayItemList() {return m_vTaxiwayForResultItem;}
	bool GetUseAllTaxiway() const { return m_vTaxiwayItems.GetUseAllTaxiway(); }
	void SetUseAllTaxiway(bool val) { m_vTaxiwayItems.SetUseAllTaxiway(val); }

protected:
	virtual void WriteParameter(ArctermFile& _file);
	virtual void ReadParameter(ArctermFile& _file);

protected:
	int   m_nSubType;
	CTaxiwayItemList m_vTaxiwayItems;
	CTaxiwayItemList m_vTaxiwayForResultItem;
	CTaxiwayUtilizationData* m_pCurrentSelectTaxiway;
};