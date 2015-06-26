#pragma once
#include "Common/ALTObjectID.h"
#include "Common/DBData.h"
//#include "InputAirsideDll.h"
#include "InputAirsideAPI.h"

class CADORecordset;

class INPUTAIRSIDE_API PaxBusParkingPlace : public CDBData
{
public:
	enum PakingPlaceGate
	{
		DepGate,
		ArrGate,
	};

	enum ParkingRelateType
	{
		OneToOne_Type,
		Random_Type
	};
	PaxBusParkingPlace();
	PaxBusParkingPlace(const PaxBusParkingPlace& parkingPlace);
	~PaxBusParkingPlace();

	void SetGate(const CString& strGate);
	const CString& GetGate();
	
	void SetTerminalSpot(const ALTObjectID& terminalSpot);
	const ALTObjectID& GetTerminalSpot()const;

	void SetGateType(PakingPlaceGate emGate);
	PakingPlaceGate GetGateType()const;

	void SetParkingRelate(ParkingRelateType emRelate);
	ParkingRelateType GetParkingRelate()const;


	const PaxBusParkingPlace& operator = (const PaxBusParkingPlace& parkingPlace);

	virtual CString GetTableName()const;
	virtual void GetDataFromDBRecordset(CADORecordset& recordset);
	virtual bool GetInsertSQL(int nParentID,CString& strSQL) const;
	virtual bool GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;

private:
	CString m_strGate;
	ALTObjectID m_terminalSpot;
	PakingPlaceGate m_enumGateType;
	ParkingRelateType m_emRelate;
};


class INPUTAIRSIDE_API PaxBusParkingPlaceList : public CDBDataCollection<PaxBusParkingPlace>
{
public:
	PaxBusParkingPlaceList(void);
	~PaxBusParkingPlaceList(void);

	virtual void GetSelectElementSQL(int nParentID,CString& strSQL) const;
};
