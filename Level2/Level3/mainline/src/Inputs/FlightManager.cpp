#include "stdafx.h"
#include <vector>
#include "assert.h"
#include "flight.h"
#include "common\airportdatabase.h"
#include <algorithm>
#include "FlightManager.h"


static const CString _strFlightGroupName = "FlightGroup.frp";
//////////////////////////////////////////////////////////////////////////////////

FlightGroup::CFlightGroupFilter::CFlightGroupFilter()
{

}

FlightGroup::CFlightGroupFilter::~CFlightGroupFilter()
{


}
void FlightGroup::CFlightGroupFilter::SetFilter(const CString& strFilter)
{
	ParserFilter(strFilter);
}
CString FlightGroup::CFlightGroupFilter::GetFilter()
{
	return m_strAirlineFilter + _T("-") + m_strADIDFilter;
}
void FlightGroup::CFlightGroupFilter::ParserFilter(const CString&  strFormatFilter)
{
	CString strFilter = strFormatFilter;
	
	strFilter.Trim();
	int nSeparatorPos = strFilter.Find(_T("-"));
	if (nSeparatorPos > 0)
	{
		m_strAirlineFilter = strFilter.Left(nSeparatorPos);
		m_strADIDFilter = strFilter.Right(strFilter.GetLength() - nSeparatorPos -1);
	}
}
void FlightGroup::Clear()
{
	for (unsigned i=0;i<m_vFilter.size();i++)
	{
		delete m_vFilter[i];
	}
	m_vFilter.clear();

}
bool FlightGroup::CFlightGroupFilter::containFlt( const char* pstrAirline ,const char* pstrFltID )
{
	char strAirlineFilter[AIRLINE_LEN];
	strcpy( strAirlineFilter, m_strAirlineFilter.GetBuffer() );
	
	char strFltIDFilter[FLIGHT_LEN];
	strcpy( strFltIDFilter, m_strADIDFilter.GetBuffer() );


	if(Filter(pstrAirline,strAirlineFilter) && Filter(pstrFltID,strFltIDFilter))
		return true;

	
	return false;
}

bool FlightGroup::CFlightGroupFilter::Filter( const char* pstrData ,char* pstrFilter )
{

	// replace wildcards
	for (int j = 0; pstrFilter[j] && pstrData[j]; j++)
	{
		if (pstrFilter[j] == '?')
			pstrFilter[j] = pstrData[j];
		else if (pstrFilter[j] == '*')
			strcpy (pstrFilter + j, pstrData + j);
	}
	if( !strcmpi (pstrData, pstrFilter) )
		return true;

	return false;
}
bool FlightGroup::containFlt( Flight* pFlight, bool bArrival )
{
	//airline
	CString strAirline;
	pFlight->getAirline(strAirline.GetBuffer(1024));
	strAirline.ReleaseBuffer();
	CString strFlightID;
	if (bArrival)
	{
		pFlight->getArrID(strFlightID.GetBuffer(1024));
	}
	else
	{
		pFlight->getDepID(strFlightID.GetBuffer(1024));
	}
	
	strFlightID.ReleaseBuffer();
	return contain(strAirline,strFlightID);
	//flight id


}
/////class CFlightGroup//////////////////////////////////////////////////////////
FlightGroup::FlightGroup()
{
}

FlightGroup::~FlightGroup()
{
	for (unsigned i=0;i<m_vFilter.size();i++)
	{
		delete m_vFilter[i];
	}
	m_vFilter.clear();
}

void FlightGroup::AddFilter(CFlightGroupFilter* groupFilter)
{
	m_vFilter.push_back(groupFilter);
}

void FlightGroup::DelFilter(CFlightGroupFilter* groupFilter)
{
	for(size_t nFilter = 0; nFilter < m_vFilter.size();++nFilter)
	{
		if (m_vFilter[nFilter] == groupFilter)
		{
			delete m_vFilter[nFilter];
			m_vFilter.erase(m_vFilter.begin() + nFilter);
			break;
		}
	}
}
size_t FlightGroup::GetFilterCount()
{
	return m_vFilter.size();
}
FlightGroup::CFlightGroupFilter* FlightGroup::GetFilterByIndex(size_t nIndex)
{
	if (nIndex < m_vFilter.size())
	{
		return m_vFilter.at(nIndex);
	}

	return NULL;
}

