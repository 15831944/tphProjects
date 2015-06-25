#include "StdAfx.h"
#include "TaxiwayUtilizationData.h"
#include "../Common/termfile.h"

//base class//////////////////////////////////////////////////////////////////////////////////
bool CTaxiwayUtilizationData::operator ==(const CTaxiwayUtilizationData& data)const
{
	if (m_sTaxiway == data.m_sTaxiway && m_sStartNode == data.m_sStartNode\
		&& m_sEndNode == data.m_sEndNode)
	{
		return true;
	}
	return false;
}

bool CTaxiwayUtilizationData::operator <(const CTaxiwayUtilizationData& data)const
{
	return (m_sTaxiway.CompareNoCase(data.m_sTaxiway) < 0);
}

BOOL CTaxiwayUtilizationData::ReadReportData(ArctermFile& _file)
{
	_file.getField(m_sTaxiway.GetBuffer(1024),1024);
	_file.getField(m_sStartNode.GetBuffer(1024),1024);
	_file.getField(m_sEndNode.GetBuffer(1024),1024);

	return TRUE;
}

BOOL CTaxiwayUtilizationData::WriteReportData(ArctermFile& _file)
{
	_file.writeField(m_sTaxiway.GetBuffer());
	_file.writeField(m_sStartNode.GetBuffer());
	_file.writeField(m_sEndNode.GetBuffer());

	return TRUE;
}

//detail data//////////////////////////////////////////////////////////////////////////////////
CTaxiwayUtilizationDetailData::CTaxiwayUtilizationDetailData()
:m_intervalStart(0)
,m_intervalEnd(0)
,m_totalMovement(0)
,m_avaSpeed(0.0)
,m_minSpeed(0.0)
,m_maxSpeed(0.0)
,m_sigmaSpeed(0.0)
,m_lOccupiedTime(0)

{

}

CTaxiwayUtilizationDetailData::~CTaxiwayUtilizationDetailData()
{

}

BOOL CTaxiwayUtilizationDetailData::ReadReportData(ArctermFile& _file)
{
	CTaxiwayUtilizationData::ReadReportData(_file);
	
	_file.getInteger(m_intervalStart);
	_file.getInteger(m_intervalEnd);
	_file.getInteger(m_totalMovement);
	_file.getFloat(m_avaSpeed);
	_file.getFloat(m_minSpeed);
	_file.getFloat(m_maxSpeed);
	_file.getFloat(m_sigmaSpeed);
	_file.getInteger(m_lOccupiedTime);
	
	int nCount = 0;
	_file.getInteger(nCount);
	_file.getLine();


	for (int i = 0; i < nCount; i++)
	{
		TaxiwayUtilizationFlightInfo info;
		_file.getField(info.m_sAirline.GetBuffer(),1024);
		_file.getField(info.m_sActype.GetBuffer(),1024);
		_file.getInteger(info.m_lMovements);
		_file.getFloat(info.m_avaSpeed);
		_file.getInteger(info.m_lOccupancy);

		m_vFlightInfo.push_back(info);
		_file.getLine();
	}	

	return TRUE;
}

BOOL CTaxiwayUtilizationDetailData::WriteReportData(ArctermFile& _file)
{	
	CTaxiwayUtilizationData::WriteReportData(_file);

	_file.writeInt(m_intervalStart);
	_file.writeInt(m_intervalEnd);
	_file.writeInt(m_totalMovement);
	_file.writeFloat((float)m_avaSpeed);
	_file.writeFloat((float)m_minSpeed);
	_file.writeFloat((float)m_maxSpeed);
	_file.writeFloat((float)m_sigmaSpeed);
	_file.writeInt(m_lOccupiedTime);
	
	_file.writeInt((int)m_vFlightInfo.size());
	_file.writeLine();

	for (int i = 0; i < (int)m_vFlightInfo.size(); i++)
	{
		_file.writeField(m_vFlightInfo[i].m_sAirline.GetBuffer());
		_file.writeField(m_vFlightInfo[i].m_sActype.GetBuffer());
		_file.writeInt(m_vFlightInfo[i].m_lMovements);
		_file.writeFloat((float)m_vFlightInfo[i].m_avaSpeed);
		_file.writeInt(m_vFlightInfo[i].m_lOccupancy);
		_file.writeLine();
	}	

	return TRUE;
}

