// CollisonTimeSegment.cpp: implementation of the CCollisonTimeSegment class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CollisonTimeSegment.h"
#include "common\states.h"
#include "reports\rep_pax.h"
#include "common\line.h"
#include "common\pollygon.h"
#include "engine\terminal.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCollisonTimeSegment::CCollisonTimeSegment()
{

}

CCollisonTimeSegment::~CCollisonTimeSegment()
{
	
	MAP_PERSON::iterator iter = m_mapPersonTrackInfo.begin();
	MAP_PERSON::iterator iterEnd = m_mapPersonTrackInfo.end();
	for( ; iter != iterEnd ;++iter )
	{
		if( iter->second )
		{
			iter->second->clear();
			delete iter->second;
		}
	}
	
}
void CCollisonTimeSegment::AddPersonTrackInfo( int _iPersonIdx, TRACKPAIR& _trackPair ,ElapsedTime& _startTime ,ElapsedTime& _endTime )
{
	if( m_startTime > _endTime || m_endTime < _startTime )
		return;
	TRACKPAIRVECTOR* pTrackInfo = m_mapPersonTrackInfo[ _iPersonIdx ];
	if( !pTrackInfo )
	{
		pTrackInfo = new TRACKPAIRVECTOR;
		m_mapPersonTrackInfo[ _iPersonIdx ] = pTrackInfo;
		
	}
	
	pTrackInfo->push_back( _trackPair );
}

