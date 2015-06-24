// MobileElemConstraintDatabase.cpp: implementation of the CMobileElemConstraintDatabase class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MobileElemConstraintDatabase.h"
#include "common\termfile.h"
#include "inputs\probab.h"
#include "inputs\fltdist.h"
#include "ProcessorDistributionWithPipe.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMobileElemConstraintDatabase::CMobileElemConstraintDatabase()
{

}

CMobileElemConstraintDatabase::~CMobileElemConstraintDatabase()
{

}



// find entry that equals p_const
// returns NULL if no constraint be found
const ProbabilityDistribution* CMobileElemConstraintDatabase::FindEqual(const CMobileElemConstraint& p_const) const
{
	int nEntryCount = getCount();
	for( int i = 0; i < nEntryCount; i++ )
	{
		const CMobileElemConstraint* pConstraint = (CMobileElemConstraint*)getItem(i)->getConstraint();
	    if( pConstraint->isEqual(&p_const) )
	        return getItem(i)->getValue();
	}
	return NULL;
}


// find first entry that fits p_const
// returns NULL if no constraint fits the p_const
const ProbabilityDistribution* CMobileElemConstraintDatabase::FindFit( const CMobileElemConstraint& p_const ) const
{
	//char szTmep[256];
	//p_const.printConstraint(szTmep);
	//// TRACE("%s\r\n",szTmep );

	int entryCount = getCount();
    for (int i = 0; i < entryCount; i++)
    {
		CMobileElemConstraint* pConstraint = (CMobileElemConstraint *)getItem(i)->getConstraint();
		//pConstraint->printConstraint(szTmep);
		//// TRACE("%s\r\n",szTmep );

//		if( pConstraint->fits (p_const) )
		if( pConstraint->fitex (p_const) )   // use fitex instead, fits does not check the CNonPaxCount
			return getItem(i)->getValue();
    }
    return NULL;
}

const ProbabilityDistribution* CMobileElemConstraintDatabase::FindFitEx( const CMobileElemConstraint& p_const, const ElapsedTime& _curTime ) const
{
	int entryCount = getCount();
    for (int i = 0; i < entryCount; i++)
    {
		ConstraintEntryEx* pEntryEx = (ConstraintEntryEx*)getItem(i);
		CMobileElemConstraint* pConstraint = (CMobileElemConstraint *)pEntryEx->getConstraint();
		long lcurTime = ((long)_curTime)%(WholeDay*TimePrecision);
		long lBeginTime = (long)pEntryEx->getBeginTime();
		long lEndTime = (long)pEntryEx->getEndTime();
		if( pConstraint->fitex (p_const)
			&& (lcurTime>=lBeginTime && lcurTime<= lEndTime))
			return pEntryEx->getValue();
    }
    return NULL;
}

// find first entry that fits p_const
// returns NULL if no constraint fits the p_const
const ConstraintEntry* CMobileElemConstraintDatabase::FindFitEntry(  const CMobileElemConstraint& p_const ) const
{
	int entryCount = getCount();
    for (int i = 0; i < entryCount; i++)
    {
		CMobileElemConstraint* pConstraint = (CMobileElemConstraint *)getItem(i)->getConstraint();	
		if( pConstraint->fits (p_const) )
			return getItem(i);
    }
    return NULL;
}


// delete all references to the passed index at p_level
void CMobileElemConstraintDatabase::deletePaxType( int p_level, int p_index )
{
	for( int i = getCount() - 1; i >= 0; i--)
	{
        if( (GetConstraint (i))->contains (p_level, p_index) )
            deleteItem (i);
	}
}


void CMobileElemConstraintDatabase::DeleteMobType( const CMobileElemConstraint* _pConstraint )
{
	for( int i = getCount() - 1; i >= 0; i-- )
	{
		if ( (GetConstraint (i))->isEqual( _pConstraint) )
		{
			deleteItem (i);
			return ;
		}   
	}
}


    // text based file I/O
