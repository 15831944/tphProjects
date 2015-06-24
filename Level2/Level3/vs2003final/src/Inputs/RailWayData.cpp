// RailWayData.cpp: implementation of the CRailWayData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RailWayData.h"
#include <algorithm>
#include "../Common/EchoSystem.h"
using namespace std;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRailWayData::CRailWayData():DataSet(RailwayDataFile)
{

}

CRailWayData::~CRailWayData()
{

}

void CRailWayData::readData (ArctermFile& p_file)
{


    p_file.getLine();
	m_allRailWayFromFile.clear();
		
	int iRailWayCount=0;
	p_file.getInteger(iRailWayCount);

	for(int i=0;i<iRailWayCount;++i)
	{
		RailwayInfo* pRailWay=new RailwayInfo();
		pRailWay->ReadDataFromFile(p_file);
		m_allRailWayFromFile.push_back(pRailWay);
	}
	
}

void CRailWayData::writeData (ArctermFile& p_file) const
{
	p_file.writeInt((int)m_allRailWayFromFile.size());
	p_file.writeLine();

	vector<RailwayInfo*>::const_iterator iter=m_allRailWayFromFile.begin();
	vector<RailwayInfo*>::const_iterator iterLast=m_allRailWayFromFile.end();

	for(;iter!=iterLast;++iter)
	{
		(*iter)->SaveDataToFile(p_file);
	}
	
	//if (ECHOLOG->IsEnable() && ECHOLOG->IsLogEnable(RailWayLog))
	{
		CString strLog = _T("");
		strLog = _T("Railway Data have wrote to file;");
		ECHOLOG->Log(RailWayLog,strLog);
	}
}

void CRailWayData::clear (void)
{
	m_allRailWayFromFile.clear();
}

void CRailWayData::SetAllRailWayInfoVector(const std::vector<RailwayInfo*>& _railWayVector)
{
	m_allRailWayFromFile.clear();
	copy(_railWayVector.begin(),_railWayVector.end(),back_inserter(m_allRailWayFromFile));

	//if (ECHOLOG->IsEnable() && ECHOLOG->IsLogEnable(RailWayLog))
	{
		CString strLog = _T("");
		strLog = _T("Railway Data have modified;");
		ECHOLOG->Log(RailWayLog,strLog);
	}
}

RailwayInfo* CRailWayData::FindRailWayPointer( const RailwayInfoKey& _anotherRailWayKey )
{
	vector<RailwayInfo*>::const_iterator iter=m_allRailWayFromFile.begin();
	vector<RailwayInfo*>::const_iterator iterLast=m_allRailWayFromFile.end();

	for(;iter!=iterLast;++iter)
	{
		/*
		if(   (*iter)->GetFirstStationIndex() == _anotherRailWay.GetFirstStationIndex()
			&& (*iter)->GetSecondStationIndex() == _anotherRailWay.GetSecondStationIndex()
			&& (*iter)->GetTypeFromFirstViewPoint() == _anotherRailWay.GetTypeFromFirstViewPoint()
			)
		*/
		if( (*iter)->GetRailWayKey() == _anotherRailWayKey )
			return *iter;
	}

	return NULL;
}