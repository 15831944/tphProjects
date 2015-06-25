#pragma once

#include <common/FileInDB.h>
#include <common\elaptime.h>

class CAircraftModelDatabase;
class CAirportDatabase;
class CADORecordset;
class AirportDatabaseConnection;

class ModelPro : public CModel
{
public:
	ModelPro();
	void DBReadData(CADORecordset& recordset);
	virtual DBModelType GetDBModelType()const {return Aircraft_Model;}

protected:
	void DBSaveData(AirportDatabaseConnection* pConn);
	void DBDeleteData(AirportDatabaseConnection* pConn);

};
class CACType;
class CAircraftModel : public ModelPro
{
public:
	CAircraftModel(CAircraftModelDatabase* pDB);
	void InitDefault();
	AirportDatabaseConnection* GetConnection();

	CAircraftModelDatabase* mpParent;
	virtual void SetDimension(const ARCVector3& dim);

	void SaveDataToDB();
	void SaveData();
	void DeleteData();
	void ReadData();

	int GetCurrentVersion()const;
	const CString GetFileName()const;
	const CString GetFileExtnsion()const;

	CACType* GetACType()const{ return mpRelateACType; };
	void SetACType(CACType* pAC){ mpRelateACType = pAC; }
protected:
	CACType* mpRelateACType;

};

