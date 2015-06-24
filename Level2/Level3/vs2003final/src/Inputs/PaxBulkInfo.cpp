// PaxBulkInfo.cpp: implementation of the CPaxBulkInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "PaxBulkInfo.h"

#include "common\termfile.h"
#include "../Common/ARCTracker.h"
#include <algorithm>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
bool CPaxBulk::fits( const CMobileElemConstraint& _const)
{

	//fits passenger
	if(m_mobileElemConst.fitex( _const)==false)return false;
	return true;
}
//either all shuttle buses have departed or passenger can't take later bus, return false;
//or return true;
bool CPaxBulk::takeCurrentBulk(ElapsedTime& time , long& nIndex, int _nPaxGroup,const ElapsedTime& flyTime)
{
	nIndex=0;
	bool bReturn=false;
	std::vector<int>::iterator iter;
	for( iter=m_vBulkSeats.begin();iter!=m_vBulkSeats.end(); )
	{
		if( *iter >0) 
		{
			if( m_TimeStart +m_TimeFrequency * nIndex < m_TimeRangeBegin + flyTime)
			{
				iter++ ;
				nIndex++ ;
				continue ;
			}
			if(m_TimeStart +m_TimeFrequency * nIndex > m_TimeRangeEnd + flyTime)
			{
				bReturn=false;
				break;
			}
	
			if(*iter - _nPaxGroup < 0)
			{
				iter ++;
				nIndex ++;
				continue;
			}
			*iter -= _nPaxGroup;
			//*iter -=min( *iter, _nPaxGroup);//allow over loading.
				//if(*iter <=0) m_nBulkTimes--;
			bReturn=true;
			time= m_TimeStart + (m_TimeFrequency * nIndex  );
			break;
		}
		else
		{
			iter++ ;
			nIndex++ ;
		}
	}

	return bReturn;
}
CPaxBulkInfo::CPaxBulkInfo():DataSet (PassengerBulkFile, 2.3f)
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
			if(!((iterBulk->m_TimeStart  <= iterBulk->m_TimeRangeBegin + flyTime && iterBulk->m_TimeEnd >=iterBulk->m_TimeRangeBegin+flyTime)
				||(iterBulk->m_TimeStart  <= iterBulk->m_TimeRangeEnd + flyTime && iterBulk->m_TimeEnd >=iterBulk->m_TimeRangeEnd+flyTime)))
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
