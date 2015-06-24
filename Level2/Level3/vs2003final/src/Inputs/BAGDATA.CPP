/*****
*
*   Class       BaggageData
*   Author      Lincoln Fellingham
*   Date        May 1994
*   Purpose     Stores databases of information relating to baggage,
*               including baggage delay times and carosel assignments
*
*****/

#include "stdafx.h"
#include "inputs\bagdata.h"
#include "inputs\procdist.h"
#include "inputs\in_term.h"
#include "inputs\schedule.h"
#include "common\termfile.h"
#include "engine\process.h"
#include "engine\proclist.h"
#include "common\elaptime.h"
#include "BagGateAssign.h"
#include "common\ARCException.h"
#include "flight.h"
#include "ProcessorDistributionWithPipe.h"

BaggageData::BaggageData() : DataSet (BaggageDataFile,(float)2.5)
{
    bagLagTime.setUnits ("SECONDS");
    assignments.setUnits ("PROC");
}

// returns a random arrival time for a bag on the passed flight
ElapsedTime BaggageData::getBagLagTime (int p_ndx, FlightSchedule* _pFlightSchedule) const
{
	assert( m_pInTerm );
    Flight *aFlight = _pFlightSchedule->getItem (p_ndx);
    FlightConstraint type = aFlight->getType ('A');
    const ProbabilityDistribution *prob = bagLagTime.lookup (type);
    return (prob)? (ElapsedTime) prob->getRandomValue(): (ElapsedTime) 0l;
}


// reads baggage data from the passed file name in CSV
void BaggageData::readData (ArctermFile& p_file)
{
	assert( m_pInTerm );
	m_dbGateLagTime.ReadFromFile( p_file, m_pInTerm );
    bagLagTime.readDatabase (p_file, "LAGTIME", m_pInTerm );
    assignments.readDatabase (p_file, "ASSIGNMENT", m_pInTerm );
	
	
}

void BaggageData::readObsoleteData(ArctermFile& p_file)
{
	assert( m_pInTerm );
	float fVersion = p_file.getVersion();
	if( fVersion ==(float)2.3 )	// this is a error version
	{
		bagLagTime.readErrorDatabase( p_file, "LAGTIME", m_pInTerm);
		assignments.readErrorDatabase( p_file, "ASSIGNMENT", m_pInTerm );
	}
	else 
	{
		bagLagTime.readDatabase (p_file, "LAGTIME", m_pInTerm );
		assignments.readDatabase (p_file, "ASSIGNMENT", m_pInTerm );
	}
}
// writes baggage data to the passed file name in
void BaggageData::writeData (ArctermFile& p_file) const
{
	assert( m_pInTerm );
	m_dbGateLagTime.WriteToFile( p_file );
    bagLagTime.writeDatabase (p_file, "LAGTIME",m_pInTerm);
    assignments.writeDatabase (p_file, "ASSIGNMENT",m_pInTerm);
	
}



