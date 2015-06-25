#include "stdafx.h"
#include <algorithm>
#include "inputs\prochrch.h"
#include "inputs\paxflow.h"
#include "inputs\pax_cnst.h"
#include <CommonData/procid.h>
#include "inputs\probab.h"
#include "inputs\procdist.h"
#include "common\termfile.h"
#include "inputs\ProcessorDistributionWithPipe.h"


//get the parents' ids and children's ids of the id be deleted
void PassengerFlowDatabase::GetParentsAndChildren(const ProcessorID& id, VECTVECTPROCID &vvParentsID, VECTVECTPROCID &vvChildrenID)
{
	unsigned size = 0;
	for(size = 0; size < vvParentsID.size(); size++)
	{
		vvParentsID[size].clear();		
	}
	vvParentsID.clear();
	for(size = 0; size < vvChildrenID.size(); size++)
	{
		vvChildrenID[size].clear();		
	}
	vvChildrenID.clear();

	ProcessorEntry *entry = NULL;
	CMobileElemConstraintDatabase *database;
    CProcessorDistributionWithPipe* pDist;
	
	for(int i = 0; i < getCount(); i++)
	{
		entry = (ProcessorEntry*)getItem(i);
		// TRACE("%s-", entry->getID()->GetIDString());
		database = entry->getDatabase();
		if( id.idFits(*entry->getID()) )
		{
			for (int j = 0; j < database->getCount(); j++)
			{
				pDist = (CProcessorDistributionWithPipe *)database->getDist(j);
				VECTPROCID vProcID;
				for(int k = 0; k < pDist->getCount(); k++)
				{
					// TRACE("%s- \n", pDist->getGroup(k)->GetIDString());
					vProcID.push_back(*(pDist->getGroup(k)));//add into the vector
				}
				vvChildrenID.push_back(vProcID);
			}
		}
		else
		{
			for (int j = 0; j < database->getCount(); j++)
			{
				if(vvParentsID.size() <= (unsigned)j)
				{
					VECTPROCID vParentID;
					vvParentsID.push_back(vParentID);
				}
				
				pDist = (CProcessorDistributionWithPipe *)database->getDist(j);
								
				for(int k = 0; k < pDist->getCount(); k++)
				{
					if( id.idFits( *( pDist->getGroup(k) ) ) )
					{
						//if the id of the parent already exists, do not push
						bool bfind = false;
						for(unsigned m = 0; m < vvParentsID[j].size(); m++)//find the id
						{
							if( vvParentsID[j].at(m).idFits(*entry->getID()) )
							{
								bfind = true;
								break;//find it 
							}
						}
						if(!bfind)//not find it, not exists, push it 
						{
							vvParentsID[j].push_back(*entry->getID());//add into the vector
							break;
						}
					}
				}
			}
		}
	}
	//debug info
	unsigned n = 0, m = 0;
	for( n = 0; n < vvParentsID.size(); n++ )
	{
		// TRACE("\nParent IDs:\n");
		for( m = 0; m < vvParentsID[n].size(); m++ )
		{
			// TRACE("%s-\n", vvParentsID[n].at(m).GetIDString());
		}
	}
	for( n = 0; n < vvChildrenID.size(); n++ )
	{
		// TRACE("\nChildren IDs:\n");
		for( m = 0; m < vvChildrenID[n].size(); m++ )
		{
			// TRACE("%s-\n", vvChildrenID[n].at(m).GetIDString());
		}
	}
}
//get the flowitemex vector of the id be deleted
void PassengerFlowDatabase::GetFlowItemOfProcessID(const ProcessorID& id, VECTVECTFLOWITEMEX& vvFlowItemEx)
{
	ProcessorEntry *entry = NULL;
	CMobileElemConstraintDatabase *database;
    CProcessorDistributionWithPipe* pDist;
	
	for(int i = 0; i < getCount(); i++)
	{
		entry = (ProcessorEntry*)getItem(i);
		database = entry->getDatabase();
		if( id.idFits(*entry->getID()) )
		{
			// TRACE("\n");
			// TRACE("%s- \n", id.GetIDString());
			// TRACE("%s- \n", entry->getID()->GetIDString());

			for (int j = 0; j < database->getCount(); j++)
			{
				pDist = (CProcessorDistributionWithPipe *)database->getDist(j);
				
				vvFlowItemEx.push_back(pDist->GetPipeVector());
			}
		}
	}

}

