// PaxBulkInfo.cpp: implementation of the CPaxBulkInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "PaxBulkInfo.h"

#include "common\termfile.h"
#include "../Common/ARCTracker.h"
#include <algorithm>
#include "..\Common\UTIL.H"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
bool CPaxBulk::fits( const CMobileElemConstraint& _const)
{

	//fits passenger
	if(m_mobileElemConst.fitex( _const)==false)return false;
	return true;
}

bool CPaxBulk::GetStartEndRange( long& iStart, long& iEnd, long lIndex,const ElapsedTime& flyTime )
{
	long iBulkCount = (long)m_vBulkPercent.size();
	ElapsedTime startTime = m_TimeStart + m_TimeFrequency * lIndex;
	ElapsedTime beginFlyTime = m_TimeRangeBegin + flyTime;
	for (long i = 0; i < iBulkCount; i++)
	{
		ElapsedTime enterTime = beginFlyTime + m_TimeFrequency*i;
		CString strEnterTime;
		strEnterTime = enterTime.printTime();
		if (enterTime < m_TimeStart)
			continue;
		
		if (enterTime > m_TimeEnd)
			continue;
		
		if (enterTime < startTime)
			continue;

		if (iStart == -1)
		{
			iStart = i;
			continue;
		}

		if (iEnd == -1)
		{
			iEnd = i;
			continue;
		}

		iStart = MIN(iStart,i);
		iEnd = MAX(iEnd,i);
	}

	if (iStart == -1 || iEnd == -1)
	{
		return false;
	}
	return true;
}

bool CPaxBulk::GetRangeStartIndex( long& nIndex,const ElapsedTime& flyTime )
{
	std::vector<int>::iterator iter;
	for( iter=m_vBulkSeats.begin();iter!=m_vBulkSeats.end(); )
	{
		
		if( m_TimeStart +m_TimeFrequency * nIndex < m_TimeRangeBegin + flyTime)
		{
			iter++ ;
			nIndex++ ;
			continue ;
		}
		if(m_TimeStart +m_TimeFrequency * nIndex > m_TimeRangeEnd + flyTime)
		{
			return false;
		}
		return true;
	}

	return false;
}
//either all shuttle buses have departed or passenger can't take later bus, return false;
//or return true;
bool CPaxBulk::takeCurrentBulk(ElapsedTime& time , long& nIndex, int _nPaxGroup,const ElapsedTime& flyTime)
{
	nIndex=0;
	//bool bReturn=false;
	//std::vector<int>::iterator iter;
	//for( iter=m_vBulkSeats.begin();iter!=m_vBulkSeats.end(); )
	//{
	//	if( *iter >0) 
	//	{
	//		if( m_TimeStart +m_TimeFrequency * nIndex < m_TimeRangeBegin + flyTime)
	//		{
	//			iter++ ;
	//			nIndex++ ;
	//			continue ;
	//		}
	//		if(m_TimeStart +m_TimeFrequency * nIndex > m_TimeRangeEnd + flyTime)
	//		{
	//			bReturn=false;
	//			break;
	//		}
	//
	//		if(*iter - _nPaxGroup < 0)
	//		{
	//			iter ++;
	//			nIndex ++;
	//			continue;
	//		}
	//	//	*iter -= _nPaxGroup;
	//		//*iter -=min( *iter, _nPaxGroup);//allow over loading.
	//			//if(*iter <=0) m_nBulkTimes--;
	//		bReturn=true;
	//		//time= m_TimeStart + (m_TimeFrequency * nIndex  );
	//		break;
	//	}
	//	else
	//	{
	//		iter++ ;
	//		nIndex++ ;
	//	}
	//}

	//if (bReturn == false)//doesn't find
	//{
	//	return false;
	//}

	if (GetRangeStartIndex(nIndex,flyTime) == false)
		return false;
	

	long iStart = -1;
	long iEnd = -1;
	if (GetStartEndRange(iStart,iEnd,nIndex,flyTime) == false)
		return false;
	
	int percentAll = 0;
	int nPercentCount = (int)m_vBulkPercent.size();
	for (int i = iStart; i <= iEnd; i++)
	{
		percentAll += m_vBulkPercent[i];
	}

	//if(percentAll<=0)
	//	return false;

	int randP = random(percentAll);
	int iPercent = 0;
	for (int j = iStart; j <= iEnd; j++)
	{
		iPercent += m_vBulkPercent[j];
		if (randP < iPercent)
		{
			long lInterVal = nIndex + j - iStart;
			if (m_vBulkSeats[lInterVal] - _nPaxGroup < 0)
			{
				return false;
			}
			m_vBulkSeats[lInterVal] -= _nPaxGroup;
			time= m_TimeStart + (m_TimeFrequency * lInterVal );
			return true;
		}
	}
	return false;
}

