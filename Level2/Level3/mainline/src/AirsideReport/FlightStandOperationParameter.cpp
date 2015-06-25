#include "StdAfx.h"
#include "FlightStandOperationParameter.h"
#include <algorithm>

CFlightStandOperationParameter::CFlightStandOperationParameter()
:m_nSubType(0)
{

}

CFlightStandOperationParameter::~CFlightStandOperationParameter()
{

}

void CFlightStandOperationParameter::LoadParameter()
{

}

void CFlightStandOperationParameter::UpdateParameter()
{

}

CString CFlightStandOperationParameter::GetReportParamName()
{
	return _T("StandOperations\\StandOperations.prm");
}

BOOL CFlightStandOperationParameter::ImportFile(ArctermFile& _file)
{
	if(!CParameters::ImportFile(_file))
		return FALSE;

	int nCount = 0;
	_file.getInteger(nCount);
	_file.getLine();

	for (int nItem = 0; nItem < nCount; ++nItem)
	{
		ALTObjectID objID;
		objID.readALTObjectID(_file);
		m_vStandIDList.push_back(objID);
		_file.getLine();
	}
	return TRUE;
}

BOOL CFlightStandOperationParameter::ExportFile(ArctermFile& _file)
{
	if(!CParameters::ExportFile(_file))
		return FALSE;
	int nCount = (int)m_vStandIDList.size();
	_file.writeInt(nCount);
	_file.writeLine();
	for (int nItem = 0; nItem < nCount; ++nItem)
	{
		m_vStandIDList[nItem].writeALTObjectID(_file);
		_file.writeLine();
	}
	return TRUE;
}

void CFlightStandOperationParameter::ReadParameter(ArctermFile& _file)
{
	CParameters::ReadParameter(_file);

	int nCount = 0;
	_file.getInteger(nCount);
	_file.getLine();

	for (int nItem = 0; nItem < nCount; ++nItem)
	{
		ALTObjectID objID;
		objID.readALTObjectID(_file);
		m_vStandIDList.push_back(objID);
		_file.getLine();
	}
}

void CFlightStandOperationParameter::WriteParameter(ArctermFile& _file)
{
	CParameters::WriteParameter(_file);

	int nCount = (int)m_vStandIDList.size();
	_file.writeInt(nCount);
	_file.writeLine();
	for (int nItem = 0; nItem < nCount; ++nItem)
	{
		m_vStandIDList[nItem].writeALTObjectID(_file);
		_file.writeLine();
	}
}

bool CFlightStandOperationParameter::fits(const ALTObjectID& objID)const
{
	for (int i = 0; i < getCount(); i++)
	{
		if(objID.idFits(m_vStandIDList[i]))
		{
			return true;
		}
	}
	return false;
}

int CFlightStandOperationParameter::getCount()const
{
	return (int)m_vStandIDList.size();
}

void CFlightStandOperationParameter::addItem(const ALTObjectID& objID)
{
	m_vStandIDList.push_back(objID);
}

const ALTObjectID& CFlightStandOperationParameter::getItem(int nIdx)
{
	ASSERT(nIdx >= 0 && nIdx < getCount());
	return m_vStandIDList.at(nIdx);
}

void CFlightStandOperationParameter::removeItem(const ALTObjectID& objID)
{
	std::vector<ALTObjectID>::iterator iter = std::find(m_vStandIDList.begin(),m_vStandIDList.end(),objID);
	if (iter != m_vStandIDList.end())
	{
		m_vStandIDList.erase(iter);
	}
}


void CFlightStandOperationParameter::clear()
{
	m_vStandIDList.clear();
}

void CFlightStandOperationParameter::removeItem(int nIdx)
{
	ASSERT(nIdx >= 0 && nIdx < getCount());
	m_vStandIDList.erase(m_vStandIDList.begin() + nIdx);
}