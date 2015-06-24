#include "stdafx.h"
#include "srvctime.h"
#include "MultiMobConstraint.h"
#include "../Common/ProbabilityDistribution.h"
#include "probab.h"
#include "PROCIDList.h"

// delete all references to the passed index at p_level
void ServiceTimeDatabase::deletePaxType (int p_level, int p_index)
{
    ProcessorEntry *entry;
    CMobileElemConstraintDatabase *database;

    for (int i = getCount() - 1; i >= 0; i--)
    {
        entry = (ProcessorEntry *)getItem (i);
        database = entry->getDatabase();
        database->deletePaxType (p_level, p_index);
        if (!database->getCount())
            deleteItem (i);
    }
}


void ServiceTimeDatabase::readData (ArctermFile& p_file)
{
	//ProcessorHierarchy::readData( p_file, true );
	ProcessorIDList idList;
	idList.ownsElements( 1 );
	CMultiMobConstraint multiConst;
	ProbabilityDistribution* prob;
    ConstraintEntryEx *anEntry;
    CMobileElemConstraint *aConst;
	ElapsedTime timeBegin, timeEnd;
	
	assert( m_pInTerm );
    while (p_file.getLine())
    {
        idList.readIDList (p_file, m_pInTerm );
        multiConst.readConstraints (p_file, m_pInTerm );
		
        if (units)
            p_file.skipField(1);
		
        prob = GetTerminalRelateProbDistribution (p_file, m_pInTerm,true );
		//////////////////////////////////////////////////////////////////////////
		// add by bird 2003/8/11 , to read begin_time , end_time 
		p_file.getTime( timeBegin );
		p_file.getTime( timeEnd );
		//////////////////////////////////////////////////////////////////////////

        for (int i = 0; i < idList.getCount(); i++)
            for (int j = 0; j < multiConst.getCount(); j++)
            {
                aConst = new CMobileElemConstraint(m_pInTerm);
                *aConst = *multiConst.getConstraint(j);
                anEntry = new ConstraintEntryEx;
                anEntry->initialize (aConst, prob, false, timeBegin, timeEnd );
                insert (*(idList.getID (i)), anEntry);
            }
    }
    buildHierarchy();
}

void ServiceTimeDatabase::readObsoleteData ( ArctermFile& p_file )
{
	assert( m_pInTerm );
	float fVersion = p_file.getVersion();
	
	//if( fVersion == (float)2.3 ) // this is a error version
	ProcessorIDList idList;
	idList.ownsElements( 1 );
	CMultiMobConstraint multiConst;
	ProbabilityDistribution* prob;
	ConstraintEntryEx *anEntry;
	CMobileElemConstraint *aConst;
	
	while (p_file.getLine())
	{
		idList.readIDList (p_file, m_pInTerm );
		multiConst.readConstraints (p_file, m_pInTerm );
		
		if (units)
			p_file.skipField(1);
		
		prob = GetTerminalRelateProbDistribution (p_file,m_pInTerm, true );
		
		for (int i = 0; i < idList.getCount(); i++)
			for (int j = 0; j < multiConst.getCount(); j++)
			{
				aConst = new CMobileElemConstraint(m_pInTerm);
				*aConst = *multiConst.getConstraint(j);
				anEntry = new ConstraintEntryEx;
				anEntry->initialize (aConst, prob);
				insert (*(idList.getID (i)), anEntry);
			}
	}
	buildHierarchy();
}

void ServiceTimeDatabase::writeData(ArctermFile& p_file) const 
{
	char procName[256];
    const ProcessorID *procID;
    CMobileElemConstraintDatabase *database;
	
    int entryCount = getCount();
	assert( m_pInTerm );
    for (int i = 0; i < entryCount; i++)
    {
        procID = getProcName(i);
        procID->printID (procName);
		
        database = getDatabase(i);
        database->writeDatabaseEx(p_file, procName, m_pInTerm);
    }
}

bool ServiceTimeDatabase::getUsedProInfo(const ProcessorID& _id,InputTerminal *_pTerm,std::vector<CString>& _strVec)
{
	bool nRet = false;
	
    const ProcessorID *procID;
    CMobileElemConstraintDatabase *database;
	
    int entryCount = getCount();
	assert( m_pInTerm );
    for (int i = 0; i < entryCount; i++)
    {
        procID = getProcName(i);
		
        database = getDatabase(i);
        if (_id.idFits (*procID))
		{
			ConstraintEntryEx* pEntry;
			const ProbabilityDistribution *aProbDist;
			CMobileElemConstraint* pConstraint;
			for (int j = 0; j < database->getCount(); j++)
			{
				CString str="";
				char tempChar[1024];
				
				pEntry = (ConstraintEntryEx*)database->getItem(j);
				pConstraint = (CMobileElemConstraint *)pEntry->getConstraint();
				aProbDist = pEntry->getValue();
				pConstraint->screenPrint(tempChar,0,1024);
				str = procID->GetIDString();
				str+=" :";
				str += tempChar;
				str += " ,";
				aProbDist->screenPrint(tempChar);
				str += tempChar;
				str +=" ;";
                _strVec.push_back(str);
			}
			nRet = true;
		}
		else if(procID->idFits(_id) && !procID->isBlank())
		{
			if(!_pTerm->HasBrother(_id,*procID))
			{
				ConstraintEntryEx* pEntry;
				const ProbabilityDistribution *aProbDist;
				CMobileElemConstraint* pConstraint;
				for (int j = 0; j < database->getCount(); j++)
				{
					CString str="";
					char tempChar[1024];
					
					pEntry = (ConstraintEntryEx*)database->getItem(j);
					pConstraint = (CMobileElemConstraint *)pEntry->getConstraint();
					aProbDist = pEntry->getValue();
					pConstraint->screenPrint(tempChar,0,1024);
					str = procID->GetIDString();
					str+=" :";
					str += tempChar;
					str += " ,";
					aProbDist->screenPrint(tempChar);
					str += tempChar;
					str +=" ;";
					_strVec.push_back(str);
				}
				nRet = true;
			}
		}

    }
	return nRet;
}