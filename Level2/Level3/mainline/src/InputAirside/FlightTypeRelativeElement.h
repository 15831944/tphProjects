#pragma once
#include "../Common/Point2008.h"

#include "InputAirsideAPI.h"

class CAirsideExportFile;
class CAirsideImportFile;

class INPUTAIRSIDE_API FlightTypeRelativeElement
{
public:
	FlightTypeRelativeElement(void);
	virtual ~FlightTypeRelativeElement(void);


public:
	void  SetID(int nID);
	int   GetID();
	void  SetTypeID(int nTypeID);
	int   GetTypeID();

	void SetFlightTypeDetailsItemID(int nFltID);

	void  SetServiceLocation(CPoint2008& point);
	CPoint2008 GetServiceLocation();


	void ConvertPointIntoLocalCoordinate(const CPoint2008& pt,const double& dDegree = 0);


	void SaveData(int nFDID);
	void UpdateData(int nFDID);
	void DeleteData();

	void ExportData(CAirsideExportFile& exportFile, int nFltTypeDetailsItemID);
	void ImportData(CAirsideImportFile& importFile, int nFltTypeDetailsItemID);
protected:
	int				m_nID;
	int				m_nFlightTypeDetailsItemID;
	CPoint2008			m_location;
	int				m_nTypeID;
};


