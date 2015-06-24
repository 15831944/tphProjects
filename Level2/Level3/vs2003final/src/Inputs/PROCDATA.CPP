#include "stdafx.h"

#include <string.h>
#include "inputs\procdata.h"
#include "inputs\miscproc.h"
#include "common\termfile.h"
#include "common\exeption.h"
#include "in_term.h"

extern const CString LOGSTRING_LOADPROJECT;



MiscDataElement::~MiscDataElement ()
{
    if (miscData)
        delete miscData;

    miscData = NULL;
}

void MiscProcessorData::allocateMiscData (MiscData* &data) const
{
    switch (procType)
    {
        case PointProc:
            data = new MiscData;
            break;
        case DepSourceProc:
            data = new MiscDepSourceData;
			break;
		case DepSinkProc:
			data = new MiscDepSinkData;
			break;
        case LineProc:
            data = new MiscLineData;
            break;
        case BaggageProc:
            data = new MiscBaggageData;
            break;
        case HoldAreaProc:
            data = new MiscHoldAreaData;
            break;
        case GateProc:
            data = new MiscGateData;
			break;
        case FloorChangeProc:
            data = new MiscFloorChangeData;
            break;
        case BarrierProc:
            break;
		case MovingSideWalkProc:
			data = new MiscMovingSideWalkData;
			break;
		case Elevator:
			data = new MiscElevatorData;
			break;
		case ConveyorProc:
			data = new MiscConveyorData;
			break;
		case IntegratedStationProc:
			data = new MiscIntegratedStation;
			break;
		case StairProc:
			data = new MiscStairData;
			break;
		case EscalatorProc:
			data = new MiscEscalatorData;
			break;	
		case BillboardProcessor:
			data = new MiscBillboardData;
			break;
		case BridgeConnectorProc:
			data = new MiscBridgeConnectorData;
			break;
		case RetailProc:
			data = new MiscRetailProcData;
			break;

		default:
			{
				ASSERT(0);
			}
    }
}

int MiscProcessorData::readMiscData (ArctermFile& file, InputTerminal* _pInTerm )
{
	file.reset();
	file.skipLines (3);

	char *typeName = MiscData::getProcTypeName (procType);
	char type[64];

	ProcessorID id;

	id.SetStrDict( _pInTerm->inStrDict );
	MiscDataElement *entry;
	MiscData *data = NULL;


	while (file.getLine())
	{
		file.getField (type, 64);
		
		if ( typeName==NULL || stricmp (typeName, type))
			continue;

		
		if (!id.readProcessorID (file))
            throw new FileFormatError ("Miscellaneous Processor Data");

		
		allocateMiscData (data);
		if (data)
		{
			data->SetInputTerminal( _pInTerm );
			data->readMiscData (file);
			entry = new MiscDataElement (id);
			entry->setData (data);
			addItem (entry);
		}
		
	}

	buildHierarchy();
	return TRUE;
}

int MiscProcessorData::readObsoleteMiscData (ArctermFile& file, InputTerminal* _pInTerm )
{
	file.reset();
	file.skipLines (3);

	char *typeName = MiscData::getProcTypeName (procType);
	char type[64];

	ProcessorID id;

	id.SetStrDict( _pInTerm->inStrDict );
	MiscDataElement *entry;
	MiscData *data = NULL;

	while (file.getLine())
	{
		file.getField (type, 64);
		if (stricmp (typeName, type))
			continue;

		if (!id.readProcessorID (file))
            throw new FileFormatError ("Miscellaneous Processor Data");

		allocateMiscData (data);
		if (data)
		{
			data->SetInputTerminal( _pInTerm );
			data->readObsoleteMiscData (file );
			entry = new MiscDataElement (id);
			entry->setData (data);
			addItem (entry);
		}
	}

	buildHierarchy();
	return TRUE;
}

bool MiscProcessorData::IfUsedThisProcssor( const ProcessorID& _procID,InputTerminal*_pTerm,std::vector<CString>& _stringVec )const
{
	bool bret= false;
	int iCount = this->getCount();
	for( int i=0; i<iCount; ++i )
	{
		if( _procID.idFits(*getItem( i )->getID()))  
		{
			CString str = getItem( i )->getID()->GetIDString();
			str += " : has an entry ;";
			_stringVec.push_back(str);
			bret = true;
		}
		else if(getItem( i )->getID()->idFits(_procID) && !getItem(i)->getID()->isBlank())
		{
		   if(!_pTerm->HasBrother(_procID,*(getItem(i)->getID())))
		   {
			   CString str = _procID.GetIDString();
			   str += " : has an entry ;";
			   _stringVec.push_back(str);
			   bret = true;
		   }
		}
		if( this->getEntry( i )->IfUsedThisProcssor( _procID,_pTerm, _stringVec ,*getItem( i )->getID() ) )
		{
			bret= true;
		}
	}

	return bret;
}
int MiscProcessorData::writeMiscData (ArctermFile& file) const
{
    char procName[256];
    const ProcessorID *procID;
    MiscData *data;
    int entryCount = getCount();
    for (int i = 0; i < entryCount; i++)
    {
        data = getEntry(i);
        file.writeField (data->getProcTypeName());

        procID = getProcName(i);
        procID->printID (procName);
        file.writeField (procName);

        data->writeMiscData (file);
        file.writeLine();
    }
    return TRUE;
}