bool FlightGroup::contain( const char* pstrAirline ,const char* pstrFltID )
{
	for (size_t nFilter = 0; nFilter < m_vFilter.size();++nFilter )
	{
		if (m_vFilter[nFilter]->containFlt(pstrAirline,pstrFltID))
			return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
/////class CFlightManager//////////////////////////////////////////////////////

FlightManager::FlightManager()
:m_vGroups(NULL)
{

}

FlightManager::~FlightManager()
{
	for (unsigned i=0;i<m_vGroups.size();i++)
	{
		delete m_vGroups[i];
	}
	m_vGroups.clear();
}
int FlightManager::findGroupItem(FlightGroup* pGroup)
{
	std::vector<FlightGroup*>::iterator it = m_vGroups.begin();
	int nIndex = 0;
	for (;it != m_vGroups.end();++it)
	{
		if (pGroup->getGroupName().CompareNoCase((*it)->getGroupName()))
		{
			return nIndex;
		}
		nIndex++;
	}
	return -1;
}

BOOL FlightManager::SaveGroupToFile()
{
	CFile* pFile = NULL;
	CString sFileName = m_strDBPath + "\\" + _strFlightGroupName;
	BOOL bRet = TRUE;

	//check readonly attribute.
	CFileStatus fStatus;
	if(CFile::GetStatus( sFileName, fStatus)==TRUE && (fStatus.m_attribute & 0x01))
	{
		if(AfxMessageBox( sFileName+ " is readonly, so that data can't be writen!\n Would you like make it writable?",MB_YESNO)==IDYES)
		{
			fStatus.m_attribute=0x00;//make file writable.
			CFile::SetStatus( sFileName, fStatus );
		}
	}

	try
	{
		pFile = new CFile(sFileName, CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive);
		CArchive ar(pFile, CArchive::store);
		ar.WriteString("Databases\\FLIGHGROUP.GRP,2.00\r\nGroups Database\r\nGroups,FlightIndex\r\n");
		for(unsigned i=0; i<m_vGroups.size(); i++)
		{
			FlightGroup* pCurGroup = m_vGroups[i];
			ar.WriteString(pCurGroup->getGroupName());
			
			//filter
			 size_t nFilterCount = pCurGroup->GetFilterCount();
			for (size_t nFilter = 0; nFilter < nFilterCount; ++nFilter)
			{
				 CString strFilter = pCurGroup->GetFilterByIndex(nFilter)->GetFilter();
				 ar.WriteString(_T(",") + strFilter);
			}

			ar.WriteString("\r\n");
		}
		CTime t = CTime::GetCurrentTime();
		ar.WriteString("\r\n" + t.Format("%m/%d/%y,%H:%M:%S"));
		ar.Close();
		delete pFile;
	}
	catch(CException* e)
	{
		AfxMessageBox("Error writing sectors databases file.", MB_ICONEXCLAMATION | MB_OK);
		e->Delete();
		if(pFile != NULL)
			delete pFile;
		bRet = FALSE;
	}
	return bRet;
}

BOOL FlightManager::LoadData(const CString& _strDBPath)
{
	m_strDBPath = _strDBPath;

	CFile* pFile = NULL;
	CString sFileName = _strDBPath + "\\" + _strFlightGroupName;
	BOOL bRet = TRUE;
	try
	{
		pFile = new CFile(sFileName, CFile::modeRead | CFile::shareDenyNone);
		CArchive ar(pFile, CArchive::load);
	//	char line[10240];
		std::string line;
		CString csLine;
		ar.ReadString( csLine );
		//read line 2
		ar.ReadString( csLine );
		csLine.MakeUpper();
//		strcpy( line, csLine );
		line = csLine;
//		if(stricmp(line, "GROUPS DATABASE") == 0)
		if (line == _T("GROUPS DATABASE"))
		{
			//skip the column names
			ar.ReadString( csLine );
			//read the values
			//read a line
			ar.ReadString( csLine );
			csLine.MakeUpper();
//			strcpy( line, csLine );
			line = csLine;
	//		while( *line != '\0')
			while(line.length() != 0)
			{
				
				CString strGroup = CString(csLine);
				strGroup.Trim();
				
			
				FlightGroup *pCurGroup = new FlightGroup();
				


				//parser group string
				int nCommaPos = -1;
				nCommaPos = strGroup.Find(_T(","));
				CString strGroupName = _T("");
				if (nCommaPos < 0)
					strGroupName = strGroup;
				else
				    strGroupName = strGroup.Left(nCommaPos);
				pCurGroup->setGroupName(strGroupName);
				
				strGroup = strGroup.Right(strGroup.GetLength() - nCommaPos);
				strGroup.TrimLeft(_T(","));
				while(( nCommaPos =strGroup.Find(_T(","))) >  0)
				{
					CString strFilter = strGroup.Left(nCommaPos);
					FlightGroup::CFlightGroupFilter *pFilter = new  FlightGroup::CFlightGroupFilter();
					if(strFilter.Find(_T("-")) >= 0)
					{
						pFilter->SetFilter(strFilter);
						pCurGroup->AddFilter(pFilter);	
					}
			
					strGroup = strGroup.Right(strGroup.GetLength() - nCommaPos);
					strGroup.TrimLeft(_T(","));

				}
				//the last one
				CString strFilter = strGroup;
				FlightGroup::CFlightGroupFilter *pFilter = new  FlightGroup::CFlightGroupFilter();
				if(strFilter.Find(_T("-")) >= 0)
				{			
					pFilter->SetFilter(strFilter);
					pCurGroup->AddFilter(pFilter);					
				}

				m_vGroups.push_back(pCurGroup);

				//CString sFlightGroupName;
				//CString sFlightIndex;
				//char* b = line;

				//CString sFlightID;
				//while(TRUE) 
				//{
				//	int nIdx = sFlightIndex.Find(',');
				//	if(nIdx != -1) 
				//	{ // found a comma
				//		sFlightID = sFlightIndex.Left(nIdx);
				//		ASSERT(0);
				//		AddFlightGroup(NULL);
				//		sFlightIndex = sFlightIndex.Right(sFlightIndex.GetLength()-nIdx-1);
				//	}
				//	else if(!sFlightIndex.IsEmpty())
				//	{ // no comma, but string not empty
				//		sFlightID = sFlightIndex;
				//		ASSERT(0);
				//		AddFlightGroup(NULL);
				//		break;
				//	}
				//	else { // no comma, string empty
				//		break;
				//	}
				//}
				ar.ReadString( csLine );
				csLine.MakeUpper();
//				strcpy( line, csLine );
				line = csLine;
			}
		}
		else
			bRet = FALSE;
		ar.Close();
		delete pFile;
	}
	catch(CException* e) {
//		AfxMessageBox("Error reading Sectors file: " + sFileName, MB_ICONEXCLAMATION | MB_OK);
		e->Delete();
		if(pFile != NULL)
			delete pFile;
		bRet = FALSE;
	}
	
	//test code
	//for (int i =0; i < (int)m_vGroups.size(); ++i)
	//{
	//	m_vGroups[i]->contain("ABC","1234");
	//	m_vGroups[i]->contain("AAC","1234");
	//	m_vGroups[i]->contain("BBC","234");
	//	m_vGroups[i]->contain("BBC","1234");
	//	m_vGroups[i]->contain("CAA","234");
	//	m_vGroups[i]->contain("CAA","1234");
	//	m_vGroups[i]->contain("ACAA","234");
	//	m_vGroups[i]->contain("ACAA","1234");

	//}


	return bRet;
}

void FlightManager::AddFlightGroup(FlightGroup* pGroup)
{
	m_vGroups.push_back(pGroup);
}

void FlightManager::DelItem(FlightGroup* pGroup)
{
	std::vector<FlightGroup*>::iterator it = m_vGroups.begin();
	for (;it != m_vGroups.end();++it)
	{
		if (pGroup->getGroupName().CompareNoCase((*it)->getGroupName())==0)
		{
			m_vGroups.erase(it);
			delete pGroup;
			break;
		}
	}
}


BOOL FlightManager::CheckFlightGroup(FlightGroup* pGroup)
{
	std::vector<FlightGroup*>::iterator it = m_vGroups.begin();
	for(; it != m_vGroups.end();++it)
	{
		if (pGroup->getGroupName().CompareNoCase((*it)->getGroupName()) == 0)
		{
			return TRUE;
		}
	}
	return FALSE;
}

int FlightManager::findFlightGroupItem(FlightGroup* pGroup)
{
	int nCount = m_vGroups.size();
	for( int i=0; i<nCount; i++ )
	{
		if( m_vGroups[i]->getGroupName() == pGroup->getGroupName() )
			return i;
	}
	return INT_MAX;
}
FlightManager* _g_GetActiveFlightMan(CAirportDatabase * pAirportDB)
{
	assert(pAirportDB);
	return pAirportDB->getFlightMan();
}
