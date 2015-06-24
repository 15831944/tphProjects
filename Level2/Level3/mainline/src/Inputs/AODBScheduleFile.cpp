#include "StdAfx.h"
#include ".\aodbschedulefile.h"
#include "../common/csv_file.h"
#include "AODBConvertParameter.h"
#include "flight.h"
#include "..\Engine\terminal.h"
#include "common\AirportsManager.h"
#include "common\AirlineManager.h"
#include "common\ACTypesManager.h"

using namespace AODB;

//schedule item
AODB::ScheduleItem::ScheduleItem()
{
	m_pFlight = NULL;
}

AODB::ScheduleItem::~ScheduleItem()
{

}

bool AODB::ScheduleItem::valid()const
{
	if (!m_arrDate.IsValid() || !m_strAirline.IsValid() || !m_strArrID.IsValid() || !m_strArrIntStop.IsValid()
		||!m_strOrigin.IsValid() || !m_arrTime.IsValid() || !m_strDepID.IsValid() ||!m_strDepIntStop.IsValid() || !m_strDest.IsValid()
		|| !m_depTime.IsValid() || !m_depDate.IsValid() || !m_strACType.IsValid() || !m_arrStand.IsValid() || !m_depStand.IsValid()
		|| !m_intStand.IsValid() || !m_stayTime.IsValid() || !m_arrGate.IsValid() || !m_depGate.IsValid() || !m_bageDevice.IsValid()
		|| !m_dArrLoadFactor.IsValid() || !m_deDepLoadFactor.IsValid() || !m_nSeats.IsValid() ||!m_ataTime.IsValid() || !m_ataDay.IsValid()
		|| !m_atdTime.IsValid() || !m_atdDay.IsValid() || !m_towOffTime.IsValid() || !m_towOffDay.IsValid() || !m_towOffExTime.IsValid()
		|| !m_towOffExDay.IsValid())
	{
		return false;
	}

	return true;
}

void AODB::ScheduleItem::ReadData( CSVFile& csvFile )
{
	//ARR_DATE
	m_arrDate.GetValue(csvFile);

	//AIRLINE
	m_strAirline.GetValue(csvFile) ;

	//ARR_ID
	m_strArrID.GetValue(csvFile);

	//INT_STOP
	m_strArrIntStop.GetValue(csvFile);

	//ORIGIN
	m_strOrigin.GetValue(csvFile);

	//ARR_TIME
	m_arrTime.GetValue(csvFile);

	m_ataTime.GetValue(csvFile);//ATA
	m_ataDay.GetValue(csvFile);//ADA

	//DEP_ID
	m_strDepID.GetValue(csvFile);

	//INT_STOP_OUT
	m_strDepIntStop.GetValue(csvFile);

	//DEST
	m_strDest.GetValue(csvFile);

	//DEP_TIME
	m_depTime.GetValue(csvFile);

    //DEP_DATE	
	m_depDate.GetValue(csvFile);

	m_atdTime.GetValue(csvFile);//ATD
	m_atdDay.GetValue(csvFile);//ADD

	//AC_TYPE	
	m_strACType.GetValue(csvFile);

    //ARR_STAND	
	m_arrStand.GetValue(csvFile);

	m_towOffTime.GetValue(csvFile);//A tow off
	m_towOffDay.GetValue(csvFile);//D tow off

    //DEP_STAND
	m_depStand.GetValue(csvFile);

	//INT_STAND
	m_intStand.GetValue(csvFile);

	m_towOffExTime.GetValue(csvFile);//T ex tow off
	m_towOffExDay.GetValue(csvFile);//D ex tow off

	//STAY_TIME
	m_stayTime.GetValue(csvFile);

    //ARR_GATE
	m_arrGate.GetValue(csvFile);

    //DEP_GATE
	m_depGate.GetValue(csvFile);

	//CAROUSEL	
	m_bageDevice.GetValue(csvFile);

	//ARR_LOAD_FACTOR
	m_dArrLoadFactor.GetValue(csvFile);
	
	//DEP_LOAD_FACTOR
	m_deDepLoadFactor.GetValue(csvFile);
    	
	//AC_SEATS
	m_nSeats.GetValue(csvFile);


	//new version add fields

}

COleDateTime AODB::ScheduleItem::GetEearistDate()
{
	if(m_arrDate.getDate().GetStatus() == COleDateTime::valid)
	{
		if(m_depDate.getDate().GetStatus() == COleDateTime::valid)
		{
			if(m_arrDate.getDate() > m_depDate.getDate())
				return m_depDate.getDate();
		}

		return m_arrDate.getDate();

	}
	else
	{
		if(m_depDate.getDate().GetStatus() == COleDateTime::valid)
			return m_depDate.getDate();

		return m_arrDate.getDate();
	}

}