// when delete a process, add it's all children to it's parent
bool PassengerFlowDatabase::AddChildToParent(const ProcessorID& id)
{
	bool bResult = false;

    ProcessorEntry *entry;
    CMobileElemConstraintDatabase *database;
    CProcessorDistributionWithPipe* pDist;

	//find the parent of the id
 	//add by frank---------
	VECTVECTPROCID vvParentsID, vvChildrenID;
	GetParentsAndChildren(id, vvParentsID, vvChildrenID);
	VECTVECTFLOWITEMEX vvFlowItemEx;
	GetFlowItemOfProcessID(id, vvFlowItemEx);
	//---------------------------------
	for(int i = 0; i < getCount(); i++)
	{
		entry = (ProcessorEntry*)getItem(i);
		// TRACE("\n");
		// TRACE("%s- \n", entry->getID()->GetIDString());

		database = entry->getDatabase();
		for (int j = 0; j < database->getCount(); j++)
		{
			if(vvParentsID[j].size() == 0)
				continue;
			ProcessorID * pid = &(*std::find(vvParentsID[j].begin(), vvParentsID[j].end(), *entry->getID()));
			if(pid != &(*vvParentsID[j].end()))//find it
			{
				pDist = (CProcessorDistributionWithPipe *)database->getDist(j);
				if ((unsigned)j < vvChildrenID.size())
				{
					for (unsigned k = 0; k < vvChildrenID[j].size(); k++)
					{
						// TRACE("\n");
						// TRACE("%s- \n", vvChildrenID[j].at(k).GetIDString());
						pDist->AddNewOneItem(vvChildrenID[j].at(k), vvFlowItemEx[j].at(k));
						
						bResult = true;
					}
				}

			}			
		}
	}
	return bResult;
}
// removes and deallocates all entries that fit passed procID
// flows that are directed to passed id will be realigned or removed
void PassengerFlowDatabase::removeAllEntries (const ProcessorID& id)
{

	//add by frank---------
	bool bAddChildToParent = AddChildToParent(id);
	//---------------------------------

    ProcessorEntry *entry;
    CMobileElemConstraintDatabase *database;
    CProcessorDistributionWithPipe* pDist;

    for (int i = getCount() - 1; i >= 0; i--)
    {
        entry = (ProcessorEntry *)getItem (i);
        if (id.idFits (*(entry->getID())))
        {
            deleteItem(i);
            continue;
        }
		else if(entry->getID()->idFits(id) && !entry->getID()->isBlank())
		{
			if(!m_pInTerm->HasBrother(id,*entry->getID()))
			{
				deleteItem(i);
				continue;
			}
		}

        database = entry->getDatabase();
        for (int j = database->getCount() - 1; j >=0; j--)
        {
            pDist = (CProcessorDistributionWithPipe *)database->getDist(j);
            pDist->removeEntry (id,m_pInTerm);
            if (!pDist->getCount())
			{
				ProcessorID destID;
				assert( m_pInTerm );
				destID.SetStrDict( m_pInTerm->inStrDict );
				destID.setID ("END");
				double prob (100);
				pDist->initDestinations (&destID, &prob, 1);
				//added by frank 050629
				std::vector<CFlowItemEx> &vItem = pDist->GetPipeVector();
				CFlowItemEx endItem;
				vItem.push_back(endItem);
				//end added
			}
			else
			{
				if (bAddChildToParent)
				{
					pDist->ReClacDestinations();
				}
			
			}

        }
        if (!database->getCount())
            deleteItem (i);
    }
	

}
//
bool PassengerFlowDatabase::getUsedProInfo(const ProcessorID& id,InputTerminal *_pTerm,std::vector<CString>& _strVec)
{
	bool nRet = false;
    ProcessorEntry *entry;
    CMobileElemConstraintDatabase *database;
    CProcessorDistributionWithPipe* pDist;
	
    for (int i = getCount() - 1; i >= 0; i--)
    {
        entry = (ProcessorEntry *)getItem (i);
		 database = entry->getDatabase();
        if (id.idFits (*(entry->getID())))
        {
			char _TypeConstrain[1024];
			CString _str = entry->getID()->GetIDString();
			_str +=": ";
			if(database->getCount()<0) continue;
			for(int k=0;k<database->getCount();k++ )
			{
				 pDist = (CProcessorDistributionWithPipe *)database->getDist(k);
				 database->GetConstraint(k)->screenPrint(_TypeConstrain,0,1024);
				 _str += _TypeConstrain;
				 _str +=", ";
				 for(int h=0;h<pDist->getCount();h++)
				 {
                     _str += entry->getID()->GetIDString();
					 _str += " --> ";
					 _str +=pDist->getGroup(h)->GetIDString();
					 _str += " ;";
					 _strVec.push_back(_str);
				 }
			}
			
            nRet = true;
            continue;
        }
		else if(entry->getID()->idFits(id) && !entry->getID()->isBlank())
		{
			if(!_pTerm->HasBrother(id,*entry->getID()))
			{
				char _TypeConstrain[1024];
				CString _str=entry->getID()->GetIDString();
				_str +=": ";
				if(database->getCount()<0) continue;
					
					for(int k=0;k<database->getCount();k++ )
					{
						pDist = (CProcessorDistributionWithPipe *)database->getDist(k);
						database->GetConstraint(k)->screenPrint(_TypeConstrain,0,1024);
						_str += _TypeConstrain;
						_str +=", ";
						for(int h=0;h<pDist->getCount();h++)
						{
							_str += entry->getID()->GetIDString();
							_str += " --> ";
							_str +=pDist->getGroup(h)->GetIDString();
							_str += " ;";
							_strVec.push_back(_str);
						}
					}
					nRet = true;
					continue;
			}
		}
		
       
        for (int j = database->getCount() - 1; j >=0; j--)
        {
            pDist = (CProcessorDistributionWithPipe *)database->getDist(j);
            if(pDist->getUsedProInfo(id,_pTerm,_strVec))
			{
				CString _str="";
				char _TypeConstrain[1024];
				database->GetConstraint(j)->screenPrint(_TypeConstrain,0,1024);
				_str = _TypeConstrain;
				
				for (int h = pDist->getCount() - 1; h >= 0; h--)
				{
					if (id.idFits (*pDist->getGroup(h)))
					{
						_str = pDist->getGroup(h)->GetIDString();
						_str +=" :";
						_str += _TypeConstrain;
						_str +=", ";
						_str += entry->getID()->GetIDString();
						_str +=" --> ";
						_str += pDist->getGroup(h)->GetIDString();
						_str += " ;";
						_strVec.push_back(_str);
					}
					else if(pDist->getGroup(h)->idFits(id) && !pDist->getGroup(h)->isBlank())
					{
						if(!_pTerm->HasBrother(id,*pDist->getGroup(h)))
						{
							_str = pDist->getGroup(h)->GetIDString();
							_str +=" :";
							_str += _TypeConstrain;
							_str +=", ";
							_str += entry->getID()->GetIDString();
							_str +=" --> ";
							_str += pDist->getGroup(h)->GetIDString();
							_str += " ;";
							_strVec.push_back(_str);
							
						}
					}
				}
				
				nRet = true;
			}
        }
    }
	return nRet;
}
// finds all entries that fit passed oldID and changes them to newID
void PassengerFlowDatabase::replaceAllEntries (const ProcessorID& oldID,
    const ProcessorID& newID,InputTerminal* _term)
{
    ProcessorEntry *entry;
    CMobileElemConstraintDatabase *database;
    CProcessorDistributionWithPipe* pDist;

    for (int i = getCount() - 1; i >= 0; i--)
    {
        entry = (ProcessorEntry *)getItem (i);
        if (oldID.idFits (*(entry->getID())))
        {
            entry->changeID (newID);
            continue;
        }
		else if(entry->getID()->idFits(oldID) && !entry->getID()->isBlank())
		{
			if(!entry->getID()->idFits(newID))
			{
				if(!m_pInTerm->HasBrother(oldID,*entry->getID()))
				{
					entry->changeID(newID);
					continue;
				}
			}
		}

        database = entry->getDatabase();
        for (int j = database->getCount() - 1; j >=0; j--)
        {
            pDist = (CProcessorDistributionWithPipe *)database->getDist(j);
            pDist->changeEntry (oldID, newID,m_pInTerm);
        }
    }
}