// removes and deallocates all entries that fit passed procID
void MiscProcessorData::removeAllEntries (const ProcessorID& id,InputTerminal *_Term)
{
    MiscDataElement *element;
    MiscData *data;

	for (int i = getCount() - 1; i >= 0; i--)
    {
        element = (MiscDataElement *)getItem(i);
        if (id.idFits (*(element->getID())))
            deleteItem (i);
		else if(element->getID()->idFits(id) && !element->getID()->isBlank())
		{
			if(!_Term->HasBrother(id,*element->getID()))
				deleteItem(i);
		}
        else
        {
            data = element->getData();
            if (data->removeID (id))
                deleteItem (i);
        }
    }
}

// finds all entries that fit passed oldID and changes them to newID
void MiscProcessorData::replaceAllEntries (const ProcessorID& oldID,
    const ProcessorID& newID,InputTerminal*_pTerm)
{
    MiscDataElement *element;
    MiscData *data;
    int entryCount = getCount();
    for (int i = 0; i < entryCount; i++)
    {
        element = (MiscDataElement* )getItem(i);
        if (oldID.idFits (*(element->getID())))
            element->changeID (newID);
		else if(element->getID()->idFits(oldID) && !element->getID()->isBlank())
		{
			if(!element->getID()->idFits(newID))
			{
				if(!_pTerm->HasBrother(oldID,*element->getID()))
					element->changeID(newID);
			}
		}
        data = element->getData();
		data->replaceID (oldID, newID);
    }
}


/*****
*
*	Miscellaneous Database
*
*	Have three version of file format:
*
*	Version 2.0
*	Version 21
*		Intermediate wrong version, and there is 
*			m_lQueueCapacity ( for all the client side )
*			m_nCyclicFreq ( for some project from Romi's side )		
*
*	Version 2.1
*		m_nCyclicFreq	
*
*	Version 3.1
*		m_nGreetingTime(holding area)
*
*	Version 3.2
*		m_pDependentProcList( cann't start new service if follow processors are occupied)
*****/

MiscDatabase::MiscDatabase() : DataSet ( MiscProcessorDataFile,(float) 3.8 )
{
	for (int i = 0; i < PROC_TYPES; i++)
		databases[i] = new MiscProcessorData;
}

MiscDatabase::~MiscDatabase()
{
    for (int i = 0; i < PROC_TYPES; i++)
		delete (databases[i]);
}

void MiscDatabase::clear (void)
{
    for (int i = 0; i < PROC_TYPES; i++)
		databases[i]->clear();
}

void MiscDatabase::readData (ArctermFile& p_file)
{
    int i;
    for (i = 0; i < PROC_TYPES; i++)
		databases[i]->setType(i);

	assert( m_pInTerm );
    for (i = 0; i < PROC_TYPES; i++)
        databases[i]->readMiscData (p_file, m_pInTerm );
}


void MiscDatabase::readObsoleteData (ArctermFile& p_file)
{
	int i;
	for (i = 0; i < PROC_TYPES; i++)
		databases[i]->setType(i);

	assert( m_pInTerm );
	for (i = 0; i < PROC_TYPES; i++)
	    databases[i]->readObsoleteMiscData (p_file, m_pInTerm );
}


void MiscDatabase::writeData (ArctermFile& p_file) const
{
    for (int i = 0; i < PROC_TYPES; i++)
        databases[i]->writeMiscData (p_file);
}

void MiscDatabase::removeAllEntries (const ProcessorID& id,InputTerminal *_term)
{
    for (int i = 0; i < PROC_TYPES; i++)
		databases[i]->removeAllEntries (id,_term);
}

void MiscDatabase::replaceAllEntries (const ProcessorID& oldID,
    const ProcessorID& newID,InputTerminal*_pTerm)
{
    for (int i = 0; i < PROC_TYPES; i++)
		databases[i]->replaceAllEntries (oldID, newID,_pTerm);
}

bool MiscDatabase::IfUsedThisProcssor( const ProcessorID& _procID ,InputTerminal *_term,std::vector<CString>&_stringList)const
{
	bool nret = false;
	for (int i = 0; i < PROC_TYPES; i++)
	{
		if( databases[i]->IfUsedThisProcssor( _procID , _term,_stringList) )
		{
			nret= true;
		}
	}
	return nret;		
}
