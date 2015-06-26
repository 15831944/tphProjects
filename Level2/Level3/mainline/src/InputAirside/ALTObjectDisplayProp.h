#pragma once

#include "..\Common\Referenced.h"
#include "..\Common\ref_ptr.h"
#include "..\Common\ARCColor.h"
#include <string>
#include "ALTObject.h"
#include "InputAirsideAPI.h"

class CAirsideImportFile;
class CAirsideExportFile;

class DisplayProp
{
public:
	//std::string strName;
	int m_nID;	//the index in the database, 
			  	//if the id == -1,the properties is not valid, use the default settings
				//otherwise ...
	
	bool bDefault;
	bool bOn;
	ARCColor4 cColor;
	inline DisplayProp():bOn(true),cColor(ARCColor4::WHITE){ m_nID = -1; }
	inline DisplayProp(bool on, ARCColor4 color) :bDefault(true), bOn(on),cColor(color){m_nID = -1;}
	void ReadData(CADORecordset& adoRecordset);
	void ReadData();
	void SaveData();
	void UpdateData();
	void DeleteData();

	void CopyData(const DisplayProp& prop);

public:
	void ExportDisplayProp(CAirsideExportFile& exportFile);
	void ImportDisplayProp(CAirsideImportFile& importFile);
};

class INPUTAIRSIDE_API ALTObjectDisplayProp : public Referenced
{
public:
	typedef ref_ptr<ALTObjectDisplayProp> RefPtr ;
		

	virtual ~ALTObjectDisplayProp(void);


	int m_nID;
	DisplayProp m_dpName;	
	DisplayProp m_dpShape;

public:
	virtual ALTOBJECT_TYPE GetType();
	void ReadBaseRecord(CADORecordset& adoRecordset);
	void UpdateBaseRecord();
	void DeleteBaseRecord();
	void SaveBaseRecord();
	virtual void ReadData( int nObjectID);
	virtual void UpdateData(int nObjectID);
	virtual void DeleteData(int nObjectID);
	virtual void SaveData(int nObjectID);

	virtual void CopyData(const ALTObjectDisplayProp& prop);

public:
	void ExportDspProp(CAirsideExportFile& exportFile);
	void ImportDspProp(CAirsideImportFile& importFile,int nObjID);

	virtual void ExportOtherDspProp(CAirsideExportFile& exportFile);
	virtual void ImportOtherDspProp(CAirsideImportFile& importFile);

public:
	ALTObjectDisplayProp(void);
};


class INPUTAIRSIDE_API TaxiwayDisplayProp : public ALTObjectDisplayProp{

public:	
	TaxiwayDisplayProp();
	~TaxiwayDisplayProp();

public:
	virtual void ReadData( int nTaxiwayID);
	virtual void UpdateData(int nTaxiwayID);
	virtual void DeleteData(int nTaxiwayID);
	virtual void SaveData(int nTaxiwayID);

	virtual void CopyData(const TaxiwayDisplayProp& prop);



	DisplayProp m_dpDir;
	DisplayProp m_dpCenterLine;
	DisplayProp m_dpMarking;

public:
	void ExportOtherDspProp(CAirsideExportFile& exportFile);
	void ImportOtherDspProp(CAirsideImportFile& importFile);

};

class INPUTAIRSIDE_API RunwayDisplayProp : public ALTObjectDisplayProp
{
public : 
	RunwayDisplayProp();
	~RunwayDisplayProp();

public:
	virtual void ReadData( int nRunwayID);
	virtual void UpdateData(int nRunwayID);
	virtual void DeleteData(int nRunwayID);
	virtual void SaveData(int nRunwayID);
};


class INPUTAIRSIDE_API HeliportDisplayProp : public ALTObjectDisplayProp
{
//public:
//	HeliportDisplayProp();
//	~HeliportDisplayProp();
//public:
//	virtual void ReadData(int nHeliportID);
//	virtual void UpdateData(int nHeliportID);
//	virtual void DeleteData(int nHeliportID);
//	virtual void SaveData(int nHeliportID);
};

class INPUTAIRSIDE_API StandDisplayProp : public ALTObjectDisplayProp
{
public:
	
	DisplayProp m_dpInConstr;
	DisplayProp m_dpOutConstr;
	
	StandDisplayProp();

public:
	virtual void ReadData( int nStandID);
	virtual void UpdateData(int nStandID);
	virtual void DeleteData(int nStandID);
	virtual void SaveData(int nStandID);

