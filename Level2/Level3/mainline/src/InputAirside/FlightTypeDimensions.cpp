#include "StdAfx.h"
#include "FlightTypeDimensions.h"
#include "AirsideImportExportManager.h"
#include "AirsideImportExportManager.h"

FlightTypeDimensions::FlightTypeDimensions(void)
{
	m_dGridExtent = 50.0;
	m_dGridSpacing = 2.0;

	m_dMaxWingSpan = 34.10;
	m_dMaxWingThickness = 7;
	m_dMinWingThickness = 3;
	m_dWingSweep = 29.22;
	m_dMaxFuselageLength = 37.57;
	m_dMaxFuselageWidth = 4.50;
	m_dWingRootPosition = 0.375;
	m_dTailPlaneSpan = 16.00;
	m_dTailPlaneThickness = 2.0;
	m_dTailPlaneSweep = 34.99;
	m_dTailPlanePosition = 0.8;
}

FlightTypeDimensions::~FlightTypeDimensions(void)
{
}

void FlightTypeDimensions::SetValue(int nIndex, double dValue)
{
	switch(nIndex)
	{
	case 0:
		m_dGridExtent = dValue;
		break;
	case 1:
		m_dGridSpacing = dValue;
		break;
	case 2:
		m_dMaxWingSpan = dValue;
		break;
	case 3:
		m_dMaxWingThickness = dValue;
		break;
	case 4:
		m_dMinWingThickness = dValue;
	    break;
	case 5:
		m_dWingSweep = dValue;
	    break;
	case 6:
		m_dMaxFuselageLength = dValue;
		break;
	case 7:
		m_dMaxFuselageWidth = dValue;
		break;
	case 8:
		m_dWingRootPosition = dValue;
	    break;
	case 9:
		m_dTailPlaneSpan = dValue;
	    break;
	case 10:
		m_dTailPlaneThickness = dValue;
		break;
	case 11:
		m_dTailPlaneSweep = dValue;
		break;
	case 12:
		m_dTailPlanePosition = dValue;
		break;
	default:
	    break;
	}
}

double FlightTypeDimensions::GetValue(int nIndex)
{
	switch(nIndex)
	{
	case 0:
		return m_dGridExtent;
	case 1:
		return m_dGridSpacing;
	case 2:
		return m_dMaxWingSpan;
	case 3:
		return m_dMaxWingThickness;
	case 4:
		return m_dMinWingThickness;
	case 5:
		return m_dWingSweep;
	case 6:
		return m_dMaxFuselageLength;
	case 7:
		return m_dMaxFuselageWidth;
	case 8:
		return m_dWingRootPosition;
	case 9:
		return m_dTailPlaneSpan;
	case 10:
		return m_dTailPlaneThickness;
	case 11:
		return m_dTailPlaneSweep;
	case 12:
		return m_dTailPlanePosition;
	default:
		ASSERT(FALSE);
		break;
	}
	return 0.0;
}

void FlightTypeDimensions::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("GridExtent"),m_dGridExtent);
	adoRecordset.GetFieldValue(_T("GridSpacing"),m_dGridSpacing);
	adoRecordset.GetFieldValue(_T("MaxWingSpan"),m_dMaxWingSpan);
	adoRecordset.GetFieldValue(_T("MaxWingThickness"),m_dMaxWingThickness);
	adoRecordset.GetFieldValue(_T("MinWingThickness"),m_dMinWingThickness);
	adoRecordset.GetFieldValue(_T("WingSweep"),m_dWingSweep);
	adoRecordset.GetFieldValue(_T("MaxFuselageLength"),m_dMaxFuselageLength);
	adoRecordset.GetFieldValue(_T("MaxFuselageWidth"),m_dMaxFuselageWidth);
	adoRecordset.GetFieldValue(_T("WingRootPosition"),m_dWingRootPosition);
	adoRecordset.GetFieldValue(_T("TailPlaneSpan"),m_dTailPlaneSpan);
	adoRecordset.GetFieldValue(_T("TailPlaneThickness"),m_dTailPlaneThickness);
	adoRecordset.GetFieldValue(_T("TailPlaneSweep"),m_dTailPlaneSweep);
	adoRecordset.GetFieldValue(_T("TailPlanePosition"),m_dTailPlanePosition);
}