// delete all references to the passed index at p_level
void PassengerFlowDatabase::deletePaxType (int p_level, int p_index)
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
void PassengerFlowDatabase::deletePaxType( const CMobileElemConstraint* _paxConstraint )
{
	ProcessorEntry *entry;
    CMobileElemConstraintDatabase *database;

    for (int i = getCount() - 1; i >= 0; i--)
    {
        entry = (ProcessorEntry *)getItem (i);
        database = entry->getDatabase();
        database->DeleteMobType( _paxConstraint );
        if (!database->getCount())
            deleteItem (i);
    }
}
void PassengerFlowDatabase::deletePipe( int _iPipeIdx )
{
	ProcessorEntry *entry;
    CMobileElemConstraintDatabase *database;

    for (int i = getCount() - 1; i >= 0; i--)
    {
        entry = (ProcessorEntry *)getItem (i);
        database = entry->getDatabase();
        database->deletePipe( _iPipeIdx );

    }
}
bool PassengerFlowDatabase::IfUseThisPipe( int _iPipeIdx, CMobileElemConstraint*& pConstr) const
{
	ProcessorEntry *entry;
	CMobileElemConstraintDatabase *database;
    for (int i = getCount() - 1; i >= 0; i--)
    {
        entry = (ProcessorEntry *)getItem (i);
        database = entry->getDatabase();
		if( database->IfUseThisPipe( _iPipeIdx,pConstr) )
			return true;
    }
	return false;
}
void PassengerFlowDatabase::initDefaultValues (void)
{/*
    ProcessorID destID;

	assert( m_pInTerm );
	destID.SetStrDict( m_pInTerm->inStrDict );
    destID.setID ("END");
    unsigned char prob (100);

    ProcessorDistribution *procDist = new ProcessorDistribution;
    procDist->initDestinations (&destID, &prob, 1);

    ConstraintEntry *anEntry = new ConstraintEntry;
    anEntry->initialize (new PassengerConstraint, procDist);

    ProcessorID sourceID;

	sourceID.SetStrDict( m_pInTerm->inStrDict );
    sourceID.setID ("START");
    insert (sourceID, anEntry);*/
}