bool CPaxBulk::TimeOverLap(const ElapsedTime& flyTime) const
{
	ElapsedTime startFlyTime = flyTime + m_TimeRangeBegin;
	ElapsedTime endFlyTime = flyTime + m_TimeRangeEnd;

	if (startFlyTime >= m_TimeStart && startFlyTime <= m_TimeEnd)
	{
		return true;
	}

	if (endFlyTime >= m_TimeStart && endFlyTime <= m_TimeEnd)
	{
		return true;
	}

	if (m_TimeStart >= startFlyTime && m_TimeStart <= endFlyTime)
	{
		return true;
	}

	if (m_TimeEnd >= startFlyTime && m_TimeEnd <= endFlyTime)
	{
		return true;
	}
	return false;
}



CPaxBulkInfo::CPaxBulkInfo():DataSet (PassengerBulkFile, 2.4f)
{
}

CPaxBulkInfo::~CPaxBulkInfo()
{
}

void CPaxBulkInfo::readData( ArctermFile& p_file )
{
	p_file.reset();
	p_file.skipLines (3);		
	while (p_file.getLine())			
	{
		CPaxBulk paxBulk;
		paxBulk.m_mobileElemConst.SetInputTerminal(GetInputTerminal());
		paxBulk.m_mobileElemConst.readConstraint(p_file);
		long sec;
		p_file.getInteger(sec);
	    paxBulk.m_TimeRangeBegin.set(sec);
		p_file.getInteger(sec);
	    paxBulk.m_TimeRangeEnd.set(sec);
			p_file.getInteger(sec);
	    paxBulk.m_TimeStart.set(sec);
		p_file.getInteger(sec);
        paxBulk.m_TimeEnd.set(sec);
		p_file.getInteger(sec);
	    paxBulk.m_TimeFrequency.set(sec);
		p_file.getInteger(paxBulk.m_nCapacity);	

		int iPercentCount = 0;
		p_file.getInteger(iPercentCount);
		for (int j = 0; j < iPercentCount; j++)
		{
			int iPercent = 0;
			p_file.getInteger(iPercent);
			paxBulk.m_vBulkPercent.push_back(iPercent);
		}

		m_vectBulkInfo.push_back( paxBulk );
	}
	SortData() ;
}
class CCompareFunction
{
public:
	bool operator()( const CPaxBulk& _First , const CPaxBulk& _next)
	{
		
		if( _First.GetMobileElementConstraint() < (_next.GetMobileElementConstraint())  )
			return TRUE ;
		else
			return FALSE ;
	}
};
void CPaxBulkInfo::readObsoleteData(ArctermFile&p_file)
{
	if (m_fVersion < 2.40)
	{
		p_file.reset();
		p_file.skipLines (3);		
		while (p_file.getLine())			
		{
			CPaxBulk paxBulk;
			paxBulk.m_mobileElemConst.SetInputTerminal(GetInputTerminal());
			paxBulk.m_mobileElemConst.readConstraint(p_file);
			long sec;
			p_file.getInteger(sec);
			paxBulk.m_TimeRangeBegin.set(sec);
			p_file.getInteger(sec);
			paxBulk.m_TimeRangeEnd.set(sec);
			p_file.getInteger(sec);
			paxBulk.m_TimeStart.set(sec);
			p_file.getInteger(sec);
			paxBulk.m_TimeEnd.set(sec);
			p_file.getInteger(sec);
			paxBulk.m_TimeFrequency.set(sec);
			p_file.getInteger(paxBulk.m_nCapacity);	
			m_vectBulkInfo.push_back( paxBulk );
		}
		SortData() ;
	}

	if (m_fVersion < 2.30)
	{
		p_file.reset();
		p_file.skipLines (3);		
		while (p_file.getLine())			
		{
			CPaxBulk paxBulk;
			paxBulk.m_mobileElemConst.SetInputTerminal(GetInputTerminal());
			paxBulk.m_mobileElemConst.readConstraint(p_file);
			p_file.getTime( paxBulk.m_TimeRangeBegin);
			p_file.getTime( paxBulk.m_TimeRangeEnd);
			p_file.getTime( paxBulk.m_TimeStart);
			p_file.getTime( paxBulk.m_TimeEnd);
			p_file.getTime(paxBulk.m_TimeFrequency);
			p_file.getInteger(paxBulk.m_nCapacity);	
			m_vectBulkInfo.push_back( paxBulk );
		}
		SortData() ;
	}
	
	
}
void CPaxBulkInfo::SortData()
{
std::sort(m_vectBulkInfo.begin(),m_vectBulkInfo.end(),CCompareFunction()) ;
}
void CPaxBulkInfo::writeData( ArctermFile& p_file ) const
{
//	m_mobileElemConst->SetInputTerminal(CPaxBulkInfo::GetInputTerminal());	
	
//	GetInputTerminal();
//	m_mobileElemConst->SetInputTerminal(this->);
	int nCount = m_vectBulkInfo.size();
	for(int i = 0; i < nCount; i++)
	{
		m_vectBulkInfo[i].m_mobileElemConst.writeConstraint(p_file);
		//p_file.writeTime( m_vectBulkInfo[i].m_TimeRangeBegin,FALSE,TRUE);
		//p_file.writeTime( m_vectBulkInfo[i].m_TimeRangeEnd,FALSE,TRUE);
		//p_file.writeTime( m_vectBulkInfo[i].m_TimeStart);
		//p_file.writeTime( m_vectBulkInfo[i].m_TimeEnd);
		//p_file.writeTime(m_vectBulkInfo[i].m_TimeFrequency);
		p_file.writeInt( m_vectBulkInfo[i].m_TimeRangeBegin.asSeconds());
		p_file.writeInt( m_vectBulkInfo[i].m_TimeRangeEnd.asSeconds());
		p_file.writeInt( m_vectBulkInfo[i].m_TimeStart.asSeconds());
		p_file.writeInt( m_vectBulkInfo[i].m_TimeEnd.asSeconds());
		p_file.writeInt(m_vectBulkInfo[i].m_TimeFrequency.asSeconds());



		p_file.writeInt(m_vectBulkInfo[i].m_nCapacity);	

		int iPercentCount = (int)m_vectBulkInfo[i].m_vBulkPercent.size();
		p_file.writeInt(iPercentCount);
		for (int j = 0; j < iPercentCount; j++)
		{
			int iPercent = m_vectBulkInfo[i].m_vBulkPercent[j];
			p_file.writeInt(iPercent);
		}
		p_file.writeLine();
	}	
}

