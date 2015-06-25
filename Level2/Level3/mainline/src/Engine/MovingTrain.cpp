#include "StdAfx.h"
#include "engine\terminal.h"
#include ".\Inputs\SingleCarSchedule.h"
#include ".\Results\TrainLogEntry.h"
#include ".\Inputs\AllCarSchedule.h"
#include ".\movingtrain.h"
#include "../Common/ARCTracker.h"

TrainCarInSim::TrainCarInSim(CCar* pCar)
:m_pCar(pCar)
{

}

TrainCarInSim::~TrainCarInSim()
{
	if (m_pCar)
	{
		delete m_pCar;
		m_pCar = NULL;
	}
}

void TrainCarInSim::AddPax(Person* pPerson, IntegratedStation *pDestStation)
{
	ASSERT(pPerson);
	m_vPerson.push_back( std::make_pair(pPerson, pDestStation));
}

void TrainCarInSim::DeletePax(Person* pPerson)
{
	//std::vector<Person*>::iterator iter = std::find(m_vPerson.begin(),m_vPerson.end(),pPerson);
	//if (iter != m_vPerson.end())
	//{
	//	m_vPerson.erase(iter);
	//}
	std::vector< std::pair< Person*, IntegratedStation *> >::iterator iter = m_vPerson.begin();
	for (; iter != m_vPerson.end(); ++ iter)
	{
		Person *curPerson = (*iter).first;
		if(curPerson && curPerson == pPerson)
		{
			m_vPerson.erase(iter);
			return;
		}
	}
}

int TrainCarInSim::GetPaxCount()const
{
	int nCount = 0;
	//for (unsigned i = 0; i < m_vPerson.size(); i++)
	//{
	//	Person* pPerson= m_vPerson.at(i);
	//	nCount += pPerson->GetActiveGroupSize();
	//}

	std::vector< std::pair< Person*, IntegratedStation *> >::const_iterator iter = m_vPerson.begin();
	for (; iter != m_vPerson.end(); ++ iter)
	{
		Person *curPerson = (*iter).first;
		if(curPerson)
		{
			nCount += curPerson->GetActiveGroupSize();
		}
	}
	return nCount;
}

bool TrainCarInSim::Valid()const
{
	if (m_pCar == NULL)
		return false;
		
	return GetPaxCount() < m_pCar->GetCarCapacity() ? true : false;
}

bool TrainCarInSim::Exist( Person* pPerson ) const
{
	//std::vector<Person*>::const_iterator iter = std::find(m_vPerson.begin(),m_vPerson.end(),pPerson);
	//if (iter != m_vPerson.end())
	//{
	//	return true;
	//}
	std::vector< std::pair< Person*, IntegratedStation *> >::const_iterator iter = m_vPerson.begin();
	for (; iter != m_vPerson.end(); ++ iter)
	{
		Person *curPerson = (*iter).first;
		if(curPerson && curPerson == pPerson)
		{
			return true;
		}
	}
	return false;
}

bool TrainCarInSim::ClearThisStationPerson(IntegratedStation *thisStation)
{
	int nPaxCount = m_vPerson.size();

	for (int nPax = nPaxCount -1; nPax >= 0; nPax --)
	{
		std::pair< Person*, IntegratedStation *>& personPair = m_vPerson[nPax];
		IntegratedStation *curStation = personPair.second;
		if(curStation == thisStation)
		{
			m_vPerson.erase(m_vPerson.begin() + nPax);
		}


	}

	//std::vector<  >::const_iterator iter = m_vPerson.begin();
	//for (; iter != m_vPerson.end(); ++ iter)
	//{
	//	Person *curPerson = (*iter).first;
	//	if(curPerson && curPerson == pPerson)
	//	{
	//		return true;
	//	}
	//}
	return true;
}

MovingTrain::MovingTrain(CTrainLogEntry& p_entry,Terminal* _pTerm):MobileElement(p_entry.GetID(),0)
,m_TrainLogEntry(p_entry)
,m_pTerm(_pTerm)
,m_nFlowIndex(0)
{
	state = Birth;
	m_iPreState = state;
	m_pSingTrainFlow = _pTerm->m_pAllCarSchedule->GetAllCarSchedule()->at(p_entry.GetIndexNum());
	m_TrainFlow = m_pSingTrainFlow->GetCarFlow();
	m_TrainLogEntry.SetEventLog(_pTerm->m_pTrainEventLog);
}

MovingTrain::~MovingTrain(void)
{
	for (int i = 0; i < (int)m_vTrainCar.size(); i++)
	{
		if (m_vTrainCar[i])
		{
			delete m_vTrainCar[i];
			m_vTrainCar[i] = NULL;
		}
	}
	m_vTrainCar.clear();
}

