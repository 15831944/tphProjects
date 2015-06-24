#include "stdafx.h"
#include "inputs\distlist.h"
#include "common\flt_cnst.h"
#include "inputs\flt_db.h"
#include "inputs\typelist.h"
#include "inputs\paxdist.h"
#include "inputs\in_term.h"
#include "common\termfile.h"
#include "common\exeption.h"


void PassengerDistributionList::readData (ArctermFile& p_file)
{
    FlightConstraint *aConst;
    PaxTypeDistribution *distribution;
    ConstraintEntry *anEntry;
	assert( m_pInTerm );
    const int *branches = m_pInTerm->inTypeList->getBranches();

	int iFirstReadTag = 0;  //matt
    while (p_file.getLine())
    {
		//
		if(!iFirstReadTag)  // first time to skip the % string 
		{
		   int nCount=0; 
		    p_file.getInteger(nCount); 
		
            for(int k=0;k < nCount+1;k++) 
			{
			   p_file.getLine();
			}
			iFirstReadTag=1;
		}
		iFirstReadTag=1;

        aConst = new FlightConstraint;
		aConst->SetAirportDB( m_pInTerm->m_pAirportDB );
        aConst->readConstraint (p_file);
        distribution = new PaxTypeDistribution (branches);
        distribution->readDistribution (p_file);
        anEntry = new ConstraintEntry;
		anEntry->initialize (aConst, distribution);
        addEntry (anEntry);
    }

    // add a default distribution
    if (!getCount())
        initDefaultValues();
}

void PassengerDistributionList::readObsoleteData(ArctermFile& p_file)
{
	assert( m_pInTerm );
	float fVersion = p_file.getVersion();
	if( fVersion == (float)2.3 ) // this is a error version
	{
		FlightConstraint *aConst;
		PaxTypeDistribution *distribution;
		ConstraintEntry *anEntry;
		const int *branches = m_pInTerm->inTypeList->getBranches();
		
		while (p_file.getLine())
		{
			aConst = new FlightConstraint;
			aConst->SetAirportDB( m_pInTerm->m_pAirportDB );
			aConst->readConstraint (p_file);
			distribution = new PaxTypeDistribution (branches);
			distribution->readOldDistribution(p_file);//
			// get the error line
			p_file.getLine();
			
			anEntry = new ConstraintEntry;
			anEntry->initialize (aConst, distribution);
			addEntry (anEntry);
		}
		
		// add a default distribution
		if (!getCount())
			initDefaultValues();
	}
	else if( fVersion ==(float)2.4)
	{
		FlightConstraint *aConst;
		PaxTypeDistribution *distribution;
		ConstraintEntry *anEntry;
		assert( m_pInTerm );
		const int *branches = m_pInTerm->inTypeList->getBranches();
		
		while (p_file.getLine())
		{
			aConst = new FlightConstraint;
			aConst->SetAirportDB( m_pInTerm->m_pAirportDB );
			aConst->readConstraint (p_file);
			distribution = new PaxTypeDistribution (branches);
			distribution->readOldDistribution(p_file);//readDistribution (p_file, m_pInTerm );
			anEntry = new ConstraintEntry;
			anEntry->initialize (aConst, distribution);
			addEntry (anEntry);
		}
		
		// add a default distribution
		if (!getCount())
			initDefaultValues();
	}
	else
	{
		readData( p_file );
	}
}

void PassengerDistributionList::writeData (ArctermFile& p_file) const
{
	assert( m_pInTerm );

	/***************///write headers retained "% % % % % % % % % % %"
/*

	int iLineCount =0;
	char sLineCount[8];
	int nodeCount = ((PaxTypeDistribution *)getDist(0))->getNodeCount();

    iLineCount = (int)(2*nodeCount / 1024) + 1;

	itoa(iLineCount,sLineCount,10);
	p_file.writeField (sLineCount);
    p_file.writeLine();

	int iEvLineNodeCount = int(nodeCount / iLineCount) + 1 ;

	char distHeaders[1048];
	ZeroMemory(distHeaders,1048);

	int iReadCount =0;//readed nodecount
	int iEvlineRead =0;
	    for(int j=0;j<iLineCount;j++)
		{
			ZeroMemory(distHeaders,1048);
			strcpy(distHeaders,"%");
			iReadCount ++;
            iEvlineRead =1;
            for(int i=0;i<iEvLineNodeCount;i++)
			{
				iReadCount ++;
				if(iReadCount > nodeCount)
					break;
				iEvlineRead ++;
				strcat (distHeaders, ",%");
			}
			p_file.writeField (distHeaders);
			p_file.writeLine();
		}
*/

    p_file.writeField ("1");
	p_file.writeLine();

	int nodeCount = ((PaxTypeDistribution *)getDist(0))->getNodeCount();

	char distHeaders[1048];
	ZeroMemory(distHeaders,1048);

	strcpy(distHeaders,"%");
	if(nodeCount < 50)
	{
		for(int i=0;i<nodeCount-1;i++)
		{
			strcat (distHeaders, ",%");
		}
		p_file.writeField (distHeaders);
		p_file.writeLine();
	}
	else
	{
		strcpy(distHeaders,"%,%,%,...");
		p_file.writeField (distHeaders);
		p_file.writeLine();
	}
	//***************/

    writeDatabase (p_file, NULL, m_pInTerm);
}

const char *PassengerDistributionList::getHeaders (void) const
{
    //static char distHeaders[256];//changed by matt
     static char distHeaders[256];

    if (!getCount())
        return "";

    strcpy (distHeaders, "Flight");
	
   // int nodeCount = ((PaxTypeDistribution *)getDist(0))->getNodeCount();
   // for (int i = 0; i < nodeCount; i++)
    //    strcat (distHeaders, ",%");
    return distHeaders;
}

void PassengerDistributionList::initDefaultValues (void)
{
	assert( m_pInTerm );
    const int *branches = m_pInTerm->inTypeList->getBranches();
    PaxTypeDistribution *defaultDist = new PaxTypeDistribution (branches);
    ConstraintEntry *anEntry = new ConstraintEntry;
    anEntry->initialize (new FlightConstraint, defaultDist);
    addEntry (anEntry);
}

void PassengerDistributionList::getDistName (int p_ndx, char *p_str) const
{
    const FlightConstraint *aConst = getConstraint (p_ndx);
    aConst->screenPrint (p_str);
}

void PassengerDistributionList::ClearEngineInitializeFlag()
{

	int entryCount = getCount();
	for (int i = 0; i < entryCount; i++)
	{
		PaxTypeDistribution *PaxDist = (PaxTypeDistribution *)getItem(i)->getValue();
		if(PaxDist)
			PaxDist->ClearEngineInitializeFlag();
	}
}
