#pragma once
#include "..\Database\DBElementCollection_Impl.h"
#include "InboundRouteItem.h"
#include "InputAirsideAPI.h"

class CAirsideImportFile;
class CAirsideExportFile;

typedef vector<CInboundRouteItem*> INBOUNDROUTEITEMLIST;
class INPUTAIRSIDE_API CInboundRouteHead : public DBElementCollection<CInboundRouteItem>
{
public:
	CInboundRouteHead(void);
	virtual ~CInboundRouteHead(void);

public:
	//DBElementCollection
	virtual void SaveData(int nInboundID);
	virtual void DeleteData();
	//DBElement
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	virtual void GetInsertSQL(int nInboundID, CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;

public:
	void GetFirstRouteItem(CInboundRouteItem **ppInboundRouteItem);
	void SetFirstRouteItem(CInboundRouteItem *pInboundRouteItem);
	void GetTailRouteItems(INBOUNDROUTEITEMLIST *pInboundRouteItemList);

	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);
	int  GetType() {return INBOUDROUTE_ITEM ;} ;

public:
	void DeleteDataFromDB();

private:
	CInboundRouteItem *m_pFirstRouteItem;
};
