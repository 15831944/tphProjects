#include "stdafx.h"
#include "common\elaptime.h"
#include "inputs\pax_cnst.h"
#include "inputs\assigndb.h"
#include "inputs\assign.h"
#include "inputs\in_term.h"
#include "engine\process.h"
#include "engine\proclist.h"
#include "engine\assevent.h"
#include "common\termfile.h"
#include "engine\hold.h"
#include "engine\lineproc.h"
#include "../Inputs/schedule.h"
#include "../Common/StartDate.h"
#include "../Database/ADODatabase.h"
#include "../Common/ARCTracker.h"
#include "Engine/DEP_SINK.H"

#include "PROCIDList.h"

ProcAssignEntry::ProcAssignEntry (const ProcAssignEntry& entry)
{ 	
    schedule = new ProcessorRosterSchedule; 
	*schedule = * entry.schedule;
}

const ProcAssignEntry& ProcAssignEntry::operator = ( const ProcAssignEntry& _anotherInstance )
{
	if( this != &_anotherInstance )
	{
		*schedule = * _anotherInstance.schedule;
	}
	return *this;
}
ProcAssignEntry::~ProcAssignEntry ()
{
	if( schedule )
	{
		delete schedule;
		schedule = NULL;
	}
}
void ProcAssignEntry::updateProcessors ( ProcessorList* _procList )
{
    schedule->updateAssignment();
    Processor *aProc;
	
	// TRACE( "==%s==\n", procID.GetIDString() );
    GroupIndex group = _procList->getGroupIndex (procID);

	if(group.start==-1)
		return;
    for (int i = group.start; i <= group.end; i++)
    {
        aProc = _procList->getProcessor (i);
		//CString strName = aProc->getID()->GetIDString();

		//TRACE(aProc->getID()->GetIDString());
        if (aProc->getSchedule() == schedule)
		{
            if (schedule->isOpen())
			{
				aProc->open();
				// to notify all waiting area which linked with aProc to release pax 
				std::vector<Processor*>vLinkedProc;
				_procList->GetLinkedWaitAreas( aProc,vLinkedProc );
				int iCount = vLinkedProc.size();
				for( int j=0; j<iCount; ++j )
				{
					Processor* pWaitArea = vLinkedProc[ j ];
					if( pWaitArea->getProcessorType() == HoldAreaProc )
					{						
						((HoldingArea*)pWaitArea)->releaseWaitInAreaPax(schedule->getCurrentTime(), aProc );
					}
					else if( pWaitArea->getProcessorType() == LineProc )
					{
						((LineProcessor*)pWaitArea)->releaseWaitInAreaPax( schedule->getCurrentTime(), aProc );
					}
				}
				//check if sink, need to wake up the pax in the 
				if(aProc->getProcessorType() == DepSinkProc)
				{
					DependentSink *pSink = (DependentSink *)aProc;
					if(pSink)
						pSink->WakeupPaxWaitingOnDependSource(schedule->getCurrentTime());
				}
		

			}
		
			else
				aProc->close();
		}
    }
}

// adds a ProcessorDataElement to the list if it does not already exist
void ProcAssignDatabase::addEntry (const ProcessorID& pID)
{
    ProcAssignEntry *pEntry = (ProcAssignEntry *)FindEntry (pID);
	if (!pEntry)
        addItem ((new ProcAssignEntry (pID)));
}
void ProcAssignDatabase::initDaily(const ProcessorID& pID,ProcessorRoster& procAssign)
{
    int index = findEntry(pID) ;
	ProcAssignEntry *pEntry = (ProcAssignEntry*)getItem (index);
	ProcessorRosterSchedule *sched = pEntry->getSchedule();
	int n_dex = sched->Find(&procAssign) ; 
    ASSERT(n_dex!= INT_MAX);
    ProcessorRoster* Ass = sched->getItem(n_dex) ;
	int days = m_pInTerm->flightSchedule->GetFlightScheduleEndTime().GetDay() ;
	if(Ass->IsDaily())
	{	
		if(procAssign.getOpenTime() >= WholeDay || procAssign.getCloseTime() >= WholeDay)
		{
			Ass->IsDaily(FALSE) ;
			return ;
		}
		sched->AddDailyData(procAssign,days);
	}
}
void ProcAssignDatabase::insert (const ProcessorID& pID,
    ProcessorRoster& procAssign)
{
    addEntry (pID);
    int index = findEntry (pID);
    ProcAssignEntry *pEntry = (ProcAssignEntry*)getItem (index);
    ProcessorRosterSchedule *sched = pEntry->getSchedule();
    sched->addItem (new ProcessorRoster (procAssign));
}

