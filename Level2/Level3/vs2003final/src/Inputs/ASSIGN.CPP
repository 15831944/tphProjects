#include "stdafx.h"
#include <string.h>
#include "engine\proclist.h"
#include "inputs\assign.h"
#include "inputs\MobileElemConstraint.h"
#include "inputs\SCHEDULE.h"
#include "common\termfile.h"

/////////////////////////////////////////////////////////////////////////////////////
// read/write file
void ProcessorRoster::readAssignment (ArctermFile& p_file, InputTerminal* _pInTerm )
{
	p_file.getInteger( iFlag );
	p_file.getTime( openTime,true );
	p_file.getTime( closeTime ,true);
	assign.readConstraints( p_file, _pInTerm );


	absOpenTime = openTime;
	absCloseTime = closeTime;
	p_file.getInteger( (int&)assRelation );
	if(p_file.getVersion() >= 2.5L)
		p_file.getInteger(m_isDaily) ;
//	p_file.getInteger(m_bWithTimeConstraint);
}

void ProcessorRoster::writeAssignment (ArctermFile& p_file) const
{
	p_file.writeInt( iFlag );
	p_file.writeTime( openTime,true );
	p_file.writeTime( closeTime ,true);
	assign.writeConstraints(p_file);
	p_file.writeInt( assRelation );
	p_file.writeInt(m_isDaily) ;
//	p_file.writeInt(m_bWithTimeConstraint);
}

void ProcessorRoster::readObsoleteAssignment(ArctermFile& p_file, InputTerminal* _pInTerm )
{
	char str[32];
    p_file.getField (str, 32);
    iFlag  = !stricmp (str, "OPEN");
    p_file.getTime (openTime, 1);
	absOpenTime = openTime;
	absCloseTime = closeTime;
    assign.readConstraints (p_file, _pInTerm );
}


ProcessorRoster& ProcessorRoster::operator = ( const ProcessorRoster& p_entry)
{
	m_ID = p_entry.m_ID ;
	iFlag		= p_entry.iFlag;
	openTime	= p_entry.openTime;
	closeTime	= p_entry.closeTime;
	assign		= p_entry.assign;
	m_pInTerm   = p_entry.m_pInTerm;

	absOpenTime = p_entry.absOpenTime;
	absCloseTime = p_entry.absCloseTime;
	
	assRelation = p_entry.assRelation;
	this->m_isDaily = p_entry.m_isDaily ;
//	m_bWithTimeConstraint = p_entry.m_bWithTimeConstraint;

	return *this;
}

int ProcessorRoster::computeAbsoluteTime( InputTerminal* _pTerminal, FlightSchedule* _pFlightSchedule)
{
	m_pInTerm = _pTerminal;

	if( iFlag == 0 )	//iFlag = 0; closed
	{
		absOpenTime = openTime;
		absCloseTime = closeTime;
	}
	else if( iFlag == 1) 
	{
		absOpenTime = openTime;
		absCloseTime = closeTime;
	}
	else	//iFlag =2
	{	
		absOpenTime = 0l;
		absCloseTime = 24*60*60l;

		//first get flight info
		//then compute absolute time from flight time
		char szFlight[128];
		const CMobileElemConstraint* pMobElem = assign.getConstraint(0);
		pMobElem->getFullID(szFlight);
		//flt constraint mode transfer to flt mode
		char chFltMode = '0';
		if(pMobElem->GetFltConstraintMode() == ENUM_FLTCNSTR_MODE_ALL)
			chFltMode = 'T';
		else if(pMobElem->GetFltConstraintMode() == ENUM_FLTCNSTR_MODE_ARR)
			chFltMode = 'A';
		else
			chFltMode = 'D';

		int idxSchedule = _pFlightSchedule->getFlightIndex(szFlight, chFltMode);
		if(idxSchedule == -1)	//not find the flight!	the assignment invalidate
		{
			return -1;		// invalidate value;
		}
		else
		{
			Flight* pFlight = _pFlightSchedule->getItem( idxSchedule );
			if(pMobElem->GetFltConstraintMode() == ENUM_FLTCNSTR_MODE_ALL)//( pFlight->isTurnaround())
			{
				absOpenTime = pFlight->getArrTime() + openTime;
				absCloseTime = pFlight->getDepTime() + closeTime;
			}
			else if(pMobElem->GetFltConstraintMode() == ENUM_FLTCNSTR_MODE_ARR)//( pFlight->isArriving() )	//				pMobElem->getMode()=='A')
			{
				absOpenTime = pFlight->getArrTime() + openTime;
				absCloseTime = pFlight->getArrTime()+ closeTime;
			}
			else if(pMobElem->GetFltConstraintMode() == ENUM_FLTCNSTR_MODE_DEP)//( pFlight->isDeparting() ) //pMobElem->getMode()=='D')
			{
				absOpenTime = pFlight->getDepTime() + openTime;
				absCloseTime = pFlight->getDepTime()+ closeTime;
			}
			
		}
	}

	return iFlag;
}

