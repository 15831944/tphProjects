// StringConvertToFlight.cpp: implementation of the CStringConvertToFlight class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "StringConvertToFlight.h"
#include "GateMappingDB.h"
#include "flight.h"
#include "schedule.h"
#include "airsideinput.h"
#include "common\termfile.h"
#include "common\AirportsManager.h"
#include "common\AirlineManager.h"
#include "common\ACTypesManager.h"
#include "engine\proclist.h"
#include "../InputAirside/ALTObject.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStringConvertToFlight::CStringConvertToFlight( InputTerminal* _pTerm,int nAirportID ):m_pTerm( _pTerm )
{
	ALTObject::GetObjectNameList(ALT_STAND,nAirportID,m_vObjectIDList);
}

CStringConvertToFlight::~CStringConvertToFlight()
{

}
// build a flight according to the string _sFormateStr and gate map database
Flight* CStringConvertToFlight::ConvertToFlight(const CString _sFormateStr,ArctermFile& _fAgentFile ,const CGateMappingDB& _dbGateMap, bool _bArrTimeSec,
												bool _bDepTimeSec, CStartDate &ConvertStartDate ,CString& strErrMsg, DATAFORMAT dataFormat)
{
	char szDate[16];
	int	nNum[3];

	COleDateTime dt;
	// init file internal data
	_fAgentFile.InitDataFromString( _sFormateStr );
	_fAgentFile.format();

	char tempData[ LARGE ];
	Flight* pFlight = new Flight( (Terminal*)m_pTerm );


	// air line
    if( ! _fAgentFile.getField (tempData, AIRLINE_LEN) )
	{

        strErrMsg.Append(_T("Get Air Line Error!"));
		delete pFlight;
		return NULL;
	}
	else
	{
		// the length of airline must less than 4
		CString strAirLine = CString( tempData );
		strAirLine = strAirLine.Left(30);
		pFlight->setAirline( strAirLine );
	}
	_g_GetActiveAirlineMan( m_pTerm->m_pAirportDB )->FindOrAddEntry( CString( tempData ) );

	
    // arrival info
    if (_fAgentFile.isBlankField())
    {
		pFlight->setArrID( 0 );
        _fAgentFile.skipField(1);
    }
    else
	{
		if( ! _fAgentFile.getField(tempData, FLIGHT_LEN) )
		{
			 strErrMsg.Append(_T("Get Flight Error!"));
			delete pFlight;
			return NULL;			
		}
		else
		{
			//pFlight->setArrID( iArrID );
			pFlight->setArrID( tempData );
		}
	}

	// arr orign airport
	if( !_fAgentFile.getField (tempData, AIRPORT_LEN) )
	{
		if( pFlight->getArrID().HasValue() )
		{
			strErrMsg.Append(_T("Arr ID has no value!"));
			delete pFlight;
			return NULL;
		}
	}
	else
	{
		pFlight->setAirport( tempData , 'A' );
		_g_GetActiveAirportMan( m_pTerm->m_pAirportDB )->FindOrAddEntry( CString( tempData ) );
	}

	// arr from airport
	if( !_fAgentFile.getField (tempData, AIRPORT_LEN) )
	{
		if( pFlight->getArrID().HasValue() )
		{
			//delete pFlight;
			//return NULL;
		}
	}
	else
	{
		pFlight->setStopoverAirport( tempData , 'A' );
		_g_GetActiveAirportMan( m_pTerm->m_pAirportDB )->FindOrAddEntry( CString( tempData ) );
	}


	int i = 0;
	//CStartDate sdStart, sdEnd;
	if (!_fAgentFile.isBlankField())
	{
		memset(szDate, 0, sizeof(szDate) / sizeof(char));
		_fAgentFile.getField(szDate, 10);
		CString strDate = szDate;
		if (!strDate.IsEmpty())
		{
			char chSplit = 0;
			if (strDate.Find('-') != -1)
				chSplit = '-';
			else if (strDate.Find('/') != -1)
				chSplit = '/';
			else
			{
				//delete pFlight;
				//strErrMsg = _T("Bad Arrival Date formart");
				//return NULL;
				ASSERT(!ConvertStartDate.IsAbsoluteDate());
			}

			if (chSplit != 0)
			{
				int nPrevPos = 0;
				int nPos = 0;
				while (((nPos = strDate.Find(chSplit, nPos + 1)) != -1) && (i < 3))
				{
					nNum[i++] = atoi(strDate.Mid(nPrevPos, nPos - nPrevPos));
					nPrevPos = nPos + 1;
				}

				nNum[i] = atoi(strDate.Mid(nPrevPos, strDate.GetLength() - nPrevPos));

				if (nNum[0] > 100)
				{
					dt.SetDate(nNum[0], nNum[1], nNum[2]);
				}
				else
				{
					//usa date format
					if (dataFormat==DATAFORMAT_USA)
					{
						dt.SetDate(nNum[2], nNum[0], nNum[1]);
					}
					//european date format
					else if (dataFormat==DATAFORMAT_EUR)
					{
						dt.SetDate(nNum[2], nNum[1], nNum[0]);
					}
					else
					{
						ASSERT(FALSE);
					}
				}
			}
			else
			{
				int nDay = atoi(strDate);
				ElapsedTime estTime;
				estTime.SetDay(nDay);

				pFlight->setArrTime(estTime);
			}
			//delete by adam 2007-05-11
			//sDate = m_pTerm->flightSchedule->GetStartDate();
			//if( !sDate.IsAbsoluteDate() )
			//{
			//	sDate.SetAbsoluteDate( true );
			//	sDate.SetDate( dt );
			//	m_pTerm->flightSchedule->SetStartDate( sDate );
			//}
			//end delete by adam 2007-05-11
		}
	}
	else
	{
		//if (ConvertStartDate.IsAbsoluteDate())
		//{
		//	delete pFlight;
		//	strErrMsg = _T("The Arrive Date could not empty");
		//	return NULL;
		//}
		_fAgentFile.skipField(1);
	}
	


	ElapsedTime tempTime, t;
	// arr time
	if( !_fAgentFile.getTime(tempTime,_bArrTimeSec) )
	{
		if( pFlight->getArrID().HasValue() )
		{
			delete pFlight;
			return NULL;
		}
	}
	else
	{
		//modify by adam 2007-05-11
		if( ConvertStartDate.IsAbsoluteDate() )
		{
			ConvertStartDate.GetRelativeTime( dt, tempTime, t);
			pFlight->setArrTime(t);
		}
		else
		{
			ElapsedTime estArrivalTime = pFlight->getArrTime();
			estArrivalTime += tempTime;
			pFlight->setArrTime( estArrivalTime );
		}
		//end modify by adam 2007-05-11
	}

    // depature info

	if (_fAgentFile.isBlankField())
    {
		pFlight->setDepID( 0 );
        _fAgentFile.skipField(1);
    }
    else
	{
		if( ! _fAgentFile.getField(tempData, FLIGHT_LEN) )
		{
			strErrMsg.Append(_T("Get Dep ID Error!"));
			delete pFlight;
			return NULL;			
		}
		else
		{
			//pFlight->setDepID( iDepID );
			pFlight->setDepID( tempData );
		}
	}

	// to airport
	if( !_fAgentFile.getField (tempData, AIRPORT_LEN) )
	{
		if( pFlight->getDepID().HasValue() )
		{
			//delete pFlight;
			//return NULL;
		}
	}
	else
	{
		pFlight->setStopoverAirport( tempData , 'D' );
		_g_GetActiveAirportMan( m_pTerm->m_pAirportDB )->FindOrAddEntry( CString( tempData ) );
	}

	// dep airport
	if( !_fAgentFile.getField (tempData, AIRPORT_LEN) )
	{
		if( pFlight->getDepID().HasValue() )
		{
			strErrMsg.Append(_T("Get Airport Error!"));
			delete pFlight;
			return NULL;
		}
	}
	else
	{
		pFlight->setAirport( tempData , 'D' );
		_g_GetActiveAirportMan( m_pTerm->m_pAirportDB )->FindOrAddEntry( CString( tempData ) );
	}

	// dep date
	i = 0;
	if (!_fAgentFile.isBlankField())
	{
		memset(szDate, 0, sizeof(szDate) / sizeof(char));
		_fAgentFile.getField(szDate, 10);
		CString strDate = szDate;
		if (!strDate.IsEmpty())
		{
			char chSplit = 0;
			if (strDate.Find('-') != -1)
				chSplit = '-';
			else if (strDate.Find('/') != -1)
				chSplit = '/';
			else
			{
				//delete pFlight;
				//strErrMsg = _T("Bad Departure Date formart");
				//return NULL;
				ASSERT(!ConvertStartDate.IsAbsoluteDate());
			}

			if (chSplit != 0)
			{
				int nPrevPos = 0;
				int nPos = 0;
				while (((nPos = strDate.Find(chSplit, nPos + 1)) != -1) && (i < 3))
				{
					//CString strTemp = strDate.Mid(nPrevPos, nPos - nPrevPos); 
					nNum[i++] = atoi(strDate.Mid(nPrevPos, nPos - nPrevPos));
					nPrevPos = nPos + 1;
				}

				//CString strTemp = strDate.Mid(nPrevPos, strDate.GetLength() - nPrevPos); 
				nNum[i] = atoi(strDate.Mid(nPrevPos, strDate.GetLength() - nPrevPos));

				if (nNum[0] > 100)
				{
					// yyyy-mm-dd
					dt.SetDate(nNum[0], nNum[1], nNum[2]);
				}
				else
				{
					//usa date format
					if (dataFormat==DATAFORMAT_USA)
					{
						dt.SetDate(nNum[2], nNum[0], nNum[1]);
					}
					//european date format
					else if (dataFormat==DATAFORMAT_EUR)
					{
						dt.SetDate(nNum[2], nNum[1], nNum[0]);
					}
					else
					{
						ASSERT(FALSE);
					}
				}
			}			

			//delete by adam 2007-05-10
			//sDate = m_pTerm->flightSchedule->GetStartDate();  
			//if ( !sDate.IsAbsoluteDate() )
			//{
			//	sDate.SetAbsoluteDate( true );
			//	sDate.SetDate( dt );
				//m_pTerm->flightSchedule->SetStartDate( sDate ); 
			//}
			//End delete by adam 2007-05-10
		}
	}
	else
	{
		//if (ConvertStartDate.IsAbsoluteDate())
		//{
		//	delete pFlight;
		//	strErrMsg = _T("The Departure Date could not empty");
		//	return NULL;
		//}

		_fAgentFile.skipField(1);
	}	

	// dep time
	if( !_fAgentFile.getTime(tempTime,_bDepTimeSec) )
	{
		if( pFlight->getDepID().HasValue() )
		{
			strErrMsg = _T("Bad departure time");
			delete pFlight;
			return NULL;
		}	
	}
	else
	{
		//modify by adam 2007-05-11
		if( ConvertStartDate.IsAbsoluteDate() )
		{
			ConvertStartDate.GetRelativeTime( dt, tempTime, t );
			pFlight->setDepTime(t);
		}
		else
		{
			ElapsedTime estDepTime = pFlight->getDepTime();
			estDepTime += tempTime;
			pFlight->setDepTime(estDepTime);
		}
		//End modify by adam 2007-05-11
	}
    
    // ac type
	if( !_fAgentFile.getField( tempData, AC_TYPE_LEN) )
	{
		strErrMsg.Append(_T("Get ACType Error!"));
		delete pFlight;
		return NULL;
	}
	else
	{
		pFlight->setACType( tempData );
		_g_GetActiveACMan( m_pTerm->m_pAirportDB )->FindOrAddEntry( CString( tempData ) );
	}
	

	//arr stand
	if( !_fAgentFile.getField( tempData , 1024 ) )
	{
	//	delete pFlight;
	//	return NULL;
	}
	else
	{
		int iPos = _dbGateMap.getIndex( CString( tempData ) );
		if( iPos >= 0 )
		{
			// service time
			if( !_fAgentFile.getTime( tempTime ) )
			{
				strErrMsg.Append(_T("Get Service time  Error!"));
				delete pFlight;
				return NULL;
			}
			else
			{
				if (tempTime < 0L)
				{
					strErrMsg.Append(_T("Flight Departure time earlier than Arrival time!"));
					delete pFlight;
					return NULL;					
				}

				pFlight->setServiceTime( tempTime );
			}
			
			//pFlight->setGate(  _dbGateMap.getItem( iPos ).getStandGate() );
			//CString procID;
			char procID[255];
			_dbGateMap.getItem( iPos ).getStandGate().printID(procID);

			if (pFlight->isArriving())
			{
				pFlight->setArrStand( ALTObjectID(procID) );
			}
			if (pFlight->isDeparting())
			{
				pFlight->setDepStand( ALTObjectID(procID) );
			}

			pFlight->setArrGate( _dbGateMap.getItem( iPos ).getArrGate() );
			pFlight->setDepGate( _dbGateMap.getItem( iPos ).getDepGate() );
			return pFlight;
		}
		else
		{
			//stand gate
			ALTObjectID tempStandID( tempData );
			if( IsAValidStand( tempStandID ) )
			{
				char strID[255];
				tempStandID.printID(strID);
				//pFlight->setStand( ALTObjectID(strID) );
				if (pFlight->isArriving())
				{
					pFlight->setArrStand( ALTObjectID(strID) );
				}
			}
			else
			{
				strErrMsg  = _T("Have no stand OR have no stand fit source stand -") + CString(tempData);
				delete pFlight;
				return NULL;
			}
		}
	}

	///////////////////Add by Emily//////////////////////////////////
	//dep stand
	if( !_fAgentFile.getField( tempData , 1024 ) )	
	{
		//delete pFlight;
		//return NULL;
	}
	else
	{
		//dep stand
		ALTObjectID tempStandID( tempData );
		if( IsAValidStand( tempStandID ) )
		{
			char strID[255];
			tempStandID.printID(strID);
			//pFlight->setStand( ALTObjectID(strID) );
			if (pFlight->isDeparting())
			{
				pFlight->setDepStand( ALTObjectID(strID) );
			}
		}
		else
		{
			strErrMsg  = _T("Have no stand OR have no stand fit source stand -") + CString(tempData);
			delete pFlight;
			return NULL;
		}

	}
	//intermediate stand
	if( !_fAgentFile.getField( tempData , 1024 ) )	
	{
		//delete pFlight;
		//return NULL;
	}
	else
	{
		//dep stand
		ALTObjectID tempStandID( tempData );
		if( IsAValidStand( tempStandID ) )
		{
			char strID[255];
			tempStandID.printID(strID);
			//pFlight->setStand( ALTObjectID(strID) );
			if (pFlight->isDeparting())
			{
				pFlight->setIntermediateStand( ALTObjectID(strID) );
			}
		}
		else
		{
			strErrMsg  = _T("Have no stand OR have no stand fit source stand -") + CString(tempData);
			delete pFlight;
			return NULL;
		}

	}
	////////////////////////////////////////////////////////////////

	// service time
	//if( !_fAgentFile.getTime( tempTime ) )
	//{
	//	if( pFlight->isTurnaround() )
	//	{
	//		tempTime = pFlight->getDepTime()-pFlight->getArrTime();
	//		pFlight->setServiceTime( tempTime );
	//	}
	//	else
	//	{
	//		delete pFlight;
	//		return NULL;
	//	}
	//}
	//else
	//{
	//	if( pFlight->isTurnaround() )
	//	{
	//		tempTime = pFlight->getDepTime()-pFlight->getArrTime();
	//	}

	//	pFlight->setServiceTime( tempTime );
	//}
	if( !_fAgentFile.getTime( tempTime ) )
	{
		strErrMsg.Append(_T("Get Service time  Error!"));
		delete pFlight;
		return NULL;
	}
	else
	{
		if (tempTime < 0L)
		{
			strErrMsg.Append(_T("Flight Departure time earlier than Arrival time!"));
			delete pFlight;
			return NULL;					
		}

		pFlight->setServiceTime( tempTime );
	}
	// arr parking time
	if( !_fAgentFile.getTime( tempTime ) )
	{
		strErrMsg.Append(_T("Get Deplane Time Error!"));		
		delete pFlight;
		return NULL;
		
	}
	else
	{
		pFlight->setArrParkingTime( tempTime );
	}

	// dep parking time
	if( !_fAgentFile.getTime( tempTime ) )
	{
		strErrMsg.Append(_T("Get Enplane Time Error!"));		
		delete pFlight;
		return NULL;

	}
	else
	{
		pFlight->setDepParkingTime( tempTime );
	}

	// int parking time
	if( !_fAgentFile.getTime( tempTime ) )
	{
		strErrMsg.Append(_T("Get Intermediate parking Time Error!"));		
		delete pFlight;
		return NULL;

	}
	else
	{
		pFlight->setIntParkingTime( tempTime );
	}

	if (pFlight->isTurnaround()&& pFlight->getArrParkingTime() == 0L)
	{
		if (pFlight->getIntermediateStand().IsBlank())
			pFlight->setArrParkingTime(pFlight->getDepTime()-pFlight->getArrTime());
		else
		{
			pFlight->setArrParkingTime(pFlight->getDeplaneTime());
			pFlight->setDepParkingTime(pFlight->getEnplaneTime());
			pFlight->setIntParkingTime(pFlight->getDepTime()-pFlight->getArrTime() - pFlight->getDeplaneTime() - pFlight->getEnplaneTime());
		}
	}
	else if (pFlight->isArriving()&& pFlight->getArrParkingTime() == 0L)
		pFlight->setArrParkingTime(pFlight->getServiceTime());
	else if (pFlight->isDeparting()&& pFlight->getDepParkingTime() == 0L)
		pFlight->setDepParkingTime(pFlight->getServiceTime());				


	ProcessorID tempID;
	tempID.SetStrDict( m_pTerm->inStrDict );
	// arr gate
	if( !tempID.readProcessorID( _fAgentFile ) || !IsAValidGate( tempID ) )
	{
		strErrMsg.Append(_T("Get ProcessorID Error or The gate is not a valid gate!"));
		delete pFlight;
		return NULL;
	}
	else
	{
		/*if( tempID.isBlank() )
		{
			tempID = pFlight->getStand().getProcID(m_pTerm->inStrDict);
		}*/
		pFlight->setArrGate( tempID );
	}

	// dep gate
	if( !tempID.readProcessorID( _fAgentFile ) || !IsAValidGate( tempID ) )
	{
		strErrMsg.Append(_T("Get ProcessorID Error or The gate is not a valid gate!"));
		delete pFlight;
		return NULL;
	}
	else
	{
		/*if( tempID.isBlank() )
		{
			tempID = pFlight->getStand().getProcID(m_pTerm->inStrDict);
		}*/
		pFlight->setDepGate( tempID );
	}

	//baggage device
	if( !tempID.readProcessorID( _fAgentFile ) || !IsAValidBaggageDevice( tempID ) )
	{
		strErrMsg.Append(_T("Get ProcessorID Error or The baggage device is not a valid processor!"));
		delete pFlight;
		return NULL;
	}
	else
	{
		pFlight->setBaggageDevice( tempID );
	}

	//arr load factor
	double aValue = -1.0;
	_fAgentFile.getFloat(aValue);
	if (aValue > 0)
		pFlight->setArrLFInput(((double)aValue)/100);
	else
		_fAgentFile.skipField(1);

	//dep load factor
	_fAgentFile.getFloat(aValue);
	if (aValue > 0)
		pFlight->setDepLFInput(((double)aValue)/100);
	else
		_fAgentFile.skipField(1);

	//capacity
	int nValue = -1;
	_fAgentFile.getInteger(nValue);
	if (nValue > 0)
		pFlight->setCapacityInput(nValue);

	return pFlight;
}