void CMobileElemConstraintDatabase::readDatabase (ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm )
{
    clear();
    p_file.reset();
    p_file.skipLines (3);

    SortedContainer<ConstraintEntry> sortedList;
    sortedList.ownsElements (0);

    CMobileElemConstraint* pConst;
    ProbabilityDistribution* pProbDist;
    ConstraintEntry* pEntry;
    char str[64];
    int probField = 1 + (p_keyword != NULL) + (units != NULL);

    while (p_file.getLine())
    {
        if (p_keyword)
        {
            p_file.getField (str, SMALL);
            if (stricmp (str, p_keyword))
                continue;
        }

        pConst = new CMobileElemConstraint(_pInTerm);
		//pConst->SetInputTerminal( _pInTerm );
        pConst->readConstraint (p_file);
        if (units)
            p_file.getField (str, SMALL);

        p_file.setToField (probField);
        pProbDist = GetTerminalRelateProbDistribution (p_file,_pInTerm);

        pEntry = new ConstraintEntry;
        pEntry->initialize( pConst, pProbDist );
        sortedList.addItem( pEntry );
    }

    for (int i = 0; i < sortedList.getCount(); i++)
        addEntry (sortedList.getItem (i));
}
void CMobileElemConstraintDatabase::readDatabase26 (ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm )
{
    clear();
    p_file.reset();
    p_file.skipLines (3);

    SortedContainer<ConstraintEntry> sortedList;
    sortedList.ownsElements (0);

    CMobileElemConstraint* pConst;
    ProbabilityDistribution* pProbDist;
    ConstraintEntry* pEntry;
    char str[64];
    int probField = 1 + (p_keyword != NULL) + (units != NULL);

    while (p_file.getLine())
    {
        if (p_keyword)
        {
            p_file.getField (str, SMALL);
            if (stricmp (str, p_keyword))
                continue;
        }

        pConst = new CMobileElemConstraint(_pInTerm);
		//pConst->SetInputTerminal( _pInTerm );
        pConst->readConstraint (p_file);

		pConst->SetMobileElementType(enum_MobileElementType_PAX);
		pConst->SetTypeIndex(0);


        if (units)
            p_file.getField (str, SMALL);

        p_file.setToField (probField);
        pProbDist = GetTerminalRelateProbDistribution (p_file,_pInTerm);

        pEntry = new ConstraintEntry;
        pEntry->initialize( pConst, pProbDist );
        sortedList.addItem( pEntry );
    }

    for (int i = 0; i < sortedList.getCount(); i++)
        addEntry (sortedList.getItem (i));
}
void CMobileElemConstraintDatabase::readErrorDatabase( ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm )
{
	readDatabase( p_file, p_keyword, _pInTerm );
}


void CMobileElemConstraintDatabase::writeDatabase (ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm) const
{
    CMobileElemConstraint* pConstraint;
    ConstraintEntry* pEntry;
    const ProbabilityDistribution *aProbDist;

    int entryCount = getCount();
    for (int i = 0; i < entryCount; i++)
    {
        pEntry = getItem(i);

        if( p_keyword )
            p_file.writeField (p_keyword);

		  pConstraint = (CMobileElemConstraint *) pEntry->getConstraint();
		  pConstraint->SetInputTerminal( _pInTerm );
		  pConstraint->writeConstraint (p_file);

		  if (units)
				p_file.writeField (units);

		  aProbDist = pEntry->getValue();
		  aProbDist->writeDistribution (p_file);
		  p_file.writeLine();
	 }
}


void CMobileElemConstraintDatabase::writeDatabaseEx (ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm) const
{
	CMobileElemConstraint* pConstraint;
    ConstraintEntryEx* pEntry;
    const ProbabilityDistribution *aProbDist;
	
    int entryCount = getCount();
    for (int i = 0; i < entryCount; i++)
    {
        pEntry = (ConstraintEntryEx*)getItem(i);
		
        if( p_keyword )
            p_file.writeField (p_keyword);
		
		pConstraint = (CMobileElemConstraint *) pEntry->getConstraint();
		pConstraint->SetInputTerminal( _pInTerm );
		pConstraint->writeConstraint (p_file);
		
		if (units)
			p_file.writeField (units);
		
		aProbDist = pEntry->getValue();
		aProbDist->writeDistribution (p_file);
		//////////////////////////////////////////////////////////////////////////
		// add by bird 2003/8/11, to write begin_time , end_time 
		p_file.writeTime( pEntry->getBeginTime() );
		p_file.writeTime( pEntry->getEndTime() );
		//////////////////////////////////////////////////////////////////////////
		p_file.writeLine();
	}
}

int CMobileElemConstraintDatabase::checkLastOfProbs()
{
	ConstraintEntry *anEntry;
	const ProbabilityDistribution *aProbDist;

	int entryCount = getCount();
	for (int i = 0; i < entryCount; i++)
	{
	  anEntry = getItem(i);
	  aProbDist = anEntry->getValue();
	  if ((((FlightTypeDistribution*)aProbDist)->getLastOfProbs()) != 100)
			return 1;
	}
    return 0;
}




void CMobileElemConstraintDatabase::deletePipe ( int _iPipeIdx )
{
	for (int i = getCount() - 1; i >= 0; i--)
	{
		CProcessorDistributionWithPipe* pDistPipe = ( CProcessorDistributionWithPipe*) getDist( i );
		pDistPipe->DeletePipe( _iPipeIdx );
	}
}
bool CMobileElemConstraintDatabase::IfUseThisPipe( int _iPipeIdx, CMobileElemConstraint*& pConstr)const
{
	for (int i = getCount() - 1; i >= 0; i--)
	{
		const CProcessorDistributionWithPipe* pDistPipe = (const CProcessorDistributionWithPipe*) getDist( i );
		ConstraintEntry *anEntry = getItem(i);	
		if( pDistPipe->IfUseThisPipe( _iPipeIdx ) )
		{
			pConstr = (CMobileElemConstraint *) anEntry->getConstraint();
			return true;
		}
	}
	return false;
}