// schedules the first assignment change for each processor group
void ProcAssignDatabase::initEvents () const
{
	PLACE_METHOD_TRACK_STRING();
    ProcAssignEntry *entry;
    ProcessorAssignmentEvent *event;
    int entryCount = getCount();
    for (int i = 0; i < entryCount; i++)
    {
        entry = (ProcAssignEntry *)getItem(i);
        event = new ProcessorAssignmentEvent;
        event->init (entry);
    }
}

//transfer old format to new format
void ProcAssignDatabase::convertOldFormatToNew()
{
	ProcessorRoster* pAss = NULL;
	ProcessorRoster* pNextAss = NULL;
	ElapsedTime timeEnd( long(24*60*60-1) );
	
	for( int i=0; i<getCount(); i++ )
	{
		ProcessorRosterSchedule* pSchd = getDatabase(i);
		//set every events close time
		for( int j=0; j<pSchd->getCount()-1; j++ )
		{
			pAss = pSchd->getItem( j );
			pNextAss = pSchd->getItem( j+1 );
			pAss->setCloseTime( pNextAss->getOpenTime() );
		}
		pAss = pSchd->getItem(pSchd->getCount()-1);
		pAss->setCloseTime(timeEnd);
		// remove 'close' state events
		for(  j = pSchd->getCount()-1; j>=0; j-- )
		{
			pAss = pSchd->getItem( j );
			if( !pAss->isOpen() )
				pSchd->deleteItem(j);
		}
	}
}


void ProcAssignDatabase::readData (ArctermFile& p_file)
{
	//if(p_file.getVersion() > (float)2.5)
	//{
 //      ReadProcAssigmentFromDB() ;
	//   return ;
	//}
	assert( m_pInTerm );
	ProcAssignEntry *entry = NULL; 
	for (int i= 0 ; i< getCount() ; i++)
	{
		entry = (ProcAssignEntry*)getItem(i) ;
		entry->getSchedule()->ClearAllDailyData() ;
	}
	float fVersion  = p_file.getVersion();
    clear();
    ProcessorIDList idList;
	idList.ownsElements( 1 );//cause idList.readIDList (p_file, m_pInTerm ); will alloc memory 
    ProcessorRoster procAssign( m_pInTerm );

   while (p_file.getLine())
    {
        idList.readIDList (p_file, m_pInTerm );
		if( fVersion< (float)2.3 )
			procAssign.readObsoleteAssignment( p_file, m_pInTerm );
		else
			procAssign.readAssignment (p_file, m_pInTerm );

        for (int i = 0; i < idList.getCount(); i++)
		{
            insert (*(idList.getID (i)), procAssign);
			//added by cj ,for daily event .
			initDaily(*(idList.getID (i)), procAssign) ;
		}
    }
    buildHierarchy();
	if( fVersion < (float)2.3)
	{ 
		convertOldFormatToNew();
	}
	if( fVersion < (float)2.4 )
		m_bNeedRepeatEntry = true;
}

void ProcAssignDatabase::writeData (ArctermFile& p_file) const
{
	/*if(p_file.getVersion() > (float)2.5)
	{
	 const_cast<ProcAssignDatabase*>(this)->WriteProAssigmentToDB() ;
		return ;
	}*/
    char procName[256];
    const ProcessorID *procID;
    ProcessorRosterSchedule *database;
   ProcessorRoster* pro_ass = NULL ;
    int entryCount = getCount();
    for (int i = 0; i < entryCount; i++)
    {
        procID = getProcName(i);
        procID->printID (procName);
        Processor* pro = m_pInTerm->GetProcessorList()->getProcessor(*procID) ;
        database = getDatabase(i);
        for (int j = 0; j < database->getCount(); j++)
        {
			pro_ass = database->getItem(j) ;
		//	if( (pro->IsDaily()&&pro_ass->getOpenTime()< WholeDay&&pro_ass->getCloseTime()<WholeDay) || !pro->IsDaily())
		//	{
				p_file.writeField (procName);
				pro_ass->writeAssignment (p_file);
				p_file.writeLine();
		//	}
        }
    }
}

