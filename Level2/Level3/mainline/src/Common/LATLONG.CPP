#include "StdAfx.h"
#include "latlong.h"
#include "../Common/termfile.h"

CLatLong::CLatLong(void)
{
	m_dir = "";
	m_nDeg=0;
	m_nMinutes=0;
	m_nSeconds=0;
    m_nDelta=0;
}

CLatLong::~CLatLong(void)
{


}
//init



void CLatLong::GetValue(CString &strValue,BOOL nFlags)
{	
	if(nFlags)
	{
		strValue.Format("%s %d:%d:%d",m_dir,m_nDeg,m_nMinutes,m_nSeconds);
	}
	else
	{
		strValue.Format("%s %d:%d:%d:%d",m_dir, m_nDeg,m_nMinutes,m_nSeconds,m_nDelta);
	}

}
void CLatLong::SetValue(CString strValue)
{
	strValue.TrimLeft();
	strValue.TrimRight();

	if (strlen(strValue))
	{
		CStringArray saArray;
		if(SplitString(strValue,strlen(strValue)," ",saArray)==2)
		{

			// N or s
			CString strValue=saArray.GetAt(0);
			strValue.TrimLeft();
			strValue.TrimRight();
			if (strValue.CompareNoCase("N")==0)
			{
				m_dir=LAT_NORTH;
			}
			else if (strValue.CompareNoCase("S")==0)
			{
				m_dir=LAT_SOUTH;
			}
			else if (strValue.CompareNoCase("E")==0)
			{
				m_dir=LONG_EAST;
			}else if(strValue.CompareNoCase("W")==0)
			{
				m_dir=LONG_WEST;
			}
			else
				return;

			strValue=saArray.GetAt(1);
			saArray.RemoveAll();
			if(SplitString(strValue,strlen(strValue),":",saArray)==3)
			{
				//120
				strValue=saArray.GetAt(0);
				m_nDeg=_ttoi(strValue);

				//strValue=saArray.GetAt(1);
				//saArray.RemoveAll();
				//if (SplitString(strValue,strlen(strValue),":",saArray)==2)
				//{
					//12
					m_nMinutes= _ttoi(saArray.GetAt(1));
					//12
					m_nSeconds=_ttoi(saArray.GetAt(2));
				//}
			}
		}
		else
		{
			m_nDeg=0;
			m_nMinutes=0;
			m_nSeconds=0;
		}
	}else
	{
		m_nDeg=0;
		m_nMinutes=0;
		m_nSeconds=0;
	}
}


void CLatLong::SetValue(CString strValueLeft, CString strValueRight)
{
	strValueLeft.TrimLeft();
	strValueLeft.TrimRight();
   
	strValueRight.TrimLeft();
	strValueRight.TrimRight();

	if (strlen(strValueLeft)&&strlen(strValueRight))
	{
		CStringArray saArrayLeft, saArrayRight;
		if(SplitString(strValueLeft,strlen(strValueLeft)," ",saArrayLeft)==2 && 
		   SplitString(strValueRight,strlen(strValueRight)," ",saArrayRight)==2)
		{

			// N or s
			CString ValueLeft=saArrayLeft.GetAt(0);
			ValueLeft.TrimLeft();
			ValueLeft.TrimRight();
			if (ValueLeft.CompareNoCase("N")==0)
			{
				m_dir=LAT_NORTH;
			}
			else if (ValueLeft.CompareNoCase("S")==0)
			{
				m_dir=LAT_SOUTH;
			}
			else if (ValueLeft.CompareNoCase("E")==0)
			{
				m_dir=LONG_EAST;
			}else if(ValueLeft.CompareNoCase("W")==0)
			{
				m_dir=LONG_WEST;
			}
			else
				return;

			ValueLeft=saArrayLeft.GetAt(1);
			CString ValueRight = saArrayRight.GetAt(1); 
			saArrayLeft.RemoveAll();
			saArrayRight.RemoveAll();
			if(SplitString(ValueLeft,strlen(ValueLeft),":",saArrayLeft)==3 &&
				SplitString(ValueRight,strlen(ValueRight),":",saArrayRight)==4)
			{
				m_nDeg=_ttoi(saArrayLeft.GetAt(0));
				m_nMinutes= _ttoi(saArrayLeft.GetAt(1));
				m_nSeconds=_ttoi(saArrayLeft.GetAt(2));
				m_nDelta = _ttoi(saArrayRight.GetAt(3));
				//}
			}
		}
		else
		{
			m_nDeg=0;
			m_nMinutes=0;
			m_nSeconds=0;
			m_nDelta = 0;
		}
	}else
	{
		m_nDeg=0;
		m_nMinutes=0;
		m_nSeconds=0;
		m_nDelta = 0;
	}
}

