// BagDistReport.cpp: implementation of the CBagDistReport class.
//
//////////////////////////////////////////////////////////////////////



#include "stdafx.h"
#include "Reports.h"
#include "BagDistReport.h"
#include "common\termfile.h"
#include "rep_pax.h"
#include "results\FltLog.h"
#include "engine\terminal.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBagDistReport::CBagDistReport(Terminal* _pTerm, const CString& _csProjPath)
	:CBaseReport(_pTerm, _csProjPath)
{

}

CBagDistReport::~CBagDistReport()
{

}
void CBagDistReport::InitParameter( const CReportParameter* _pPara )
{
	CBaseReport::InitParameter( _pPara );
	m_usesInterval = 1;
}
void CBagDistReport::GenerateDetailed(ArctermFile& p_file)
{
	p_file.writeField(" ");
	p_file.writeLine();

	ElapsedTime ActArrTime, ActDepTime;
	ElapsedTime SchArrTime, SchDepTime;

	FlightLogEntry fltEntry;
	MobLogEntry paxEntry;
//	PaxDescStruct paxDesc;
	paxEntry.SetOutputTerminal(m_pTerm);
	paxEntry.SetEventLog(m_pTerm->m_pMobEventLog);
	fltEntry.SetOutputTerminal(m_pTerm);
	fltEntry.SetEventLog(m_pTerm->m_pFltEventLog);
	long count = m_pTerm->paxLog->getCount();

	_mapCount *mapCount = new _mapCount[count];
	for(long i = 0; i < count; i++)
	{
		mapCount[i].arrCount = 0;
		mapCount[i].depCount = 0;
		mapCount[i].index = 0;
	}

	long flagNum = 0;
	int flag = 1;
	char str[250];
	
	for(i = 0; i < count; i++)
	{
		m_pTerm->paxLog->getItem(paxEntry, i);
		paxEntry.clearLog();
		if( paxEntry.GetMobileType() == 0 )// is pax
		{
			long arrIndex = paxEntry.getArrFlight();
			long depIndex = paxEntry.getDepFlight();
			long index = paxEntry.getFlightIndex();
			long bagNum = paxEntry.getBagCount();

			if(arrIndex != depIndex && arrIndex != -1 && depIndex != -1)
			{
				// TRACE("%ld\n",bagNum);
			}

			for(int j = 0; j < flagNum; j++)
			{
				if(
					mapCount[j].index == index
				)
				{
					if(paxEntry.isArriving())
					{
						mapCount[j].arrCount += bagNum;
					}
					if(paxEntry.isDeparting())
					{
						mapCount[j].depCount += bagNum;
					}
					flag = 0;
					break;
				}
				if(flagNum - j == 1) flag = 1;
			}

			if(flag)
			{
				mapCount[flagNum].index = index;
				if(paxEntry.isArriving())

				{
					mapCount[flagNum].arrCount = bagNum;
				}
				if(paxEntry.isDeparting())

				{
					mapCount[flagNum].depCount = bagNum;
				}
				flagNum++;
			}
		}
		
	}
	
	for(i = 0; i < flagNum; i++)
	{
		m_pTerm->flightLog->getItem(fltEntry, mapCount[i].index);
		if(fltEntry.isArriving() && !fltEntry.isDeparting())
		{
			fltEntry.getFlightID(str,'A');
			p_file.writeField(str);
			p_file.writeField("");
			p_file.writeTime(fltEntry.getArrTime());
			p_file.writeField("");
			p_file.writeInt(mapCount[i].arrCount);
			p_file.writeField("");
		}
		if(!fltEntry.isArriving() && fltEntry.isDeparting())
		{
			fltEntry.getFlightID(str,'D');
			p_file.writeField(" ");
			p_file.writeField(str);
			p_file.writeField("");
			p_file.writeTime(fltEntry.getDepTime());
			p_file.writeField("");
			p_file.writeInt(mapCount[i].depCount);
		}
		if(fltEntry.isArriving() && fltEntry.isDeparting())
		{
			fltEntry.getFlightID(str,'A');
			p_file.writeField(str);
			fltEntry.getFlightID(str,'D');
			p_file.writeField(str);
			p_file.writeTime(fltEntry.getArrTime());
			p_file.writeTime(fltEntry.getDepTime());
			p_file.writeInt(mapCount[i].arrCount);
			p_file.writeInt(mapCount[i].depCount);
		}
		p_file.writeLine();
	}
	delete[] mapCount;

}

void CBagDistReport::ReadSpecifications(const CString& _csProjPath)
{
	//general operations
    char specFileName[128];
    PROJMANAGER->getSpecFileName( _csProjPath, GetSpecFileType(), specFileName );

    if (PROJMANAGER->fileExists (specFileName))
    {
        ArctermFile specFile;
        specFile.openFile (specFileName, READ);
        specFile.getLine();
        specFile.getTime (m_startTime);
        specFile.getTime (m_endTime);
        if (!specFile.eol() && m_usesInterval)
            specFile.getTime (m_interval);

        specFile.getLine();
        if (!specFile.isBlank())
		{
			ASSERT(m_pTerm);
			m_multiConst.readConstraints (specFile,m_pTerm);
		}
	}
}