int MovingTrain::move(ElapsedTime currentTime,bool bNoLog)
{
	PLACE_METHOD_TRACK_STRING();
	switch(state)
	{
	case Birth:
		{
			InitTrain();
			state = TrainFreeMoving;
			generateEvent(currentTime,false);
		}
		break;
	case TrainFreeMoving:
		{
			if (m_iPreState == Birth)
			{
				GetSchedule(currentTime);
			}
			else
			{
				GetSchedule(currentTime - m_TrainLogEntry.GetTurnAroundTime());
			}

			int nCount = m_movePath.size();
			double dPrevHeading = 0.0;

			for( int i=0; i<nCount; i++ )
			{
				CViehcleMovePath movePath = m_movePath[i];
				int nPathCount = movePath.GetCount();
				float fPrevHeading;
				for( int z=0; z<nPathCount; z++ )
				{
					CStateTimePoint timePt = movePath.GetItem( z );
					TrainEventStruct event;
					ElapsedTime t;
					event.time = timePt.GetTime();
					t.setPrecisely(event.time);
					if (currentTime < t)
					{
						currentTime = t;
					}
					event.x = (float)timePt.getX();
					event.y = (float)timePt.getY();
					event.z = (short)timePt.getZ();
					if( z < nPathCount - 1 )
					{
						CStateTimePoint nextTimePt = movePath.GetItem( z+1 );
						double dNewHeading = ( nextTimePt - timePt ).getHeading();
						event.heading = (float)dNewHeading;
						fPrevHeading = event.heading;
					}
					else
					{
						event.heading = fPrevHeading;
					}
					event.state = timePt.GetState();
					m_TrainLogEntry.addEvent( event );
				}
			}
			ElapsedTime delayTime;
			delayTime.setPrecisely(01l); 

			generateEvent(currentTime - delayTime,false);//notify passenger train arrival
			m_iPreState = state;
			state = TrainArriveAtStation;
		}
		break;
	case TrainArriveAtStation:
		{
			if (m_nFlowIndex + 1 == (int)m_TrainFlow.size())
			{
				m_nFlowIndex = 0;
			}
			m_nFlowIndex++;
			IntegratedStation* pSourceStation = GetSourceStation(m_nFlowIndex);
			pSourceStation->SetTrain(this);
			ElapsedTime delayTime;
			delayTime.setPrecisely(01l); 
			generateEvent(currentTime + delayTime,false);//for correct time to leave
			state = TrainWaitForLeave;
		}
		break;
	case TrainWaitForLeave:
		{
			currentTime += m_TrainLogEntry.GetTurnAroundTime();
			generateEvent(currentTime,false);
			m_iPreState = state;
			state = TrainLeaveStation;
		}
		break;
	case TrainLeaveStation:
		{
			if (m_TrainLogEntry.GetEndTime() < currentTime)
			{
				generateEvent(currentTime,false);
				state = Death;
			}
			else
			{
				generateEvent(currentTime,false);
				m_iPreState = state;
				state = TrainFreeMoving;
			}
			IntegratedStation* pSourceStation = GetSourceStation(m_nFlowIndex);
			pSourceStation->ClearStationTrain(this);
			ClearThisStationPerson(pSourceStation);

		}
		break;
	case Death:
		{
			FlushLog();
		}
		break;
	default:
		break;
	}
	return TRUE;
}

bool MovingTrain::GetSchedule( ElapsedTime _time )
{
	if (m_pSingTrainFlow == NULL)
	{
		return false;
	}

	m_movePath.clear();
	IntegratedStation* pSourceStation = GetSourceStation(m_nFlowIndex);
	IntegratedStation* pDestStation = GetSourceStation(m_nFlowIndex + 1);

	return m_pSingTrainFlow->SelectBestSchedule(pSourceStation,pDestStation,_time,m_movePath);
}

IntegratedStation* MovingTrain::GetSourceStation(int nFlowIndex)
{
	if (nFlowIndex + 1 > (int)m_TrainFlow.size())
	{
		nFlowIndex = 1;
	}
	CARFLOW::iterator iter = m_TrainFlow.begin();
	for (int i = 0; i < (int)m_TrainFlow.size(); i++)
	{
		if (i == nFlowIndex)
		{
			return *iter;
		}
		iter++;
	}
	return NULL;
}

void MovingTrain::InitTrain()
{
	std::vector<TrainCarInSim*>::iterator iter = m_vTrainCar.begin();
	std::vector<TrainCarInSim*>::iterator iterEnd = m_vTrainCar.end();

	for( ; iter!=iterEnd ; ++iter )
		delete (*iter);
	m_vTrainCar.clear();

	CAR::iterator carIter = m_pSingTrainFlow->GetStationLayout()->GetAllCars().begin();
	CAR::iterator carIiterEnd = m_pSingTrainFlow->GetStationLayout()->GetAllCars().end();

	for(; carIter!=carIiterEnd; ++carIter)
	{
		CCar* pCar= new CCar ( **carIter );
		TrainCarInSim* pTrainCarInSim = new TrainCarInSim(pCar);
		m_vTrainCar.push_back(pTrainCarInSim);
	}
}

void MovingTrain::FlushLog()
{
	TrainEventStruct *log = NULL;
	int nEventCount = m_TrainLogEntry.getCurrentCount();
	if( nEventCount > 0 )
	{
		m_TrainLogEntry.getLog( log );
		m_TrainLogEntry.setEventList( log, nEventCount );
		delete [] log;
		log = NULL;
	}
	m_pTerm->m_pTrainLog->updateItem(m_TrainLogEntry,m_TrainLogEntry.GetTrainID());
	state = Death;
}

TrainCarInSim* MovingTrain::getCar( Person* pPerson )
{
	for (unsigned i = 0; i < m_vTrainCar.size(); i++)
	{
		TrainCarInSim* pTrainCarInSim = m_vTrainCar[i];
		if (pTrainCarInSim->Exist(pPerson))
		{
			return pTrainCarInSim;
		}
	}
	return NULL;
}

TrainCarInSim* MovingTrain::getCar( int nCarIndex )
{
	if(nCarIndex  < (int)m_vTrainCar.size() && nCarIndex >= 0)
	{
		return m_vTrainCar[nCarIndex];
	}
	
	return NULL;
}

bool MovingTrain::ClearThisStationPerson( IntegratedStation *thisStation )
{
	std::vector<TrainCarInSim*>::iterator iter = m_vTrainCar.begin();
	for (; iter != m_vTrainCar.end(); ++iter)
	{
		TrainCarInSim* pCar = *iter;
		if(pCar)
		{
			pCar->ClearThisStationPerson(thisStation);
		}
	}
	return true;

}
