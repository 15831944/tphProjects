#pragma once
#include "altobject.h"

#include "..\Common\path2008.h"
#include "InputAirsideAPI.h"

enum ParkingType
{ 
	NOSEIN = 0,
	LINE 
};

class ARCVector3;
class INPUTAIRSIDE_API ParkingPlace: public ALTObject
{
public:
	typedef ref_ptr<ParkingPlace> RefPtr;

public:
	ParkingPlace(int nParentID);
	~ParkingPlace(void);


	virtual ALTOBJECT_TYPE GetType()const { return ALT_PARKINGPLACE; }
	virtual CString GetTypeName()const  { return "Parking place"; }


	const CPath2008& GetSpotLine(void)const;
	void SetSpotLine(const CPath2008& _path);

	DistanceUnit GetSpotWidth(void);
	void SetSpotWidth(DistanceUnit dSpotWidth);

	DistanceUnit GetSpotLength(void);
	void SetSpotLength(DistanceUnit dSpotLength);

	int GetSpotAngle();
	void SetSpotAngle(int nSpotAngle);

	ParkingType GetParingType();
	void SetParkingType(ParkingType _type);

	// get a copy of this object , change its name in sequence
	virtual ALTObject * NewCopy();

	void OnRotate(DistanceUnit dx, const ARCVector3& vCenter);
	void DoOffset(DistanceUnit dx, DistanceUnit dy, DistanceUnit dz);

	virtual bool CopyData(const ALTObject* pObj);

private:
	int m_nPathID;
	CPath2008 m_SpotLine;
	DistanceUnit m_dSpotLength;
	DistanceUnit m_dSpotWidth;
	int m_nSpotAngle;
	ParkingType m_enumParkingType;
	int m_nParentID;

	//database operation
public:

	void ReadObject(int nObjID);
	int SaveObject(int nAirportID);
	virtual void UpdateObject(int nObjID);
	virtual void DeleteObject(int nObjID);

protected:
	virtual CString GetSelectScript(int nObjID) const;
	virtual CString GetInsertScript(int nObjID) const;
	virtual CString GetUpdateScript(int nObjID) const;
	virtual CString GetDeleteScript(int nObjID) const;

};

class INPUTAIRSIDE_API ParkingPlaceList
{
public:
	ParkingPlaceList(int nParentID);
	~ParkingPlaceList();

	void AddParkingPlace(ParkingPlace* pParkingPlace);
	int GetParkingPlaceCount();
	ParkingPlace* GetParkingPlace(int nIdx);
	void DelParkingPlace(ParkingPlace* pParkingPlace);

	void ReadParkingPlaceList();
	void SaveParkingPlaceList(int nAirportID);
	void DeleteParkingPlaceList();

	void CopyData(ParkingPlaceList& pOtherParkingList);
	void ClearList();

private:
	std::vector<ParkingPlace*> m_vParkingPlaceList;
	std::vector<ParkingPlace*> m_vDelParkingPlaceList;
	int m_nParentID;
};