// delete all references to the passed index at p_level
void ProcAssignDatabase::deletePaxType (int p_level, int p_index)
{
    ProcessorRoster *assign;
    ProcessorRosterSchedule *database;

    for (int i = getCount()-1; i >= 0; i--)
    {
        database = getDatabase(i);
        for (int j = database->getCount()-1; j >= 0; j--)
        {
            assign = database->getItem (j);
            if (assign->contains (p_level, p_index))
                database->deleteItem (j);
        }
        if (!database->getCount())
        {
            delete database;
            deleteItem (i);
        }
    }
}


// from relative time to absolute time
int ProcAssignDatabase::computeAbsolutTime( InputTerminal* _pTerminal, FlightSchedule* _pFlightSchedule )
{
	PLACE_METHOD_TRACK_STRING();
	int iDelete = 0;
	ProcessorRosterSchedule *database;

	for( int i =0; i<getCount(); i++ )
	{
		database = getDatabase(i);
		for( int j=database->getCount()-1; j>=0; j-- )
		{
			if( (database->getItem(j))->computeAbsoluteTime( _pTerminal, _pFlightSchedule ) == -1)	//can not change from relative time to absolution time
			{
#ifdef DEBUG
				int _nCount_0 = database->getCount();
#endif
				database->deleteItem(j);
				iDelete++;
			}
		}

	}

	return iDelete;
}

// divide an event into an 'opne' event and a 'close' event
void ProcAssignDatabase::divideEvents( InputTerminal* _pTerminal )
{
	ProcessorRosterSchedule* database;

	int nCount = getCount();
	for( int i=0; i< nCount; i++ )
	{
		database = getDatabase( i );
		CString strProcID = ((ProcAssignEntry *)getItem(i))->getID()->GetIDString();

		database->divideEvents( _pTerminal );
	}
}

// Generate Events for _nDays - 1 days
void ProcAssignDatabase::GenerateEntryForMultipleDays( int _nDays, InputTerminal* _pTerminal  )
{
	PLACE_METHOD_TRACK_STRING();
	if( !m_bNeedRepeatEntry )
		return;

	ProcessorRosterSchedule* database;

	for( int i=0; i<getCount(); i++ )
	{
		database = getDatabase( i );
		database->GenerateEntryForMultipleDays( _nDays, _pTerminal );
	}
}

// create default event for processor which have no user's event
int  ProcAssignDatabase::createDefaultEvent()
{
	PLACE_METHOD_TRACK_STRING();
	int iCreateNum = 0;
	// init <DEFAULT> pax type
	CMultiMobConstraint multiMob;
	multiMob.initDefault( m_pInTerm );
	UnmergedAssignments _unMergedAssgn;

	long lAbsoluteTime = 0L;

	//Modify by adam 2007-05-30
	//if(m_pInTerm->flightSchedule->GetStartDate().IsAbsoluteDate())
	//{
	//	COleDateTimeSpan span = m_pInTerm->flightSchedule->GetEndDate().GetDate() - m_pInTerm->flightSchedule->GetStartDate().GetDate();
	//	int nHours =(int) span.GetTotalHours();
	//	nHours = nHours < 24 ? 24 : nHours+24;
	//	lAbsoluteTime = nHours *60*60 -1;
	//}
	//else
	//{
	//	lAbsoluteTime = 3*24*60*60-1;
	//}

	COleDateTimeSpan span = m_pInTerm->flightSchedule->GetEndDate().GetDate() - m_pInTerm->flightSchedule->GetStartDate().GetDate();
	int nHours =(int) span.GetTotalHours();
	nHours = nHours < 24 ? 24 : nHours+24;
	lAbsoluteTime = nHours *60*60 -1;
	//End Modify by adam 2007-05-30
	
	// for each processor
	int iProcCount = m_pInTerm->procList->getProcessorCount();
	for( int i=0; i<iProcCount; i++ )
	{
		ProcessorID procID = *(m_pInTerm->procList->getProcessor(i)->getID());
		if( findBestMatch( procID ) == -1 )	// user never define it
		{
			// create default event
			ElapsedTime time;
			ProcessorRoster* pProcAssn = new ProcessorRoster( m_pInTerm );
			pProcAssn->setFlag(1);
			time.set(0,0,0);
			pProcAssn->setOpenTime( time  );
			//time.set(lAbsoluteTime);  //delete by adam 2007-11-29
			pProcAssn->setCloseTime( time );
			pProcAssn->setAssignment( multiMob );
			// add event to db
			addEntry( procID );	
			int nIdx = findEntry( procID );
			assert( nIdx != INT_MAX );
			ProcessorRosterSchedule* pSchd = getDatabase( nIdx );
			
			_unMergedAssgn.clear();
			_unMergedAssgn.addItem( pProcAssn );
			_unMergedAssgn.merge( pSchd );
			
			iCreateNum++;
		}
	}
	
	return iCreateNum;
}