bool CStringConvertToFlight::IsAValidGate( const ProcessorID& _procID )const
{
	ASSERT( m_pTerm );
	if( _procID.isBlank() )
		return true;
	GroupIndex groupIdx = m_pTerm->procList->getGroupIndex( _procID );	

	if( groupIdx.start < 0 )
		return false;
	if (groupIdx.start >= 0)
	{
		return m_pTerm->procList->getProcessor( groupIdx.start )->getProcessorType() == GateProc ;	
	}	
	return false;
}

bool CStringConvertToFlight::IsAValidStand( const ALTObjectID& _objID ) const
{
	if(_objID.IsBlank())
	{
		return true;
	}

	for(int i=0;i< (int)m_vObjectIDList.size();i++)
	{
		if(m_vObjectIDList.at(i).idFits(_objID))
			return true;
	}
	return false;
}

bool CStringConvertToFlight::IsAValidBaggageDevice( const ProcessorID& _procID ) const
{
	ASSERT( m_pTerm );
	if( _procID.isBlank() )
		return true;
	GroupIndex groupIdx = m_pTerm->procList->getGroupIndex( _procID );	

	if( groupIdx.start < 0 )
		return false;
	if (groupIdx.start >= 0)
	{
		return m_pTerm->procList->getProcessor( groupIdx.start )->getProcessorType() == BaggageProc ;	
	}	
	return false;
}