void CPaxBulkInfo::initBulk()
{
	PLACE_METHOD_TRACK_STRING();
	
	int nBulkCount=0;
	int nBulkIndex=0;
	std::vector<CPaxBulk>::iterator iterBulk;
	ElapsedTime timeLength;
	for(iterBulk=m_vectBulkInfo.begin(); iterBulk!=m_vectBulkInfo.end();iterBulk++)
	{
		nBulkIndex ++;
		iterBulk->m_nBulkIndex = nBulkIndex; //car index in vector
		//iterBulk->m_nBulkTimes = 0;         // one index car 's times
		nBulkCount= ( iterBulk->m_TimeEnd.asMinutes() - iterBulk->m_TimeStart.asMinutes() )/iterBulk->m_TimeFrequency.asMinutes() +1;
		iterBulk->m_vBulkSeats.clear();
		for(long i=0;i<nBulkCount;i++)
		{
		//	timeLength = iterBulk->m_TimeStart + iterBulk->m_TimeFrequency * i ;
		//	if( timeLength>=iterBulk->m_TimeRangeBegin && timeLength<=iterBulk->m_TimeRangeEnd )
		//	{
				iterBulk->m_vBulkSeats.push_back(iterBulk->m_nCapacity);//this bus can take.
			//	iterBulk->m_nBulkTimes ++;
		//	}
		//	else
		//	{
			//	iterBulk->m_vBulkSeats.push_back(0);// this bus can't take.
		//	}
			
		}
	}
}

//find a proper bulk and return its index in m_vectorBulkInfo or return -1;
int CPaxBulkInfo::findFitBulk( const CMobileElemConstraint& _const)
{
	int nResultBulk=-1;
//	int nIndex=0;
	std::vector<CPaxBulk>::iterator iterBulk;
	for(iterBulk=m_vectBulkInfo.begin(); iterBulk!=m_vectBulkInfo.end(); iterBulk++ )//, nIndex++ )
	{
		if( iterBulk->fits( _const)) //&& iterBulk->m_nBulkTimes >0)
		{
			nResultBulk = iterBulk->m_nBulkIndex;//nIndex;
			//// TRACE("Bulk index %d\n", nIndex);
			break;
		}
	}
	return nResultBulk;
}
int CPaxBulkInfo::findValuedBulk(ElapsedTime&time,long& nfreqIndex,int group,const CMobileElemConstraint& _const,const ElapsedTime&flyTime)
{
	int nResultBulk=-1;
	std::vector<CPaxBulk>::iterator iterBulk;
	for(iterBulk=m_vectBulkInfo.begin(); iterBulk!=m_vectBulkInfo.end(); iterBulk++)
	{
		if(iterBulk->fits( _const))
		{
		/*	if(!((iterBulk->m_TimeStart  <= iterBulk->m_TimeRangeBegin + flyTime && iterBulk->m_TimeEnd >=iterBulk->m_TimeRangeBegin+flyTime)
				||(iterBulk->m_TimeStart  <= iterBulk->m_TimeRangeEnd + flyTime && iterBulk->m_TimeEnd >=iterBulk->m_TimeRangeEnd+flyTime)))
              continue;*/
			
			if (iterBulk->TimeOverLap(flyTime) == false)
				continue;

			if(iterBulk->takeCurrentBulk(time,nfreqIndex,group,flyTime))
			{
				nResultBulk = iterBulk->m_nBulkIndex;
			    // TRACE("Bulk index %d\n", iterBulk->m_nBulkIndex);
			    break;
			}
		}
	}
	return nResultBulk;
}
