#pragma once
#include "Common/ALTObjectID.h"
#include "Common/DBData.h"
#include "InputAirsideAPI.h"

class CADORecordset;
class INPUTAIRSIDE_API BaggageParkingPlace : public CDBData
{
public:
	BaggageParkingPlace();
	BaggageParkingPlace(const BaggageParkingPlace& parkingPlace);
	~BaggageParkingPlace();

	enum ParkingRelateType
	{
		OneToOne_Type,
		Random_Type
	};
	
	void SetParkingRelate(ParkingRelateType emRelate);
	ParkingRelateType GetParkingRelate()const;

	void SetParkingProcessor(const CString& strProc);
	const CString& GetParkingProcessor()const;
	
	void SetBagTrainSpot(const ALTObjectID& trainSpot);
	const ALTObjectID& GetBagTrainSpot()const;

	virtual CString GetTableName()const;
	virtual void GetDataFromDBRecordset(CADORecordset& recordset);
	virtual bool GetInsertSQL(int nParentID,CString& strSQL) const;
	virtual bool GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;

	const BaggageParkingPlace& operator = (const BaggageParkingPlace& parkingPlace );
private:
	ParkingRelateType m_emRelate;
	CString m_strBaggageParking;
	ALTObjectID m_terminalSpot;
};

class INPUTAIRSIDE_API BaggageCartParkingLinkage : public CDBDataCollection<BaggageParkingPlace>
{
public:
	BaggageCartParkingLinkage(void);
	~BaggageCartParkingLinkage(void);

	virtual void GetSelectElementSQL(int nParentID,CString& strSQL) const;
};