void AODB::ScheduleItem::Convert( ConvertParameter& pParameter )
{


	//ARR_DATE
	m_arrDate.Convert(pParameter);

	//AIRLINE
	m_strAirline.Convert(pParameter);

	//ARR_ID
	m_strArrID.Convert(pParameter);

	//INT_STOP
	m_strArrIntStop.Convert(pParameter);

	//ORIGIN
	m_strOrigin.Convert(pParameter) ;

	//ARR_TIME
	m_arrTime.Convert(pParameter);

	//DEP_ID
	m_strDepID.Convert(pParameter);

	//INT_STOP_OUT
	m_strDepIntStop.Convert(pParameter);

	//DEST
	m_strDest.Convert(pParameter);

	//DEP_TIME
	m_depTime.Convert(pParameter);

	//DEP_DATE	
	m_depDate.Convert(pParameter);

	//AC_TYPE	
	m_strACType.Convert(pParameter);

	//ARR_STAND	
	m_arrStand.Convert(pParameter);

	//DEP_STAND
	m_depStand.Convert(pParameter);

	//INT_STAND
	m_intStand.Convert(pParameter);

	//STAY_TIME
	m_stayTime.Convert(pParameter);

	//ARR_GATE
	m_arrGate.Convert(pParameter);

	//DEP_GATE
	m_depGate.Convert(pParameter);

	//CAROUSEL	
	m_bageDevice.Convert(pParameter);

	//ARR_LOAD_FACTOR
	m_dArrLoadFactor.Convert(pParameter);

	//DEP_LOAD_FACTOR
	m_deDepLoadFactor.Convert(pParameter);

	//AC_SEATS
	m_nSeats.Convert(pParameter);

	
	//new version add fields
	m_ataTime.Convert(pParameter);//ATA
	m_ataDay.Convert(pParameter);//ADA

	m_atdTime.Convert(pParameter);//ATD
	m_atdDay.Convert(pParameter);//ADD

	m_towOffTime.Convert(pParameter);//A tow off
	m_towOffDay.Convert(pParameter);//D tow off

	m_towOffExTime.Convert(pParameter);//T ex tow off
	m_towOffExDay.Convert(pParameter);//D ex tow off

	//check all information are valid

	//then generate a flight


	ConvertToFlight(pParameter);

}