void BaggageData::BuildFlightAssignToBaggageDevice( FlightSchedule* _pFlightSchedule )
{
	ASSERT( m_pInTerm );
	int iFlightCount = _pFlightSchedule->getCount();

	UnsortedContainer<ProcessorID> idContainer;// = new ProcessorID[iFlightCount];
	UnsortedContainer<int> idDistribution;	
	SortedContainer<Flight> flightSortSchedule;
	_pFlightSchedule->sortFlights(flightSortSchedule);


	if( m_pInTerm->bagGateAssignDB->IfGateHasHigherPriority()  )//gate has higher priority 
	{
		for( int i=0; i<iFlightCount; ++i )
		{
			Flight* pFlight = flightSortSchedule.getItem(i);//m_pInTerm->flightSchedule->getItem( i );
			int iAssignedGate = pFlight->getStandID();
			ProcessorID gateProcID;
			gateProcID.SetStrDict( m_pInTerm->inStrDict);
			gateProcID.setID( pFlight->getStand().GetIDString() );
				
///			// TRACE("gate Procid", gateProcID.GetIDString() );
			
			if( (pFlight->isArriving() || pFlight->isTurnaround() )
				&& gateProcID.isBlank()== false )
			{								
				int index = m_pInTerm->bagGateAssignDB->findBestMatch (gateProcID);
				if( index >= 0 )
				{
					BagGateEntry *pBagEntry = (BagGateEntry*)m_pInTerm->bagGateAssignDB->getItem (index);

					const ProcessorID* pDestProcID = pBagEntry->getProbDist()->getDestinationGroup( RANDOM );
					ASSERT( pDestProcID );

//					// TRACE("\n%s\n",pDestProcID->GetIDString());
					Processor* pProc = m_pInTerm->procList->getProcessor( pDestProcID->GetIDString() );
					if( !pProc )// is a processor group name
					{
						int iIdx = 0;
						bool Tag =false;
						for(int j=0;j<idContainer.getCount();j++)
						{
							if(idContainer.getItem(j)==pDestProcID)
							{
								Tag = true;
								break;
								//	* idDistribution.getItem(j) +=1;
							}    
						}

						GroupIndex group =  m_pInTerm->procList->getGroupIndex (*pDestProcID);
						int iCount = group.end - group.start + 1;

						if(Tag)
						{
							iIdx = (*idDistribution.getItem(j) + iCount-1) % iCount ; // fast processor
							*idDistribution.getItem(j) = iIdx;
						}//random( iCount );
						else
						{
							idContainer.addItem((ProcessorID*)pDestProcID);
							idDistribution.addItem(new int(0));
						}
						
					//	int iIdx = random( iCount );
						pDestProcID = m_pInTerm->procList->getProcessor( group.start + iIdx )->getID();					
					}

					//add baggageDevice to Flight
					pFlight->setBaggageDevice(*pDestProcID);
					//

					FlightConstraint* pFltConstr = new FlightConstraint;
					*pFltConstr = pFlight->getType( 'A');
					ConstraintEntry* pEntry = new ConstraintEntry();
					ProcessorDistribution* pDist = new CProcessorDistributionWithPipe( true );
					double nPercent = 100;		
//					// TRACE("\n%s\n",pDestProcID->GetIDString());
					pDist->initDestinations( pDestProcID , &nPercent , 1 );

					pEntry->initialize( pFltConstr, pDist );		
					
					if( pFlight->isTurnaround() )
					{
						FlightConstraint tempDep = pFlight->getType( 'D');
						FlightConstraint* _pDepFltCon = new FlightConstraint;
						*_pDepFltCon =  tempDep;
						
						ConstraintEntry* pDetEntry = new ConstraintEntry();
						ProcessorDistribution* pDetDist = new CProcessorDistributionWithPipe( true );
						pDetDist->initDestinations( pDestProcID , &nPercent , 1 );						
						pDetEntry->initialize( _pDepFltCon, pDetDist );	
					}
				}
			}				
		}
	}
	else// flight has higher priority
	{

		//
	

		for( int i=0; i<iFlightCount; ++i )
		{
			Flight* pFlight = flightSortSchedule.getItem(i);//m_pInTerm->flightSchedule->getItem( i );
			if( pFlight->isArriving() || pFlight->isTurnaround() )
			{

				FlightConstraint tempFlight = pFlight->getType( 'A' );
				ProcessorDistribution* pAssignDistribution = (ProcessorDistribution* )assignments.lookup( tempFlight );
				if( pAssignDistribution )
				{
					FlightConstraint* pFltConstr = new FlightConstraint;
					*pFltConstr = tempFlight;

					ConstraintEntry* pEntry = new ConstraintEntry();
					ProcessorDistribution* pDist = new CProcessorDistributionWithPipe( true );
					
					const ProcessorID* pDestProcID = pAssignDistribution->getDestinationGroup( RANDOM );
					ASSERT( pDestProcID );

//					// TRACE("\n%s\n",pDestProcID->GetIDString());
					Processor* pProc = m_pInTerm->procList->getProcessor( pDestProcID->GetIDString() );
					if( !pProc )
					{
						bool Tag =false;
						for(int j=0;j<idContainer.getCount();j++)
						{
							if(idContainer.getItem(j)==pDestProcID)
							{
								Tag = true;
								break;
							//	* idDistribution.getItem(j) +=1;
							}    
						}
				


						GroupIndex group =  m_pInTerm->procList->getGroupIndex (*pDestProcID);
						if( group.start == -1 && group.end == -1 )
						{
							throw new ARCProcessNameInvalideError( pDestProcID->GetIDString(), " in baggage device" );
						}
						int iCount = group.end - group.start + 1;
                        int iIdx = 0;
						if(Tag)
						{
							iIdx = (*idDistribution.getItem(j) + iCount-1) % iCount ; // fast processor
							*idDistribution.getItem(j) = iIdx;
						}//random( iCount );
						else
						{
							idContainer.addItem((ProcessorID*)pDestProcID);
							idDistribution.addItem(new int(0));
						}
						pDestProcID = m_pInTerm->procList->getProcessor( group.start + iIdx )->getID();					
					}

					//add baggageDevice to Flight
					pFlight->setBaggageDevice(*pDestProcID);
					//

					double nPercent = 100;		
//					// TRACE("\n%s\n",pDestProcID->GetIDString());
					pDist->initDestinations( pDestProcID , &nPercent , 1 );

					pEntry->initialize( pFltConstr, pDist );		

					if( pFlight->isTurnaround() )
					{
						FlightConstraint tempDep = pFlight->getType( 'D');
						FlightConstraint* _pDepFltCon = new FlightConstraint;
						*_pDepFltCon =  tempDep;

						ConstraintEntry* pDetEntry = new ConstraintEntry();
						ProcessorDistribution* pDetDist = new CProcessorDistributionWithPipe( true );
						pDetDist->initDestinations( pDestProcID , &nPercent , 1 );						
						pDetEntry->initialize( _pDepFltCon, pDetDist );	
					}
					
				}
				
			}
		}
	}
	idContainer.clear();
	int iCount= idDistribution.getCount();
	for(int h=iCount-1;h>=0;h--)
		idDistribution.Destroy(h);
}