	virtual void CopyData(const StandDisplayProp& prop);


public:
	void ExportOtherDspProp(CAirsideExportFile& exportFile);
	void ImportOtherDspProp(CAirsideImportFile& importFile);
};


class INPUTAIRSIDE_API ParkSpotDisplayProp : public ALTObjectDisplayProp
{
public:

	DisplayProp m_dpInConstr;
	DisplayProp m_dpOutConstr;

	ParkSpotDisplayProp();

public:
	virtual void ReadData( int nStandID);
	virtual void UpdateData(int nStandID);
	virtual void DeleteData(int nStandID);
	virtual void SaveData(int nStandID);

	virtual void CopyData(const ParkSpotDisplayProp& prop);

public:

};



class INPUTAIRSIDE_API DeicePadDisplayProp : public ALTObjectDisplayProp{
public:
	DeicePadDisplayProp();

	DisplayProp m_dpPad;
	DisplayProp m_dpTruck;
	DisplayProp m_dpInConstr;
	DisplayProp m_dpOutConstr;

public:
	virtual void ReadData( int nDeicePadID);
	virtual void UpdateData(int nDeicePadID);
	virtual void DeleteData(int nDeicePadID);
	virtual void SaveData(int nDeicePadID);

	virtual void CopyData(const DeicePadDisplayProp& prop);


public:
	void ExportOtherDspProp(CAirsideExportFile& exportFile);
	void ImportOtherDspProp(CAirsideImportFile& importFile);
};




class INPUTAIRSIDE_API AirRouteDisplayProp : public ALTObjectDisplayProp{
public:
	AirRouteDisplayProp();

public:
	virtual void ReadData( int nAirRouteID);
	virtual void UpdateData(int nAirRouteID);
	virtual void DeleteData(int nAirRouteID);
	virtual void SaveData(int nAirRouteID);


};

class INPUTAIRSIDE_API AirHoldDisplayProp : public ALTObjectDisplayProp{
public:
	AirHoldDisplayProp();


public:
	virtual void ReadData( int nAirHoldID);
	virtual void UpdateData(int nAirHoldID);
	virtual void DeleteData(int nAirHoldID);
	virtual void SaveData(int nAirHoldID);
};

class INPUTAIRSIDE_API AirWayPointDisplayProp : public ALTObjectDisplayProp{
public:
	AirWayPointDisplayProp();

public:
	virtual void ReadData( int nWayPointID);
	virtual void UpdateData(int nWayPointID);
	virtual void DeleteData(int nWayPointID);
	virtual void SaveData(int nWayPointID);
};

class INPUTAIRSIDE_API AirSectorDisplayProp : public ALTObjectDisplayProp{
public:
	AirSectorDisplayProp();

public:
	virtual void ReadData( int nWayPointID);
	virtual void UpdateData(int nWayPointID);
	virtual void DeleteData(int nWayPointID);
	virtual void SaveData(int nWayPointID);
};

class INPUTAIRSIDE_API SurfaceDisplayProp : public ALTObjectDisplayProp
{

};

class INPUTAIRSIDE_API ContourDisplayProp : public ALTObjectDisplayProp
{

};

class INPUTAIRSIDE_API ReportingAreaDisplayProp : public ALTObjectDisplayProp
{

public:
	virtual void ReadData( int nAreaID);
	virtual void UpdateData(int nAreaID);
	virtual void DeleteData(int nAreaID);
	virtual void SaveData(int nAreaID);
};

class INPUTAIRSIDE_API StructureDisplayProp : public ALTObjectDisplayProp
{
public:	
	virtual void ReadData( int nStructureID);
	virtual void UpdateData(int nStructureID);
	virtual void DeleteData(int nStructureID);
	virtual void SaveData(int nStructureID);

};

class INPUTAIRSIDE_API StretchDisplayProp : public ALTObjectDisplayProp
{
};
 
class INPUTAIRSIDE_API VehiclePoolParkingDisplayProp:public ALTObjectDisplayProp
{
};



class INPUTAIRSIDE_API TrafficLightDisplayProp:public ALTObjectDisplayProp
{
};

class INPUTAIRSIDE_API TollGateDisplayProp:public ALTObjectDisplayProp
{
};

class INPUTAIRSIDE_API StopSignDisplayProp:public ALTObjectDisplayProp
{
};

class INPUTAIRSIDE_API YieldSignDisplayProp:public ALTObjectDisplayProp
{
};