AODB::MapField* AODB::ScheduleItem::GetMapField(ScheduleItemType type)
{
	switch (type)
	{
		//arr day
	case ARRIVAL_DAY_TYPE:
		{
			return &(m_arrDate);
		}
	case AIRLINE_TYPE:
		{
			return &(m_strAirline);
		}
	case ARRID_TYPE:
		{
			return &(m_strArrID);
		}
	case ARRINTSTOP_TYPE:
		{
			return &(m_strArrIntStop);
		}
	case ORIGIN_TYPE:
		{
			return &(m_strOrigin);
		}
	case ARRTIME_TYPE:
		{
			return &(m_arrTime);
		}
	case DEPID_TYPE:
		{
			return &(m_strDepID);
		}
	case DEPINTSTOP_TYPE:
		{
			return &(m_strDepIntStop);
		}
	case DEST_TYPE:
		{
			return &(m_strDest);
		}
	case DEPTIME_TYPE:
		{
			return &(m_depTime);
		}
	case DEPDAY_DAY_TYPE:
		{
			return &(m_depDate);
		}
	case ACTYPE_TYPE:
		{
			return &(m_strACType);
		}
	case ARRIVAL_STAND_TYPE:
		{
			return &(m_arrStand);
		}
	case DEP_STAND_TYPE:
		{
			return &(m_depStand);
		}
	case INT_STAND_TYPE:
		{
			return &(m_intStand);
		}	
	case STAY_TIME_TYPE:
		{
			return &(m_stayTime);
		}
	case ARRIVAL_GATE_TYPE:
		{
			return &(m_arrGate);
		}
	case DEP_GATE_TYPE:
		{
			return &(m_depGate);
		}
	case BAG_DEVICE_TYPE:
		{
			return &(m_bageDevice);
		}
	case ARRLOAD_FACTOR_TYPE:
		{
			return &(m_dArrLoadFactor);
		}
	case DEPLOAD_FACTOR_TYPE:
		{
			return &(m_deDepLoadFactor);
		}
	case SEAT_TYPE:
		{
			return &(m_nSeats);
		}
	//new version data
	case ATATIME_TYPE:
		{
			return &(m_ataTime);
		}
	case ADATIME_TYPE:
		{
			return &(m_ataDay);
		}
	case ATDTIME_TYPE:
		{
			return &(m_atdTime);
		}
	case ADDTIME_TYPE:
		{
			return &(m_atdDay);
		}
	case DTOWOFF_TYPE:
		{
			return &(m_towOffTime);
		}
	case TTOWOFF_TYPE:
		{
			return &(m_towOffDay);
		}
	case DEXINT_STAND_TYPE:
		{
			return &(m_towOffExTime);
		}
	case TEXINT_STAND_TYPE:
		{
			return &(m_towOffExDay);
		}
	}
	return NULL;
}
//check the item is valid or completely
bool AODB::ScheduleItem::ConvertToFlight(ConvertParameter& pParameter)
{
	bool bValid = true;
	
	//airline, flight number
	if(!IsCodeValid())
		bValid = false;

	//arrival, if all column is empty, it is OK
	bool bArrEmpty = false;
	if(m_strArrID.IsEmpty() &&
		m_strOrigin.IsEmpty() &&
		m_arrDate.IsEmpty() &&
		m_arrTime.IsEmpty() &&
		m_strArrIntStop.IsEmpty()&&
		m_ataTime.IsEmpty() &&
		m_ataDay.IsEmpty())
	{
		bArrEmpty = true;
	}
	else
	{
		if(!IsArrivalValid())
			bValid = false;
	}
	
	bool bDepEmpty = false;
	//departure part
	if(m_strDepID.IsEmpty() &&
		m_strDest.IsEmpty() &&
		m_depDate.IsEmpty() &&
		m_depTime.IsEmpty() &&
		m_strDepIntStop.IsEmpty() &&
		m_atdTime.IsEmpty() &&
		m_atdDay.IsEmpty())
	{
		bDepEmpty = true;
		if(bArrEmpty)
			bValid = false;//has no arrival and departure part

	}
	else
	{
		if(!IsDepartureValid())
			bValid = false;
	}

	//the other information
	
	if(!m_arrGate.IsEmpty() && !m_arrDate.IsValid())
		bValid = false;

	if(!m_depGate.IsEmpty() && !m_depGate.IsValid())
		bValid = false;

	if(!m_arrStand.IsEmpty() && !m_arrStand.IsValid())
		bValid = false;

	//T tow off
	if (!m_towOffTime.IsEmpty() && !m_towOffTime.IsValid())
		bValid = false;
	
	// D tow off
	if (!m_towOffDay.IsEmpty() && !m_towOffDay.IsValid())
		bValid = false;

	if(!m_depStand.IsEmpty() && !m_depStand.IsValid())
		bValid = false;

	if(!m_intStand.IsEmpty() && !m_intStand.IsValid())
		bValid = false;

	//T ex tow off
	if(!m_towOffExTime.IsEmpty() && !m_towOffExTime.IsValid())
		bValid = false;

	//D ex tow off
	if(!m_towOffExDay.IsEmpty() && !m_towOffExDay.IsValid())
		bValid = false;

	if(!m_bageDevice.IsEmpty() && !m_bageDevice.IsValid())
		bValid = false;


	//load factor
	if(m_dArrLoadFactor.IsEmpty() && !m_dArrLoadFactor.IsValid())
		bValid = false;

	if(m_deDepLoadFactor.IsEmpty() && !m_deDepLoadFactor.IsValid())
		bValid = false;

	if(m_nSeats.IsEmpty() && !m_nSeats.IsValid())
		bValid = false;

	delete m_pFlight;
	m_pFlight = NULL;

	if(bValid)
	{//the item is valid


		m_pFlight = new Flight( (Terminal*)pParameter.pTerminal);

		m_pFlight->setAirline(m_strAirline.GetMapValue());
		_g_GetActiveAirlineMan( pParameter.pTerminal->m_pAirportDB )->FindOrAddEntry( m_strAirline.GetMapValue());

		//arrival information
		if(!bArrEmpty)
		{//convert arrival
			
			//arrival id
			m_pFlight->setArrID(m_strArrID.GetMapValue());

			//origin
			m_pFlight->setAirport( m_strOrigin.GetMapValue() , 'A' );
			_g_GetActiveAirportMan( pParameter.pTerminal->m_pAirportDB )->FindOrAddEntry( m_strOrigin.GetMapValue() );


			//stop over airport
			if(!m_strArrIntStop.IsEmpty())
			{
				m_pFlight->setStopoverAirport( m_strArrIntStop.GetMapValue() , 'A' );
				_g_GetActiveAirportMan( pParameter.pTerminal->m_pAirportDB )->FindOrAddEntry( m_strArrIntStop.GetMapValue() );
			}

			//arrival time
			ElapsedTime eArrTime;
			int nDay = m_arrDate.GetDay();
			ASSERT(nDay >= 0);
			eArrTime.SetDay(nDay + 1);
			eArrTime += m_arrTime.GetTime();
			m_pFlight->setArrTime(eArrTime);

			ElapsedTime eAtaTime;
			nDay = m_ataDay.GetDay();
			ASSERT(nDay >= 0);
			eAtaTime.SetDay(nDay + 1);
			eAtaTime += m_ataTime.GetTime();
			m_pFlight->SetAtaTime(eAtaTime);
		}

		if(!bDepEmpty)
		{//convert departure

			//arrival id
			m_pFlight->setDepID(m_strDepID.GetMapValue());

			//origin
			m_pFlight->setAirport( m_strDest.GetMapValue() , 'D' );
			_g_GetActiveAirportMan( pParameter.pTerminal->m_pAirportDB )->FindOrAddEntry( m_strDest.GetMapValue() );

			//stop over airport
			if(!m_strDepIntStop.IsEmpty())
			{
				m_pFlight->setStopoverAirport( m_strDepIntStop.GetMapValue() , 'D' );
				_g_GetActiveAirportMan( pParameter.pTerminal->m_pAirportDB )->FindOrAddEntry( m_strDepIntStop.GetMapValue() );

			}

			//arrival time
			ElapsedTime eDepTime;
			int nDay = m_depDate.GetDay();
			ASSERT(nDay >= 0);
			eDepTime.SetDay(nDay + 1);
			eDepTime += m_depTime.GetTime();
			m_pFlight->setDepTime(eDepTime);

			ElapsedTime eAtdTime;
			nDay = m_atdDay.GetDay();
			ASSERT(nDay >= 0);
			eAtdTime.SetDay(nDay + 1);
			eAtdTime += m_atdTime.GetTime();
			m_pFlight->SetAtdTime(eAtdTime);
		}

		//aircraft type
		m_pFlight->setACType(m_strACType.GetMapValue());
		_g_GetActiveACMan( pParameter.pTerminal->m_pAirportDB )->FindOrAddEntry( m_strACType.GetMapValue() );


		//arrival stand
		m_pFlight->setArrStand(m_arrStand.getObjectID());

		//departure stand
		m_pFlight->setDepStand(m_depStand.getObjectID());

		//intermediates stand
		m_pFlight->setIntermediateStand(m_intStand.getObjectID());


		//arrival gate
		if(!m_arrGate.IsEmpty())
			m_pFlight->setArrGate(m_arrGate.getProcID());

		//departure gate
		if(!m_depGate.IsEmpty())
			m_pFlight->setDepGate(m_depGate.getProcID());

		//baggage device
		if(!m_bageDevice.IsEmpty())
			m_pFlight->setBaggageDevice(m_bageDevice.getProcID());
        

		//load factor
		if(!m_dArrLoadFactor.IsEmpty())
			m_pFlight->setArrLFInput(m_dArrLoadFactor.getDouble()/100 );

		if(!m_deDepLoadFactor.IsEmpty())
			m_pFlight->setDepLFInput(m_deDepLoadFactor.getDouble()/100 );

		//seats
		if(!m_nSeats.IsEmpty())
			m_pFlight->setCapacityInput(m_nSeats.getValue());

		//tow off 
		if (!m_towOffTime.IsEmpty()&& !m_towOffDay.IsEmpty())
		{
			ElapsedTime eTowoffTime;
			int nDay = m_towOffDay.GetDay();
			ASSERT(nDay >= 0);
			eTowoffTime.SetDay(nDay + 1);
			eTowoffTime += m_towOffTime.GetTime();
			m_pFlight->SetTowoffTime(eTowoffTime);
		}

		//Ex Int Stand
		if (!m_towOffExTime.IsEmpty() && !m_towOffExDay.IsEmpty())
		{
			ElapsedTime eExIntStandTime;
			int nDay = m_towOffExDay.GetDay();
			ASSERT(nDay >= 0);
			eExIntStandTime.SetDay(nDay + 1);
			eExIntStandTime += m_towOffExTime.GetTime();
			m_pFlight->SetExIntStandTime(eExIntStandTime);
		}
	}

	m_bSuccess = bValid;

	return bValid;
}