const char *PassengerFlowDatabase::getHeaders (void) const
{
    //static char flowHeaders[256];
    ProcessorEntry *entry;
	CMobileElemConstraintDatabase *database;
    CProcessorDistributionWithPipe* pProcDist;
    int destCount = 0;

    for (int i = 0; i < getCount(); i++)
    {
        entry = (ProcessorEntry *)getItem (i);
        database = entry->getDatabase();
        for (int j = 0; j < database->getCount(); j++)
        {
            pProcDist = (CProcessorDistributionWithPipe *)database->getDist (j);
            if (pProcDist->getCount() > destCount)
                destCount = pProcDist->getCount();
        }
    }

    /*
	strcpy (flowHeaders, "Origin,Passenger Type");
    
	for (i = 0; i < destCount; i++)
        strcat (flowHeaders, ",Destination,Percent,Pipes");
	
    return flowHeaders;
	*/
	return "Origin,Passenger Type,Destination,Percent,Pipes,...,Destination,Percent,Pipes";
}


// check if the Mobile Type has its onw start;
bool PassengerFlowDatabase::HasOwnStart( const CMobileElemConstraint& _constr )
{
    ProcessorID idStart;
	assert( m_pInTerm );
	idStart.SetStrDict( m_pInTerm->inStrDict );
    idStart.setID ("START");
	
	ProcessorEntry* pEntry = (ProcessorEntry*)FindEntry ( idStart );
	if( !pEntry )
		return false;

	CMobileElemConstraintDatabase* pDB = pEntry->getDatabase();
	int nEntryCount = pDB->getCount();
    for( int i = 0; i < nEntryCount; i++ )
    {
		CMobileElemConstraint* pConstraint = (CMobileElemConstraint *)pDB->getItem(i)->getConstraint();
		if( pConstraint->fits ( _constr ) && pConstraint->GetTypeIndex() > 0  )
			return true;
    }
	return false;	
}


void PassengerFlowDatabase::readData (ArctermFile& p_file)
{
	ProcessorHierarchy::readData( p_file, true );
}

void PassengerFlowDatabase::readObsoleteData(ArctermFile& p_file)
{
	ProcessorHierarchy::readObsoleteData( p_file, true );
}

void PassengerFlowDatabase::InitFlowRules()
{
	ProcessorEntry *entry;

    for (int i = 0; i < getCount(); i++)
    {
        entry = (ProcessorEntry *)getItem (i);
        entry->getDatabase()->InitRules();
	}
}