void CCollisonTimeSegment::CaculateAll( const CMobileElemConstraint& _paxType, ArctermFile& _file,float _threshold ,Pollygon& _region )
{
	MAP_PERSON::iterator iter = m_mapPersonTrackInfo.begin();
	MAP_PERSON::iterator iterEnd = m_mapPersonTrackInfo.end();
	ReportPaxEntry element, anotherElement;
	element.SetOutputTerminal( m_pTerm );
	anotherElement.SetOutputTerminal(m_pTerm);
	element.SetEventLog( m_pTerm->m_pMobEventLog );  
	anotherElement.SetEventLog( m_pTerm->m_pMobEventLog);
	MAP_PERSON::iterator iterAnoter;
	//int iSizeNouse =m_mapPersonTrackInfo.size();
	//int iTrace=0;
	for( ; iter != iterEnd ;++iter )
	{
		//// TRACE("\niTrace=%d\n", iTrace++ );
		m_pTerm->paxLog->getItem( element , iter->first );
		element.clearLog();
		int groupMember = element.getGroupMember();
		iterAnoter = iter;
		iterAnoter++;
		for( ; iterAnoter != iterEnd ; ++iterAnoter)
		{
			m_pTerm->paxLog->getItem( anotherElement , iterAnoter->first );
			anotherElement.clearLog();
			//pax type
			if( !element.fits( _paxType ) & !anotherElement.fits( _paxType ) )
				continue;

			//filter0.9: no greeter/pax collision.
			if(anotherElement.isGreeter())				
			{
				if(element.getID() == anotherElement.getOwnerID())
						continue;
			}
			else
			{
				if(element.isGreeter())
					if(anotherElement.getID() == element.getOwnerID())
						continue;
			}

			//filter0.95: no pax collision in a same group
			int groupMember_tmp = anotherElement.getGroupMember();	
			if(anotherElement.getID() - element.getID() == groupMember_tmp - groupMember)
				continue;
		

			TRACKPAIRVECTOR::iterator pairIter = iter->second->begin();
			TRACKPAIRVECTOR::iterator pairIterEnd = iter->second->end();
			
			//int nSize = iter->second->size();
			for( ; pairIter != pairIterEnd ;++pairIter )
			{
				MobEventStruct firstEventStruct1( element.getEvent(pairIter->m_lFirstTrackIdx) );
				MobEventStruct firstEventStruct2( element.getEvent(pairIter->m_lSecondTrackIdx ) );
				int iFirstState =firstEventStruct1.state;
				TRACKPAIRVECTOR::iterator otherPairIter = iterAnoter->second->begin();
				TRACKPAIRVECTOR::iterator otherPairIterEnd = iterAnoter->second->end();
				for( ; otherPairIter != otherPairIterEnd ; ++otherPairIter )
				{
					MobEventStruct secondEventStruct1( anotherElement.getEvent(otherPairIter->m_lFirstTrackIdx) );
					
					int iSecondState =secondEventStruct1.state;
					if( iFirstState != iSecondState || iFirstState != WaitInQueue || iFirstState != WaitingForBags )
					{
						MobEventStruct secondEventStruct2( anotherElement.getEvent(otherPairIter->m_lSecondTrackIdx ) );	
						Point firstPoint1( firstEventStruct1.x,firstEventStruct1.y,firstEventStruct1.z);
						Point firstPoint2( firstEventStruct2.x,firstEventStruct2.y,firstEventStruct2.z);

						Point secondPoint1 ( secondEventStruct1.x,secondEventStruct1.y,secondEventStruct1.z);
						Point secondPoint2( secondEventStruct2.x,secondEventStruct2.y,secondEventStruct2.z);
						
						Line firstLine( firstPoint1, firstPoint2 );
						Line secondLine( secondPoint1, secondPoint2 );
						if(firstLine.intersects(secondLine) == TRUE) //line overlap
							{
								//filter6: check if in the THRESHOLD circle.
								Point intersection = firstLine.intersection(secondLine);
								if( _region.getCount() >2 )
								{
									intersection.setZ( _region.getPoint(0).getZ());
									if( ! _region.contains( intersection ) )
										continue;
								}
									
								
								DistanceUnit distance; 
								
								distance = firstPoint1.distance(intersection);
								
								
								ElapsedTime t1 = firstEventStruct1.time / TimePrecision + (long)( ( distance - _threshold/2 ) * ( firstEventStruct2.time - firstEventStruct1.time )/ TimePrecision / firstLine.GetLineLength());
								ElapsedTime t2 = firstEventStruct2.time / TimePrecision - (long)( (firstPoint2.distance( intersection ) - _threshold/2 ) * ( firstEventStruct2.time - firstEventStruct1.time )/TimePrecision / firstLine.GetLineLength());
								ElapsedTime t3 = secondEventStruct1.time / TimePrecision + (long)( (secondPoint1.distance( intersection ) - _threshold/2 ) * ( secondEventStruct2.time - secondEventStruct1.time )/TimePrecision / secondLine.GetLineLength());
								ElapsedTime t4 = secondEventStruct2.time / TimePrecision - (long)( (secondPoint2.distance( intersection ) - _threshold/2 ) * ( secondEventStruct2.time - secondEventStruct1.time) /TimePrecision/ secondLine.GetLineLength());

								if( t2 <= t3 || t1 >= t4 || t2<= t1 || t4 <= t3)
										continue;
								ElapsedTime interTime ;
								if( t1 < t4 )
								{
									interTime = t1 + (long)(( t4.asSeconds() - t1.asSeconds() ) /2 );
								}
								else
								{
									interTime = t3 + (long)(( t2.asSeconds() - t3.asSeconds() ) /2 );
								}														

								if( interTime >= m_startTime && m_endTime >= interTime )
								{
									//char str[64];
									CString str;
									_paxType.screenPrint (str);
									_file.writeField (str.GetBuffer(0));					        					
									_file.writeTime ( interTime  ,TRUE);												
									CString strID;
									strID.Format("%ld",element.getID());							
									_file.writeField(strID);							
									strID.Format("%ld",anotherElement.getID() );
									_file.writeField(strID);
									_file.writeLine();													
								}
						
			
							}
		
					}	
				}
			}
		}
	}

}

void CCollisonTimeSegment::ClearAll()
{
	MAP_PERSON::iterator iter = m_mapPersonTrackInfo.begin();
	MAP_PERSON::iterator iterEnd = m_mapPersonTrackInfo.end();
	for( ; iter != iterEnd ;++iter )
	{
		if( iter->second )
		{
			iter->second->clear();
			delete iter->second;
		}
	}
	m_mapPersonTrackInfo.clear();
}