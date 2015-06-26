#pragma once
#include "../Database/ADODatabase.h"

#include "InputAirsideAPI.h"

class CAirsideExportFile;
class CAirsideImportFile;

class INPUTAIRSIDE_API FlightTypeDimensions
{
public:
	FlightTypeDimensions(void);
	virtual ~FlightTypeDimensions(void);

public:
	void ReadData(CADORecordset& adoRecordset);
	int  SaveData(int nProjID, CString strFltType);
	void UpdateData(int nID, int nProjID, CString strFltType);
	void DeleteData(int nID);

	void SetValue(int nIndex, double dValue);
	double GetValue(int nIndex);

public:

	void SetGridExtent(double dVal)				{ m_dGridExtent = dVal;	}
	void SetGridSpacing(double dVal)			{ m_dGridSpacing = dVal;}

	void SetMaxWingSpan(double dVal)			{ m_dMaxWingSpan = dVal; }
	void SetMaxWingThickness(double dVal)		{ m_dMaxWingThickness = dVal; }
	void SetMinWingThickness(double dVal)		{ m_dMinWingThickness = dVal; }
	void SetWingSweep(double dVal)				{ m_dWingSweep = dVal; }
	void SetMaxFuselageLength(double dVal)		{ m_dMaxFuselageLength = dVal; }
	void SetMaxFuselageWidth(double dVal)		{ m_dMaxFuselageWidth = dVal; }
	void SetWingRootPosition(double dVal)		{ m_dWingRootPosition = dVal; }
	void SetTailPlaneSpan(double dVal)			{ m_dTailPlaneSpan = dVal; }
	void SetTailPlaneThickness(double dVal)		{ m_dTailPlaneThickness = dVal; }
	void SetTailPlaneSweep(double dVal)			{ m_dTailPlaneSweep = dVal; }
	void SetTailPlanePosition(double dVal)		{ m_dTailPlanePosition = dVal; }


	double GetGridExtent()				{ return m_dGridExtent;	}
	double GetGridSpacing()			    { return m_dGridSpacing;}

	double GetMaxWingSpan()			    { return m_dMaxWingSpan; }
	double GetMaxWingThickness()		{ return m_dMaxWingThickness; }
	double GetMinWingThickness()		{ return m_dMinWingThickness; }
	double GetWingSweep()				{ return m_dWingSweep; }
	double GetMaxFuselageLength()		{ return m_dMaxFuselageLength; }
	double GetMaxFuselageWidth()		{ return m_dMaxFuselageWidth; }
	double GetWingRootPosition()		{ return m_dWingRootPosition; }
	double GetTailPlaneSpan()			{ return m_dTailPlaneSpan; }
	double GetTailPlaneThickness()		{ return m_dTailPlaneThickness; }
	double GetTailPlaneSweep()			{ return m_dTailPlaneSweep; }
	double GetTailPlanePosition()		{ return m_dTailPlanePosition; }


	void   ExportData(CAirsideExportFile& exportFile);
	void   ImportData(CAirsideImportFile& importFile);

protected:
	double			m_dGridExtent;
	double			m_dGridSpacing;

	double			m_dMaxWingSpan;
	double			m_dMaxWingThickness;
	double			m_dMinWingThickness;
	double			m_dWingSweep;
	double			m_dMaxFuselageLength;
	double			m_dMaxFuselageWidth;
	double			m_dWingRootPosition;
	double			m_dTailPlaneSpan;
	double			m_dTailPlaneThickness;
	double			m_dTailPlaneSweep;
	double			m_dTailPlanePosition;
};