void BaggageData::InputFlightAssignBaggageDevice( FlightSchedule* _pFlightSchedule )
{
	ASSERT( m_pInTerm );
	int iFlightCount = _pFlightSchedule->getCount();
	for( int i=0; i<iFlightCount; ++i )
	{
		Flight* pFlight = _pFlightSchedule->getItem( i );

		if(pFlight->getBaggageDevice().isBlank()) 
			continue;

		FlightConstraint* pFltConstr = new FlightConstraint;

		if (pFlight->isArriving())
			*pFltConstr = pFlight->getType( 'A' );
		else
			*pFltConstr = pFlight->getType( 'D' );

		ConstraintEntry* pEntry = new ConstraintEntry();
		ProcessorDistribution* pDist = new CProcessorDistributionWithPipe( true );
		double nPercent = 100;		
		pDist->initDestinations(&pFlight->getBaggageDevice() , &nPercent , 1 );
		
		pEntry->initialize( pFltConstr, pDist );		
		
		if( pFlight->isTurnaround() )
		{
			FlightConstraint tempDep = pFlight->getType( 'D');
			FlightConstraint* _pDepFltCon = new FlightConstraint;
			*_pDepFltCon =  tempDep;
			
			ConstraintEntry* pDetEntry = new ConstraintEntry();
			ProcessorDistribution* pDetDist = new CProcessorDistributionWithPipe( true );
			pDetDist->initDestinations(&pFlight->getBaggageDevice() , &nPercent , 1 );						
			pDetEntry->initialize( _pDepFltCon, pDetDist );		
		}
	}


}