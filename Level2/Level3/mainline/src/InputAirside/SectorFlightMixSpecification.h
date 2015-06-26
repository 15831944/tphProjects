#pragma once
#include "ApproachSeparationType.h"
#include <vector>
#include "InputAirsideAPI.h"

class CADORecordset;

class INPUTAIRSIDE_API ACTypeLimitation
{
public:
	ACTypeLimitation();
	~ACTypeLimitation();

	void ReadData(CADORecordset& adoRecordset);
	void SaveData(int nParentID);
	void UpdateData(int nParentID);
	void DeleteData();

	int GetID();
	FlightClassificationBasisType GetACCategory();
	int GetACTypeID();
	int GetMaxLimit();
	
	void SetACCategory(FlightClassificationBasisType type){ m_emAcCategory = type; }
	void SetACTypeID(int nID){ m_nACTypeID = nID; }
	void SetMaxLimit(int nValue){ m_nMaxLimit = nValue; }

private:
	int m_nId;
	int m_nACTypeID;
	FlightClassificationBasisType    m_emAcCategory;
	int m_nMaxLimit;
};

class INPUTAIRSIDE_API SectorAltitudeLimitation
{
public:
	SectorAltitudeLimitation();
	~SectorAltitudeLimitation();

	enum AltitudeType{ AnyAltitude = 0,SameAltitude  };

	void ReadData(CADORecordset& adoRecordset);
	void SaveData(int nParentID);
	void UpdateData(int nParentID);
	void DeleteData();

	int GetID();
	AltitudeType GetAltitudeType();
	int GetMaxLimit();
	int GetACTypeLimitCount();
	ACTypeLimitation* GetACTypeLimitByIdx(int nIdx);

	void AddACTypeLimit(ACTypeLimitation* pItem);
	void DelACTypeLimit(ACTypeLimitation* pItem);

	void SetAltitudeType(AltitudeType type){ m_enumAltitudeType = type; }
	void SetMaxLimit(int nValue){ m_nMaxLimit = nValue; }
private:
	int m_nID;
	AltitudeType m_enumAltitudeType;
	int m_nMaxLimit;
	std::vector<ACTypeLimitation*> m_vACTypeLimitList;
	std::vector<ACTypeLimitation*> m_vACTypeLimitDelList;

};

class INPUTAIRSIDE_API SectorLimitation
{
public:
	SectorLimitation();
	~SectorLimitation();

	void ReadData(CADORecordset& adoRecordset);
	void SaveData(int nPrjID);
	void UpdateData(int nPrjID);
	void DeleteData();

	int GetID();
	int GetSectorID();
	int GetAltitudeLimitCount();
	SectorAltitudeLimitation* GetAltitudeLimitByIdx(int nIdx);

	void AddAltitudeLimit(SectorAltitudeLimitation* pItem);
	void DelAltitudeLimit(SectorAltitudeLimitation* pItem);

	void SetSectorID(int nID){ m_nSectorID = nID; }
private:
	int m_nID;
	int m_nSectorID;
	std::vector<SectorAltitudeLimitation*> m_vAltitudeLimitList;
	std::vector<SectorAltitudeLimitation*> m_vAltitudeLimitDelList;
};


class INPUTAIRSIDE_API SectorFlightMixSpecification
{
public:
	SectorFlightMixSpecification(void);
	~SectorFlightMixSpecification(void);

	void ReadData(int nPrjID);
	void SaveData();
	void UpdateData();
	void DeleteData();

	int GetSectorLimitCount();
	SectorLimitation* GetSectorLimitByIdx(int nIdx);

	void AddSectorLimit(SectorLimitation* pItem);
	void DelSectorLimit(SectorLimitation* pItem);
private:
	int m_nPrjID;
	std::vector<SectorLimitation*> m_vSectorLimitList;
	std::vector<SectorLimitation*> m_vSectorLimitDelList;
};