int FlightTypeDimensions::SaveData(int nProjID, CString strFltType)
{
	CString strSQL;

	strSQL.Format(_T("INSERT INTO FlightTypeDetails \
					 (ProjID, FlightType, GridExtent, GridSpacing, MaxWingSpan, MaxWingThickness, MinWingThickness, \
					 WingSweep, MaxFuselageLength, MaxFuselageWidth, WingRootPosition,TailPlaneSpan, TailPlaneThickness,TailPlaneSweep, TailPlanePosition) \
					 VALUES (%d,'%s',%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f)"),
					 nProjID,strFltType, m_dGridExtent,m_dGridSpacing,m_dMaxWingSpan,m_dMaxWingThickness,
					 m_dMinWingThickness,m_dWingSweep,m_dMaxFuselageLength,m_dMaxFuselageWidth,
					 m_dWingRootPosition,m_dTailPlaneSpan,m_dTailPlaneThickness,m_dTailPlaneSweep,
					 m_dTailPlanePosition);

	return CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

}

void FlightTypeDimensions::UpdateData(int nID, int nProjID, CString strFltType)
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE FlightTypeDetails\
					 SET ProjID =%d, FlightType='%s',\
					 GridExtent =%f, GridSpacing =%f, MaxWingSpan =%f, MaxWingThickness =%f, MinWingThickness =%f, \
					 WingSweep =%f, MaxFuselageLength =%f, MaxFuselageWidth =%f, WingRootPosition =%f,\
					 TailPlaneSpan =%f, TailPlaneThickness =%f, TailPlaneSweep =%f, TailPlanePosition =%f\
					 WHERE (ID = %d)"),
					 nProjID, strFltType, m_dGridExtent,m_dGridSpacing,m_dMaxWingSpan,m_dMaxWingThickness,
					 m_dMinWingThickness,m_dWingSweep,m_dMaxFuselageLength,m_dMaxFuselageWidth,
					 m_dWingRootPosition,m_dTailPlaneSpan,m_dTailPlaneThickness,m_dTailPlaneSweep,
					 m_dTailPlanePosition, nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void FlightTypeDimensions::DeleteData(int nID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM FlightTypeDetails\
					 WHERE (ID = %d)"),nID);
	
	CADODatabase::ExecuteSQLStatement(strSQL);
}


void FlightTypeDimensions::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeDouble(m_dGridExtent);
	exportFile.getFile().writeDouble(m_dGridSpacing);

	exportFile.getFile().writeDouble(m_dMaxWingSpan);
	exportFile.getFile().writeDouble(m_dMaxWingThickness);
	exportFile.getFile().writeDouble(m_dMinWingThickness);
	exportFile.getFile().writeDouble(m_dWingSweep);
	exportFile.getFile().writeDouble(m_dMaxFuselageLength);
	exportFile.getFile().writeDouble(m_dMaxFuselageWidth);
	exportFile.getFile().writeDouble(m_dWingRootPosition);
	exportFile.getFile().writeDouble(m_dTailPlaneSpan);
	exportFile.getFile().writeDouble(m_dTailPlaneThickness);
	exportFile.getFile().writeDouble(m_dTailPlaneSweep);
	exportFile.getFile().writeDouble(m_dTailPlanePosition);


}

void FlightTypeDimensions::ImportData(CAirsideImportFile& importFile)
{
	importFile.getFile().getFloat(m_dGridExtent);
	importFile.getFile().getFloat(m_dGridSpacing);

	importFile.getFile().getFloat(m_dMaxWingSpan);
	importFile.getFile().getFloat(m_dMaxWingThickness);
	importFile.getFile().getFloat(m_dMinWingThickness);
	importFile.getFile().getFloat(m_dWingSweep);
	importFile.getFile().getFloat(m_dMaxFuselageLength);
	importFile.getFile().getFloat(m_dMaxFuselageWidth);
	importFile.getFile().getFloat(m_dWingRootPosition);
	importFile.getFile().getFloat(m_dTailPlaneSpan);
	importFile.getFile().getFloat(m_dTailPlaneThickness);
	importFile.getFile().getFloat(m_dTailPlaneSweep);
	importFile.getFile().getFloat(m_dTailPlanePosition);

}







