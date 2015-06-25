#include "StdAfx.h"
#include "AirsideRunwayCrossingsPara.h"
#include "../Results/AirsideFlightEventLog.h"

AirsideRunwayCrossingsParameter::AirsideRunwayCrossingsParameter()
{

}

AirsideRunwayCrossingsParameter::~AirsideRunwayCrossingsParameter()
{

}

CString AirsideRunwayCrossingsParameter::GetReportParamName()
{
	return _T("RunwayCrossings\\RunwayCrossings.prm");
}

void AirsideRunwayCrossingsParameter::LoadParameter()
{

}

void AirsideRunwayCrossingsParameter::UpdateParameter()
{

}
BOOL AirsideRunwayCrossingsParameter::ExportFile(ArctermFile& _file)
{
	if(!CParameters::ExportFile(_file))
		return FALSE;
	int nCount = (int)m_vRunwayCrossings.size();
	_file.writeInt(nCount);
	_file.writeLine();
	for (int nItem = 0; nItem < nCount; ++nItem)
	{
		m_vRunwayCrossings[nItem].WriteParameter(_file);
	}
	_file.writeLine();
	return TRUE;
}

BOOL AirsideRunwayCrossingsParameter::ImportFile(ArctermFile& _file)
{
	if(!CParameters::ImportFile(_file))
		return FALSE;

	_file.getLine();
	int nCount = 0;
	_file.getInteger(nCount);
	_file.getLine();

	for (int nItem = 0; nItem < nCount; ++nItem)
	{
		RunwayCrossingsItem runwayParam;
		runwayParam.ReadParameter(_file);
		m_vRunwayCrossings.push_back(runwayParam);
	}
	return TRUE;
}

void AirsideRunwayCrossingsParameter::ReadParameter(ArctermFile& _file)
{
	CParameters::ReadParameter(_file);

	int nCount = 0;
	_file.getInteger(nCount);
	_file.getLine();

	for (int nItem = 0; nItem < nCount; ++nItem)
	{
		RunwayCrossingsItem runwayParam;
		runwayParam.ReadParameter(_file);
		_file.getLine();
		m_vRunwayCrossings.push_back(runwayParam);
	}
}

void AirsideRunwayCrossingsParameter::WriteParameter(ArctermFile& _file)
{
	CParameters::WriteParameter(_file);

	int nCount = (int)m_vRunwayCrossings.size();
	_file.writeInt(nCount);
	_file.writeLine();
	for (int nItem = 0; nItem < nCount; ++nItem)
	{
		m_vRunwayCrossings[nItem].WriteParameter(_file);
		_file.writeLine();
	}
}

int AirsideRunwayCrossingsParameter::getCount()const
{
	return (int)m_vRunwayCrossings.size();
}

AirsideRunwayCrossingsParameter::RunwayCrossingsItem& AirsideRunwayCrossingsParameter::getItem(int nIdx)
{
	ASSERT(nIdx >= 0 && nIdx < getCount());
	return m_vRunwayCrossings[nIdx];
}

void AirsideRunwayCrossingsParameter::addItem(const RunwayCrossingsItem& item)
{
	m_vRunwayCrossings.push_back(item);
}

void AirsideRunwayCrossingsParameter::deleteItem(int nIdx)
{
	ASSERT(nIdx >= 0 && nIdx < getCount());
	m_vRunwayCrossings.erase(m_vRunwayCrossings.begin() + nIdx);
}

void AirsideRunwayCrossingsParameter::clear()
{
	m_vRunwayCrossings.clear();
}

bool AirsideRunwayCrossingsParameter::fit(AirsideRunwayCrossigsLog* pLog)
{
	ElapsedTime time;
	time.setPrecisely(pLog->time);

	if (time < m_startTime || time > m_endTime)
	{
		return false;
	}

	for (int i = 0; i < getCount(); i++)
	{
		if(m_vRunwayCrossings[i].fit(pLog))
			return true;
	}

	return false;
}
/////////////////////////////////////////////////////////////////////////////////////////////////
AirsideRunwayCrossingsParameter::RunwayCrossingsItem::RunwayCrossingsItem()
{

}

AirsideRunwayCrossingsParameter::RunwayCrossingsItem::~RunwayCrossingsItem()
{

}

int AirsideRunwayCrossingsParameter::RunwayCrossingsItem::getCount()const
{
	return (int)m_vTaxiway.size();
}

void AirsideRunwayCrossingsParameter::RunwayCrossingsItem::addItem(const ALTObjectID& item)
{
	m_vTaxiway.push_back(item);
}

void AirsideRunwayCrossingsParameter::RunwayCrossingsItem::deleteItem(int nIdx)
{
	ASSERT(nIdx >= 0 && nIdx < getCount());
	m_vTaxiway.erase(m_vTaxiway.begin() + nIdx);
}


ALTObjectID& AirsideRunwayCrossingsParameter::RunwayCrossingsItem::getItem(int nIdx)
{
	ASSERT(nIdx >= 0 && nIdx < getCount());
	return m_vTaxiway[nIdx];
}


void AirsideRunwayCrossingsParameter::RunwayCrossingsItem::setRunway(const ALTObjectID& strObjName)
{
	m_RunwayItem = strObjName;
}

ALTObjectID& AirsideRunwayCrossingsParameter::RunwayCrossingsItem::getRunway()
{
	return m_RunwayItem;
}

void AirsideRunwayCrossingsParameter::RunwayCrossingsItem::ReadParameter(ArctermFile& _file)
{
	m_RunwayItem.readALTObjectID(_file);
	_file.getLine();
	int nCount = 0;
	_file.getInteger(nCount);
	_file.getLine();
	for (int nItem = 0; nItem < nCount; ++nItem)
	{
		ALTObjectID mapItem;
		mapItem.readALTObjectID(_file);
		m_vTaxiway.push_back(mapItem);
		_file.getLine();
	}
}

void AirsideRunwayCrossingsParameter::RunwayCrossingsItem::WriteParameter(ArctermFile& _file)
{
	m_RunwayItem.writeALTObjectID(_file);
	_file.writeLine();
	int nCount = (int)m_vTaxiway.size();
	_file.writeInt(nCount);
	_file.writeLine();
	for (int nItem = 0; nItem < nCount; ++nItem)
	{
		_file.writeField(m_vTaxiway[nItem].GetIDString().GetBuffer());
		_file.writeLine();
	}
}

bool AirsideRunwayCrossingsParameter::RunwayCrossingsItem::fit(AirsideRunwayCrossigsLog* pLog)
{
	ALTObjectID runwayID(pLog->sRunway.c_str());
	if(!runwayID.idFits(m_RunwayItem) && m_RunwayItem.GetIDString().CompareNoCase(_T("ALL")))
		return false;

	for (int i = 0; i < getCount(); i++)
	{
		ALTObjectID taxiwayID(pLog->sTaxiway.c_str());
		if(!taxiwayID.idFits(m_vTaxiway[i]) && m_vTaxiway[i].GetIDString().CompareNoCase(_T("ALL")))
			return false;
	}
	return true;
}