bool ProcAssignDatabase::getUsedProInfo(const ProcessorID& _id,InputTerminal *_pTerm,std::vector<CString>& _strVec)
{
	bool nRet = false;
	
    const ProcessorID *procID;
    ProcessorRosterSchedule *database;
	ProcessorRoster *assign;
	
    int entryCount = getCount();
    for (int i = 0; i < entryCount; i++)
    {
        procID = getProcName(i); 
        database = getDatabase(i);
		if (_id.idFits (*procID))
		{
			for (int j = 0; j < database->getCount(); j++)
			{
				CString str=procID->GetIDString();
				char pcMob[1024];
				assign =database->getItem(j);
				CMultiMobConstraint *multiMob =assign->getAssignment();
                multiMob->getConstraint(multiMob->getCount()-1)->screenPrint(pcMob,0,1024);
				CString stime ="[" + assign->getOpenTime().printTime(0);
				stime += " - ";
				stime += assign->getCloseTime().printTime(0);
				stime += "]";
                
				str+=" : ";
				str+=pcMob;
				str += ",";
				str += stime;
				str += " ;";
				_strVec.push_back(str);
			}

			nRet =true;
		}
		else if(procID->idFits(_id) && !procID->isBlank())
		{
			if(!_pTerm->HasBrother(_id,*procID))
			{
				for (int j = 0; j < database->getCount(); j++)
				{
					CString str=procID->GetIDString();
					char pcMob[1024];
					assign =database->getItem(j);
					CMultiMobConstraint *multiMob =assign->getAssignment();
					multiMob->getConstraint(multiMob->getCount()-1)->screenPrint(pcMob,0,1024);
					CString stime ="[" + assign->getOpenTime().printTime(0);
					stime += " - ";
					stime += assign->getCloseTime().printTime(0);
					stime += "]";
					
					str+=" : ";
					str+=pcMob;
					str += ",";
					str += stime;
					str += " ;";
					_strVec.push_back(str);
				}
				nRet = true;
			}
		}
    } 
	return nRet;
}
void ProcAssignDatabase::WriteProAssignment(CString prostr ,ProcessorRoster& _proAss)
{
	CString strsql ;
	CString paxty ;
	_proAss.getAssignment()->getConstraint(0)->screenPrint(paxty) ;
	if(_proAss.GetID() == -1)
		strsql.Format(_T("INSERT INTO TB_PROCASSIGMENT_ROSTER (FLAG, OPENTIME,CLOSETIME,PAXTY,ISDAILY,RELATION ,PROCESSORID) \
						 VALUES (%d, '%s','%s' ,'%s',%d,%d,'%s')") ,
						 _proAss.getFlag(),
						 _proAss.getAbsOpenTime().printTime(),
						 _proAss.getAbsCloseTime().printTime(),
						 paxty,
						 _proAss.IsDaily(),
						 _proAss.getAssRelation(),
						 prostr) ;
	else
		strsql.Format(_T("UPDATE TB_PROCASSIGMENT_ROSTER SET FLAG =%d',OPENTIME = '%s',CLOSETIME = '%s',PAXTY = '%s',ISDAILY = %d',RELATION = %d,PROCESSORID = 's' WHERE ID = %d"),
		_proAss.getFlag(),
		_proAss.getAbsOpenTime().printTime(),
		_proAss.getAbsCloseTime().printTime(),
		paxty,
		_proAss.IsDaily(),
		_proAss.getAssRelation(),
		prostr,
		_proAss.GetID()) ;
	try
	{
		CADODatabase::ExecuteSQLStatement(strsql,DATABASESOURCE_TYPE_ACCESS) ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return ;
	}

}
ProcessorID ProcAssignDatabase::GetProcAssignment(ProcessorRoster& _proAss , CADORecordset* _set)
{
	ProcessorID ID ;
	ID.SetStrDict(m_pInTerm->inStrDict) ;
	int flg = 0 ;
	CString strtime ;
	CString paxTY ;
	ElapsedTime opentime ;
	ElapsedTime closetime ;
	int isdaily = 0 ;
	int id = 0 ;
	int relation = 0;
	CString proid ;
	_set->GetFieldValue(_T("ID"),id) ;
	_set->GetFieldValue(_T("FLAG"),flg) ;
	_set->GetFieldValue(_T("OPENTIME"),strtime) ;
	opentime.SetTime(strtime) ;
	_set->GetFieldValue(_T("CLOSETIME"),strtime) ;
	closetime.SetTime(strtime) ;
	_set->GetFieldValue(_T("ISDAILY"),isdaily) ;
	_set->GetFieldValue(_T("PAXTY"),paxTY) ;
	_set->GetFieldValue(_T("RELATION") ,relation) ;
	_set->GetFieldValue(_T("PROCESSORID") ,proid) ;
	paxTY.Remove('(') ;
	paxTY.Remove(')') ;
	paxTY.MakeUpper(); paxTY.TrimLeft(), paxTY.TrimRight();
	_proAss.getAssignment()->getConstraint(0)->setAirline(paxTY) ;

	_proAss.setFlag(flg) ;
	_proAss.SetID(id) ;
	_proAss.setOpenTime(opentime) ;
	_proAss.setAbsOpenTime(opentime) ;
	_proAss.setCloseTime(closetime) ;
	_proAss.setAbsCloseTime(closetime) ;
	_proAss.setAssRelation((ProcAssRelation)relation) ;

	ID.setID(proid) ;
	return ID ;

}
void ProcAssignDatabase::ReadProcAssigmentFromDB()
{
	CString  strsql ;
	strsql.Format(_T("SELECT * FROM TB_PROCASSIGMENT_ROSTER")) ;
	CADORecordset recordSet ;
	long num = 0 ;
	try
	{
		CADODatabase::ExecuteSQLStatement(strsql,num,recordSet,DATABASESOURCE_TYPE_ACCESS) ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return ;
	}
	ProcessorRoster procAssign( m_pInTerm );
	ProcessorID id ;
	while (!recordSet.IsEOF())
	{
		id = GetProcAssignment(procAssign,&recordSet) ;
		insert (id, procAssign);
		initDaily(id, procAssign) ;
		recordSet.MoveNextData() ;
	}
}