TaxiwayUtilizationFlightInfo* CTaxiwayUtilizationDetailData::GetFlightInfo(const CString& sAirline,const CString& sActype)
{
	for (int i = 0; i < (int)m_vFlightInfo.size(); i++)
	{
		TaxiwayUtilizationFlightInfo& flightInfo = m_vFlightInfo[i];
		if (flightInfo.m_sAirline == sAirline && flightInfo.m_sActype == sActype)
		{
			return /*flightInfo*/&(m_vFlightInfo[i]);
		}
	}
	return NULL;
}
//summary data/////////////////////////////////////////////////////////////////////////////////
CTaxiwayUtilizationSummaryData::CTaxiwayUtilizationSummaryData()
{
	
}

CTaxiwayUtilizationSummaryData::~CTaxiwayUtilizationSummaryData()
{

}

BOOL CTaxiwayUtilizationSummaryData::ReadReportData(ArctermFile& _file)
{
	CTaxiwayUtilizationData::ReadReportData(_file);

	m_movementData.ReadReportData(_file);
	m_speedData.ReadReportData(_file);
	m_occupancyData.ReadReportData(_file);

	return TRUE;
}

BOOL CTaxiwayUtilizationSummaryData::WriteReportData(ArctermFile& _file)
{
	CTaxiwayUtilizationData::WriteReportData(_file);

	m_movementData.WriteReportData(_file);
	m_speedData.WriteReportData(_file);
	m_occupancyData.WriteReportData(_file);

	return TRUE;
}
//TaxiwayUtilzationSummaryData/////////////////////////////////////////////////////////////////
BOOL TaxiwayUtilzationSummaryData::ReadReportData(ArctermFile& _file)
{
	_file.getFloat(m_dTotalValue);
	_file.getFloat(m_dMinValue);
	_file.getFloat(m_dMaxValue);
	//_file.getInteger(m_lIntervalOfMin);
	_file.getInteger(m_lIntervalOfMax);
	_file.getFloat(m_dVerageValue);

	_file.getFloat(m_statisticalTool.m_dTotal);
	_file.getFloat(m_statisticalTool.m_dMin);
	_file.getFloat(m_statisticalTool.m_dAverage);
	_file.getFloat(m_statisticalTool.m_dMax);
	_file.getFloat(m_statisticalTool.m_dQ1);
	_file.getFloat(m_statisticalTool.m_dQ2);
	_file.getFloat(m_statisticalTool.m_dQ3);
	_file.getFloat(m_statisticalTool.m_dP1);
	_file.getFloat(m_statisticalTool.m_dP5);
	_file.getFloat(m_statisticalTool.m_dP10);
	_file.getFloat(m_statisticalTool.m_dP90);
	_file.getFloat(m_statisticalTool.m_dP95);
	_file.getFloat(m_statisticalTool.m_dP99);
	_file.getFloat(m_statisticalTool.m_dSigma);
	_file.getFloat(m_statisticalTool.m_dCount);
	return TRUE;
}

BOOL TaxiwayUtilzationSummaryData::WriteReportData(ArctermFile& _file)
{
	_file.writeFloat((float)m_dTotalValue);
	_file.writeFloat((float)m_dMinValue);
	_file.writeFloat((float)m_dMaxValue);
	//_file.writeInt(m_lIntervalOfMin);
	_file.writeInt(m_lIntervalOfMax);

	_file.writeFloat((float)m_dVerageValue);
	_file.writeFloat((float)m_statisticalTool.m_dTotal);
	_file.writeFloat((float)m_statisticalTool.m_dMin);
	_file.writeFloat((float)m_statisticalTool.m_dAverage);
	_file.writeFloat((float)m_statisticalTool.m_dMax);
	_file.writeFloat((float)m_statisticalTool.m_dQ1);
	_file.writeFloat((float)m_statisticalTool.m_dQ2);
	_file.writeFloat((float)m_statisticalTool.m_dQ3);
	_file.writeFloat((float)m_statisticalTool.m_dP1);
	_file.writeFloat((float)m_statisticalTool.m_dP5);
	_file.writeFloat((float)m_statisticalTool.m_dP10);
	_file.writeFloat((float)m_statisticalTool.m_dP90);
	_file.writeFloat((float)m_statisticalTool.m_dP95);
	_file.writeFloat((float)m_statisticalTool.m_dP99);
	_file.writeFloat((float)m_statisticalTool.m_dSigma);
	_file.writeFloat((float)m_statisticalTool.m_dCount);
	return TRUE;
}