bool AODB::ScheduleItem::IsCodeValid() const
{
	if(m_strAirline.IsValid())//airline cannot be NULL
		return true;

	return false;
}

bool AODB::ScheduleItem::IsArrivalValid() const
{

	//arrival id
	if(m_strArrID.IsEmpty() || !m_strArrID.IsValid())
		return false;

	if(m_strOrigin.IsEmpty() || !m_strOrigin.IsValid())
		return false;

	if(m_arrDate.IsEmpty() || !m_arrDate.IsValid())
		return false;

	if(m_arrTime.IsEmpty() || !m_arrTime.IsValid())
		return false;


	//int stop is not necessary
	if(!m_strArrIntStop.IsEmpty() && !m_strArrIntStop.IsValid())
		return false;

	//ATA and ADA
	if(!m_ataTime.IsEmpty() && !m_ataTime.IsValid())
		return false;

	if(!m_ataDay.IsEmpty() && !m_ataDay.IsValid())
		return false;



	return true;
}

bool AODB::ScheduleItem::IsDepartureValid() const
{
	//arrival id
	if(m_strDepID.IsEmpty() || !m_strDepID.IsValid())
		return false;

	if(m_strDest.IsEmpty() || !m_strDest.IsValid())
		return false;

	if(m_depDate.IsEmpty() || !m_depDate.IsValid())
		return false;

	if(m_depTime.IsEmpty() || !m_depTime.IsValid())
		return false;


	//int stop is not necessary
	if(!m_strDepIntStop.IsEmpty() && !m_strDepIntStop.IsValid())
		return false;

	//ATD and ADD
	if(!m_atdTime.IsEmpty() && !m_atdTime.IsValid())
		return false;

	if(!m_atdDay.IsEmpty() && !m_atdDay.IsValid())
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////
//ScheduleFile
//ARR_DATE	AIRLINE	ARR_ID	INT_STOP	ORIGIN	ARR_TIME	DEP_ID	INT_STOP_OUT	DEST	DEP_TIME	DEP_DATE	AC_TYPE	ARR_STAND	DEP_STAND	INT_STAND	STAY_TIME	ARR_GATE	DEP_GATE	CAROUSEL	ARR_LOAD_FACTOR	DEP_LOAD_FACTOR	AC_SEATS

ScheduleFile::ScheduleFile(void)
{
}

ScheduleFile::~ScheduleFile(void)
{
}

void ScheduleFile::ReadData( CSVFile& csvFile )
{
	Clear();

	while(csvFile.getLine())
	{
		ScheduleItem* pItem = new ScheduleItem;
		pItem->ReadData(csvFile);

		AddData(pItem);
	}
}



void ScheduleFile::WriteData( CSVFile& csvFile )
{
	ASSERT(0);
}

ScheduleItem * AODB::ScheduleFile::GetItem(int nIndex)
{
	return (ScheduleItem *)GetData(nIndex);
}

COleDateTime AODB::ScheduleFile::GetStartDate(COleDateTime earliestDate)
{

	int nCount = GetCount();

	for (int nItem = 0; nItem < nCount; ++ nItem)
	{
		ScheduleItem *pItem = GetItem(nItem);

		if(pItem)
		{
			COleDateTime oleDate =  pItem ->GetEearistDate();
			if(oleDate.GetStatus() == COleDateTime::valid)
			{
				if(oleDate < earliestDate)
					earliestDate = oleDate;
			}
		}

	}
	return earliestDate;
}


//************************************
// Method:    Convert
// FullName:  AODB::ScheduleFile::Convert
// Access:    public 
// Returns:   void
// Qualifier: throw AODBException
// Parameter: ConvertParameter & pParameter
//************************************
void AODB::ScheduleFile::Convert( ConvertParameter& pParameter )
{
	int nCount = GetCount();

	for (int nItem = 0; nItem < nCount; ++ nItem)
	{
		ScheduleItem *pItem = GetItem(nItem);
		if(pItem)
			pItem->Convert(pParameter);
	}
}

bool AODB::ScheduleFile::ReplaceSchedule(InputTerminal *pTerminal)
{
	if(!IsReady())
		return false;

	int nCount = GetCount();

	std::vector< Flight *> vFlights;

	for (int nItem = 0; nItem < nCount; ++ nItem)
	{
		ScheduleItem *pItem = GetItem(nItem);
		if(pItem->m_pFlight)
		{
			vFlights.push_back(pItem->m_pFlight);
		}
	}

	pTerminal->flightSchedule->clear();
	for( int i=0; i<static_cast<int>(vFlights.size()); i++ )
	{
		pTerminal->flightSchedule->addItem( vFlights[i] );
	}

	return true;
}