bool ProcessorRoster::isDefault()
{
//	if( iFlag == 1 && openTime == 0l && closeTime == 3*24*60*60*100l )
	if( iFlag == 1 && openTime == 0l)
	{
		if( assign.getCount() == 1 && assign.getConstraint(0)->isDefault() )
			return true;
	}

	return false;
}

bool ProcessorRoster::isEqual(const ProcessorRoster& _entry )const
{
	return  iFlag == _entry.iFlag &&
		   	openTime == _entry.openTime &&
			closeTime == _entry.closeTime &&
			assign == _entry.assign && 
			absOpenTime == _entry.absOpenTime && 
			absCloseTime == _entry.absCloseTime &&
			assRelation == _entry.assRelation && m_isDaily == _entry.m_isDaily;
}

int ProcessorRoster::operator==( const ProcessorRoster& p_entry ) const
{
	return isEqual(p_entry);
}

int ProcessorRoster::operator<( const ProcessorRoster& p_entry ) const
{
	if ((absOpenTime == p_entry.absOpenTime) && (iFlag != p_entry.iFlag))
	{
		return iFlag == 0;
	}

	return absOpenTime < p_entry.absOpenTime;
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
// returns the current assignment
ProcessorRoster* ProcessorRosterSchedule::getProcRoster() const 
{ 
	if( current >= getCount() )
		return NULL;
	//{
	//	if(dailycurrent < (int)dailyData.size())
	//		return dailyData.getItem(dailycurrent) ;
	//	else
	//		return NULL ;
	//}
	return getItem(current); 
}

// returns the next assignment
ProcessorRoster* ProcessorRosterSchedule::getNextProcRoster() const 
{ 
	if( current+1 >= getCount() )
		return NULL;
	/*{
		if((dailycurrent + 1) < (int)dailyData.size())
		{
			ProcAssignment* proass = dailyData.getItem(dailycurrent + 1) ;
			const_cast<ProcAssignSchedule*>(this)->ToNext();
			return proass ;
		}
		else
			return NULL ;
	}*/
	return getItem(1+current); 
}

int ProcessorRosterSchedule::isOpen() const 
{ 
	ProcessorRoster* pAssign = getProcRoster();
	if( pAssign == NULL )
		return false;
#ifdef DEBUG
	int nCount = getCount();
#endif
	return pAssign->isOpen(); 
}
bool ProcessorRosterSchedule::ifExist( const ProcessorRoster& _assign )const
{
	for(int i=0; i<getCount() ;i++)
	{
		ProcessorRoster* pAss = getItem(i);
		if( pAss->isEqual( _assign ) )
		{
			return true;
		}
	}
	for (int i = 0 ; i< (int)dailyData.size() ;i++)
	{
		if (dailyData.getItem(i)->isEqual(_assign))
		{
			return true ;
		}
	}
	return false;
}
// returns the starting time of the current assignment
ElapsedTime ProcessorRosterSchedule::getCurrentTime() const 
{ 
	ProcessorRoster* pAssign = getProcRoster();
	if( pAssign == NULL )
		return ElapsedTime();
	return pAssign->getAbsOpenTime(); 
}


ProcessorRoster *ProcessorRosterSchedule::getAssignAtTime (ElapsedTime p_time, InputTerminal* _pInTerm ) const
{
    ProcessorRoster procAssign( _pInTerm );
    procAssign.setAbsOpenTime(p_time);

    int index = findItem (&procAssign);
    if (index == INT_MAX)
        //return NULL;
	{
		for (int i = 0 ; i< (int)dailyData.size() ;i++)
		{
			if (procAssign.isEqual(*dailyData.getItem(i)))
			{
				return dailyData.getItem(i) ;
			}
		}
		return NULL ;
	}
    return getItem (index);
}


// divide an event into an 'open' event and a 'close' event
void ProcessorRosterSchedule::divideEvents( InputTerminal* _pTerminal )
{	
	// item that will insert into procAssignSchedule
	std::vector<ProcessorRoster*> m_vectAddItem;
	ProcessorRoster *newAssign;

	for(int i=0; i<getCount() ;i++)
	{
		ProcessorRoster* pAss = getItem(i);
		//add 'close' events
		newAssign = new ProcessorRoster( _pTerminal );
		newAssign->setFlag(0);									//close
		newAssign->setAbsOpenTime( pAss->getAbsCloseTime() );
		newAssign->setAssignment( *(pAss->getAssignment()) );
		newAssign->setAssRelation( pAss->getAssRelation() );
		//add by hans 8/9
//		newAssign->setWithTimeConstraint( pAss->getWithTimeConstraint());

		//Add by adam 200711-29
		if (pAss->getAbsOpenTime().asSeconds() == 0
			&& pAss->getAbsCloseTime().asSeconds() == 0)
		{
			delete newAssign;
			newAssign = NULL;
			continue;
		}
		//End Add by adam 200711-29

		m_vectAddItem.push_back( newAssign );		
	}

	//push all item of m_vectAddItem into proAssignSchedule
	for( unsigned j=0; j<m_vectAddItem.size(); j++ )
	{
		addItem( m_vectAddItem[j] );
	}
    divideDailyEvents(_pTerminal);
}
void ProcessorRosterSchedule::AddDailyData( const ProcessorRoster& assigment ,int ndays) 
  {
	  
	  for(int i = 1 ; i < ndays ; i++)
	  {
		  ProcessorRoster* procAssi = new ProcessorRoster(assigment) ;
		  procAssi->setOpenTime(assigment.getOpenTime() + WholeDay*(long)i ) ;

		  procAssi->setCloseTime(assigment.getAbsCloseTime() + WholeDay*(long)i) ;

		  procAssi->setAbsOpenTime(assigment.getAbsOpenTime() + WholeDay*(long)i) ;

		  procAssi->setAbsCloseTime(assigment.getAbsCloseTime() + WholeDay*(long)i) ;

		  dailyData.addItem(procAssi) ;
	  }
   
  }
void ProcessorRosterSchedule::divideDailyEvents( InputTerminal* _pTerminal)
{
	std::vector<ProcessorRoster*> m_vectAddItem;
	for(int i=0; i<(int)dailyData.size() ;i++)
	{
		ProcessorRoster* newAssign = new ProcessorRoster(_pTerminal) ;
		//add 'open ' events
		newAssign->setFlag(1);									//open
		newAssign->setAbsOpenTime( dailyData.getItem(i)->getAbsOpenTime() );
		newAssign->setAssignment(  *(dailyData.getItem(i)->getAssignment()));
		newAssign->setAssRelation( dailyData.getItem(i)->getAssRelation() );
       m_vectAddItem.push_back( newAssign );

		//add 'close' events
	   newAssign = new ProcessorRoster(_pTerminal) ;
		newAssign->setFlag(0);									//close
		newAssign->setAbsOpenTime( dailyData.getItem(i)->getAbsCloseTime() );
		newAssign->setAssignment(  *(dailyData.getItem(i)->getAssignment()));
		newAssign->setAssRelation( dailyData.getItem(i)->getAssRelation() );
		//End Add by adam 200711-29

		m_vectAddItem.push_back( newAssign );		
	}

	//push all item of m_vectAddItem into proAssignSchedule
	for( unsigned j=0; j<m_vectAddItem.size(); j++ )
	{
		addItem( m_vectAddItem[j] );
	}
	
}
// Generate Events for _nDays - 1 days
void ProcessorRosterSchedule::GenerateEntryForMultipleDays( int _nDays,  InputTerminal* _pTerminal  )
{
	// item that will insert into procAssignSchedule
	std::vector<ProcessorRoster*> m_vectAddItem;
	ProcessorRoster *newAssign;

	for(int i=0; i<getCount() ;i++)
	{
		ProcessorRoster* pAss = getItem(i);
		for( int j=1; j<_nDays; j++ )
		{
			newAssign = new ProcessorRoster( _pTerminal );
			*newAssign = *pAss;
			newAssign->setAbsOpenTime( pAss->getAbsOpenTime() + WholeDay*(long)j );
			m_vectAddItem.push_back( newAssign );
		}
	}
    
	//push all item of m_vectAddItem into proAssignSchedule
	for( unsigned j=0; j<m_vectAddItem.size(); j++ )
	{
		addItem( m_vectAddItem[j] );
	}
}



// the processor is close or not
bool ProcessorRosterSchedule::processorIsClose()
{
	if( getCount()== 1 )
	{
		ProcessorRoster* pAssign = getItem( 0 );
		if( pAssign->isOpen() == 0 )
			return true;
	}

	return false;
}
void ProcessorRosterSchedule::RemoveProcAssignment(ProcessorRoster* _proAss)
{
	if(_proAss != NULL)
	{
		removeItem(_proAss) ;
		m_delProcAss.Add(_proAss) ;
	}
}
void ProcessorRosterSchedule::RemoveAllProcAssignment()
{
	int iCount = getCount();
	for( int i=iCount -1 ; i>=0; --i )
	{
		m_delProcAss.Add(getItem(i)) ;
		removeItem(i) ;
	}
}
void ProcessorRosterSchedule::ClearAllAssignment()
{
	int iCount = getCount();
	for( int i=iCount -1 ; i>=0; --i )
	{
		deleteItem( i );
	}


    while(m_delProcAss.getCount() > 0)
	{
		m_delProcAss.deleteItem(0);
	}
	
	ClearAllDailyData() ;
	//clear();
}
void ProcessorRosterSchedule::ClearAllDailyData() 
{
	while(dailyData.getCount() > 0)
	{
		dailyData.deleteItem(0);
	}

}
void ProcessorRosterSchedule::ResetDailyData(int ndays)
{
	ClearAllDailyData() ;
    for (int i = 0 ; i < getCount() ; i++)
    {
		if(getItem(i)->getAbsOpenTime().asSeconds() == 0 && getItem(i)->getAbsCloseTime().asSeconds() == 0)
			continue ;
		if(getItem(i)->IsDaily())
		    AddDailyData(*getItem(i),ndays) ;
    }
}
ProcessorRoster* ProcessorRosterSchedule::GetNearestOpenAssignment(ElapsedTime _CureentTime , const CMobileElemConstraint& _MobileEletype)
{
	ProcessorRoster* PProAssign = NULL ;
	for (int i = 0 ; i < getCount() ;i++)
	{
		PProAssign = getItem(i) ;
		if(!PProAssign->isOpen())
			continue ;
		if(_CureentTime <= PProAssign->getAbsOpenTime() && PProAssign->getAssignment()->isMatched(_MobileEletype))
			return PProAssign ;
	}
	return NULL ;
}
BOOL ProcessorRosterSchedule::CheckIsOpenAtThisTime(ElapsedTime _currentTime ,ProcessorRoster** _CurrentAssign)
{
	ProcessorRoster* ProcAssign = NULL ;
	for (int i = 0 ; i < getCount() ; i++)
	{
		ProcAssign = getItem(i) ;

		if(ProcAssign->isOpen())
		{
			if(ProcAssign->getAbsOpenTime() <= _currentTime && ProcAssign->getAbsCloseTime() >= _currentTime)//this time processor is open 
			{
				*_CurrentAssign = ProcAssign ;
				return TRUE ;

			}
		}
	}
	if(getCount() == 1) //has no assign,the processor open always
		return TRUE ;
	//current time processor is closed.
	return FALSE ;
}

bool ProcessorRosterSchedule::IsTimeRangeAvailable( const ElapsedTime& tStart, const ElapsedTime& tEnd, bool bExclusive )
{
	int nCount = getCount();

	if (nCount ==0)
		return true;

	ProcessorRoster* pRoster = getItem(0);
	if (tStart < pRoster->getAbsOpenTime() && tEnd <= pRoster->getAbsOpenTime())	//open before the first and time not overlap
		return true;

	std::vector<ProcessorRoster*> vOverlapRosters;
	vOverlapRosters.clear();
	for (int i =0; i < nCount; i++)
	{
		pRoster = getItem(i);

		if (tStart <= pRoster->getAbsOpenTime() && (tEnd > pRoster->getAbsOpenTime() && tEnd <= pRoster->getAbsCloseTime()))	//intersect
			vOverlapRosters.push_back(pRoster);

		if (tStart <= pRoster->getAbsOpenTime() && tEnd >= pRoster->getAbsCloseTime())		//include
			vOverlapRosters.push_back(pRoster);

		if (tStart >= pRoster->getAbsOpenTime() && tEnd <= pRoster->getAbsCloseTime())		//be included
			vOverlapRosters.push_back(pRoster);

		if ((tStart >= pRoster->getAbsOpenTime() && tStart < pRoster->getAbsCloseTime()) && tEnd >= pRoster->getAbsCloseTime())	//intersect
			vOverlapRosters.push_back(pRoster);

		if (tEnd <= pRoster->getAbsOpenTime())
			break;

	}

	size_t nSize = vOverlapRosters.size();

	if (nSize >0 && bExclusive)
		return false;

	for(size_t i =0; i < nSize; i++)
	{
		pRoster = vOverlapRosters.at(i);

		if (pRoster->getAssRelation() == Exclusive)
			return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
void UnmergedAssignments::split (ProcessorRosterSchedule *p_schedule)
{
    clear();
    const CMultiMobConstraint *multiConst= NULL;
	CMultiMobConstraint *tConst = NULL;
    ProcessorRoster *origAssign, *newAssign;
    int constCount, count = p_schedule->getCount();

	assert( m_pInTerm );
    for (int i = 0; i < count; i++)
    {
        origAssign = p_schedule->getItem (i);
        multiConst = origAssign->getAssignment();
        constCount = multiConst->getCount();
        if (constCount == 1)
            addItem (new ProcessorRoster (*origAssign));
        else for (int j = 0; j < constCount; j++)
        {
            // constructor sets up DEFAULT
            newAssign = new ProcessorRoster( m_pInTerm );
            newAssign->setFlag(origAssign->isOpen());
            newAssign->setOpenTime (origAssign->getOpenTime());
			newAssign->setCloseTime(origAssign->getCloseTime());
			newAssign->setAbsOpenTime(origAssign->getAbsOpenTime());
			newAssign->setAbsCloseTime(origAssign->getAbsCloseTime());
			newAssign->IsDaily(origAssign->IsDaily()) ;
            tConst = newAssign->getAssignment();
            tConst->setConstraint (*(multiConst->getConstraint (j)), 0);
            addItem (newAssign);
        }
    }
}

void UnmergedAssignments::merge (ProcessorRosterSchedule *p_schedule )
{
    p_schedule->clear();
    const CMultiMobConstraint *multiConst = NULL;
    ProcessorRoster *origAssign;//, *newAssign;
    int count = getCount();

	//assert( m_pInTerm );
    for (int i = 0; i < count; i++)
    {
        origAssign = getItem (i);
		/*
		newAssign = p_schedule->getAssignAtTime (origAssign->getOpenTime(), m_pInTerm);   
		if ( newAssign && newAssign->getCloseTime() == origAssign->getCloseTime()
			&& newAssign->isOpen() == origAssign->isOpen() )
        {
            multiConst = origAssign->getAssignment();
            newAssign->addAssignment (*(multiConst->getConstraint (0)));
        }
		*/
        //else
            p_schedule->addItem (new ProcessorRoster (*origAssign));
    }
}


void UnmergedAssignments::readData (const char *p_filename )
{
    clear();
    ProcessorRoster *procAssign;

    ArctermFile file;
    file.openFile (p_filename, READ);
	assert( m_pInTerm );
    while (file.getLine())
    {
        procAssign = new ProcessorRoster( m_pInTerm );
        procAssign->readAssignment (file, m_pInTerm );
        addItem (procAssign);
    }
    file.closeIn();
}

void UnmergedAssignments::writeData (const char *p_filename) const
{
    ProcessorRoster *procAssign;
    ArctermFile file;
    file.openFile (p_filename, WRITE);

    file.writeField ("Temporary Swap File\nHeaders");
    file.writeLine();

    for (int i = 0; i < getCount(); i++)
    {
        procAssign = getItem (i);
        procAssign->writeAssignment (file);
        file.writeLine();
    }
    file.endFile();
}

