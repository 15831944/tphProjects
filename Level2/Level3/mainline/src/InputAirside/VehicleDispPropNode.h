
#ifndef __VEHICLEDISPPROPNODE_H__
#define __VEHICLEDISPPROPNODE_H__

#pragma once

#include "VehicleDispPropItem.h"

class INPUTAIRSIDE_API CVehicleDispPropNode : public DBElement
{
public:
	CVehicleDispPropNode(int nProjID);
	virtual ~CVehicleDispPropNode(void);

	CString GetVehicleNodeName(void);
	void SetVehicleNodeName(CString strName);
	CVehicleDispPropItem * GetVehicleDispProp(int nIndex);
	int GetVehicleDispPropCount(void);
	void AddVehicleDispPropItem(CVehicleDispPropItem * pVehicleDispPropItem);
	CVehicleDispPropItem *  DeleteVehicleDispPropItem(int nIndex);//function return the pointer not delete and don't do the DeleteData();
	int GetCulSelItem(void);
	void SetCulSelItem(int nCulSel);
	int GetCount(){return m_vrVehicleDispProp.size();}
public:
	virtual void SaveData(int nParentID);
	virtual void DeleteData(void);
	virtual void UpdateData(void);
	virtual void ReadData(int nID);
protected:
	virtual void InitFromDBRecordset(CADORecordset& adoRecordset);
	virtual void GetInsertSQL(int nParentID,CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;
	virtual void GetSelectSQL(int nID,CString& strSQL)const;

	void Clear(void);

public:
	virtual void ExportData(CAirsideExportFile& exportFile);
	virtual void ImportData(CAirsideImportFile& importFile);

protected:
	int m_nProjID;
	CString m_strVehicleNodeName;
	int m_nCulSelItem;
	std::vector<CVehicleDispPropItem *> m_vrVehicleDispProp;	
};

#endif //__VEHICLEDISPPROPNODE_H__