void ProcAssignDatabase::WriteProAssigmentToDB()
{

	char procName[256] = {0};
	const ProcessorID *procID;
	ProcessorRosterSchedule *database;
	ProcessorRoster* pro_ass = NULL ;
	int entryCount = getCount();
	for (int i = 0; i < entryCount; i++)
	{
		procID = getProcName(i);
		procID->printID (procName);
		Processor* pro = m_pInTerm->GetProcessorList()->getProcessor(*procID) ;
		database = getDatabase(i);
		for (int j = 0; j < database->getCount(); j++)
		{
			pro_ass = database->getItem(j) ;
			WriteProAssignment(procName,*pro_ass) ;
		}
		SortedContainer<ProcessorRoster>&  _delProAss = database->GetDelProAssignment() ;
		for (int cout = 0 ; cout < _delProAss.getCount() ; cout++)
		{
			DeleteProAssigment(*_delProAss.getItem(cout)) ;
		}
	}
}
void ProcAssignDatabase:: DeleteProAssigment(ProcessorRoster& _proAss)
{
	if(_proAss.GetID() == -1 )
		return ;
	CString sqlstr ;
	sqlstr.Format(_T("DELETE FROM TB_PROCASSIGMENT_ROSTER WHERE ID = %d") ,_proAss.GetID()) ;
	try
	{
		CADODatabase::ExecuteSQLStatement(sqlstr,DATABASESOURCE_TYPE_ACCESS) ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return  ;
	}

}