void CMobileElemConstraintDatabase::InitRules()
{
	for(int i = 0; i< getCount(); i++)
	{
		//const_cast<ProbabilityDistribution*>(getDist( i ))->InitRules();
		getItem( i )->getValue()->InitRules();
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
CNoPaxConstraintCoutDatabase::CNoPaxConstraintCoutDatabase()
{

}

CNoPaxConstraintCoutDatabase::~CNoPaxConstraintCoutDatabase()
{

}

// text based file I/O
void CNoPaxConstraintCoutDatabase::readDatabase (ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm )
{
	clear();
    p_file.reset();
    p_file.skipLines (3);
	
    SortedContainer<ConstraintEntryEx> sortedList;
    sortedList.ownsElements (0);
	
    CMobileElemConstraint* pConst;
    ProbabilityDistribution* pProbDist;
    ConstraintEntryEx* pEntry;
    char str[64];
    int probField = 1 + (p_keyword != NULL) + (units != NULL);
	
    while (p_file.getLine())
    {
        if (p_keyword)
        {
            p_file.getField (str, SMALL);
            if (stricmp (str, p_keyword))
                continue;
        }
		
        pConst = new CMobileElemConstraint(_pInTerm);
		//pConst->SetInputTerminal( _pInTerm );
        pConst->readConstraint (p_file);
        if (units)
            p_file.getField (str, SMALL);
		
        p_file.setToField (probField);
        pProbDist = GetTerminalRelateProbDistribution (p_file,_pInTerm);
		
		// read flag
		char chFlag;
		p_file.getChar( chFlag );
		bool flag = chFlag == 'Y' ? true : false;

        pEntry = new ConstraintEntryEx;
        pEntry->initialize( pConst, pProbDist, flag );
        sortedList.addItem( pEntry );
    }
	
    for (int i = 0; i < sortedList.getCount(); i++)
        addEntry (sortedList.getItem (i));
}

void CNoPaxConstraintCoutDatabase::readOldDatabase (ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm )
{
	//CMobileElemConstraintDatabase::readDatabase( p_file, p_keyword, _pInTerm );
	clear();
    p_file.reset();
    p_file.skipLines (3);
	
    SortedContainer<ConstraintEntryEx> sortedList;
    sortedList.ownsElements (0);
	
    CMobileElemConstraint* pConst;
    ProbabilityDistribution* pProbDist;
    ConstraintEntryEx* pEntry;
    char str[64];
    int probField = 1 + (p_keyword != NULL) + (units != NULL);
	
    while (p_file.getLine())
    {
        if (p_keyword)
        {
            p_file.getField (str, SMALL);
            if (stricmp (str, p_keyword))
                continue;
        }
		
        pConst = new CMobileElemConstraint(_pInTerm);
		//pConst->SetInputTerminal( _pInTerm );
        pConst->readConstraint (p_file);
        if (units)
            p_file.getField (str, SMALL);
		
        p_file.setToField (probField);
        pProbDist = GetTerminalRelateProbDistribution (p_file,_pInTerm);
		
        pEntry = new ConstraintEntryEx;
        pEntry->initialize( pConst, pProbDist );
        sortedList.addItem( pEntry );
    }
	
    for (int i = 0; i < sortedList.getCount(); i++)
        addEntry (sortedList.getItem (i));
}


void CNoPaxConstraintCoutDatabase::readErrorDatabase( ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm )
{
	readDatabase( p_file, p_keyword, _pInTerm );
}

void CNoPaxConstraintCoutDatabase::writeDatabase (ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm) const
{
	CMobileElemConstraint* pConstraint;
    ConstraintEntryEx* pEntry;
    const ProbabilityDistribution *aProbDist;
	
    int entryCount = getCount();
    for (int i = 0; i < entryCount; i++)
    {
        pEntry = (ConstraintEntryEx*)getItem(i);
		
        if( p_keyword )
            p_file.writeField (p_keyword);
		
		pConstraint = (CMobileElemConstraint *) pEntry->getConstraint();
		pConstraint->SetInputTerminal( _pInTerm );
		pConstraint->writeConstraint (p_file);
		
		if (units)
			p_file.writeField (units);
		
		aProbDist = pEntry->getValue();
		aProbDist->writeDistribution (p_file);
		//write flag
		p_file.writeChar( pEntry->getFlag() ? 'Y' : 'N' );
		
		p_file.writeLine();
	}
}

