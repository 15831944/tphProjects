#pragma once
#include "../Common/FLT_CNST.H"
#include "FlightTypeDimensions.h"
#include "FlightTypeServiceLoactions.h"
#include "RingRoad.h"
#include "InputAirsideAPI.h"

class CAirportDatabase;
class CAirsideExportFile;
class CAirsideImportFile;
class INPUTAIRSIDE_API FlightTypeDetailsItem
{
public:
	FlightTypeDetailsItem(CAirportDatabase* pAirportDB, int nProjID);
	virtual ~FlightTypeDetailsItem(void);

	bool operator < (FlightTypeDetailsItem& rhs) const
	{
		return m_fltType < rhs.GetFltType();
	}

public:

	FlightTypeDimensions&		GetDimensions();
	FlightTypeServiceLoactions& GetRelativeLocations();
	FlightConstraint			GetFlightConstraint();
	RingRoad&					GetRingRoad();
	CPath2008						GetRingRoadPath();

	FlightConstraint			GetFltType();
	void						SetFltType(FlightConstraint fltCons);

	void						InitDefault();

public:
	int							GetID();
	void						ReadData(CADORecordset& adoRecordset);
	void						SaveData();
	void						UpdateData();
	void						DeleteData();

	void						SaveImportData(CString& strFltType);
	void						ExportData(CAirsideExportFile& exportFile);
	void						ImportData(CAirsideImportFile& importFile);

protected:
	int									m_nID;
	int									m_nProjID;
	FlightConstraint					m_fltType;
	CAirportDatabase					*m_pAirportDB;

	FlightTypeDimensions				m_dimensions;
	FlightTypeServiceLoactions			m_serviceLocations;
	RingRoad							m_ringRoad;
};