void CLatLong::ReadData(ArctermFile &file)
{
	char szValue[256];
	file.getField(szValue,256);
	CString strValue=szValue;
	SetValue(strValue);

}

void CLatLong::WriteData(ArctermFile &file)
{
	CString strValue;
	CString strDirection;

	strValue.Format("%s %d:%d.%d:%d",m_dir,m_nDeg,m_nMinutes,m_nSeconds,m_nDelta);
	file.writeField(strValue);
}

int CLatLong::SplitString(LPCTSTR lpszString, int nStrLen, int chSep, CStringArray &strArray)
{
	if(strArray.GetSize()>0)
		strArray.RemoveAll();
	char ch;
	if(nStrLen>0)
	{
		char *pBuffer=new char[nStrLen+1];
		int index=0,nTemp=0;
		CString strText;
		while(index<nStrLen)
		{
			ch=lpszString[index];
			if(ch==chSep)
			{
				pBuffer[nTemp]='\0';
				nTemp=0;
				strArray.Add(pBuffer);
			}
			else
			{
				pBuffer[nTemp++]=ch;
				if(index==nStrLen-1)
				{
					pBuffer[nTemp]='\0';
					strArray.Add(pBuffer);
				}
			}
			index++;
		}
		delete[] pBuffer;
	}
	return strArray.GetSize();
}


int CLatLong::SplitString(LPCTSTR lpszString, int nStrLen, LPCTSTR lpszSep, CStringArray &strArray)
{
	if(strArray.GetSize()>0)
		strArray.RemoveAll();
	if(nStrLen>0)
	{
		if(lpszSep==NULL || strlen(lpszSep)==0)
		{
			strArray.Add(lpszString);
			return 1;
		}
		char *pBuffer=new char[nStrLen+1];
		int index=0,nTemp=0;
		char *pTemp=(char*) strstr(lpszString+index,lpszSep);
		while(pTemp)
		{
			nTemp=pTemp-lpszString;
			strncpy(pBuffer,lpszString+index,nTemp-index);
			pBuffer[nTemp-index]='\0';
			strArray.Add(pBuffer);
			index=nTemp+strlen(lpszSep);
			if(index>=nStrLen)
				break;
			pTemp=(char*) strstr(lpszString+index,lpszSep);
			if(!pTemp)
				strArray.Add(lpszString+index);
		}
		if(index==0)
		{
			strArray.Add(lpszString);
		}
		delete[] pBuffer;
	}
	return strArray.GetSize();
}

//////////////////////////////////////////////////////////////////////////
//class CLatitude

CLatitude::CLatitude()
{
	m_dir=LAT_NORTH;

}

CLatitude::CLatitude( long sec )
{
	if(sec >=0 ) m_dir = "N";
	else m_dir = "S";
	long llat = abs(sec);
	m_nDeg = (llat)/ (60*60);
	llat -= m_nDeg  * 60 * 60 ;
	m_nMinutes = int( llat / 60 );
	llat -= m_nMinutes  * 60;
	m_nSeconds = llat;
	m_nDelta = 0;

}
CLatitude::~CLatitude()
{

}

//////////////////////////////////////////////////////////////////////////
//class CLongitude

CLongitude::CLongitude()
{
    m_dir=LONG_EAST;
}

CLongitude::CLongitude( long sec )
{
	if(sec >=0 ) m_dir = "E";
	else m_dir = "W";
	long llat = abs(sec);
	m_nDeg = (llat)/ (60*60);
	llat -= m_nDeg  * 60 * 60 ;
	m_nMinutes = int( llat / 60 );
	llat -= m_nMinutes  * 60;
	m_nSeconds = llat;
	m_nDelta = 0;
}
CLongitude::~CLongitude()
{

}
