#include "stdafx.h"
#include "movrdata.h"
#include "probab.h"
#include "../Common/ProbabilityDistribution.h"
#include "in_term.h"


PeopleMoverData::PeopleMoverData ()
{
    startTime = 0l;
    distList = new ProbabilityDistribution *[DIST_COUNT];
    for (int i = 0; i < DIST_COUNT; i++)
        distList[i] = new ConstantDistribution (0.0);
}

PeopleMoverData::~PeopleMoverData ()
{
    if (distList)
    {
        delete [] distList;
        distList = NULL;
    }
}

const char *PeopleMoverData::getUnits (int p_ndx) const
{
    char *units[] =    // 13 chars
    {
        "meters/sec",
        "meters/sec^2",
        "meters/sec^2",
        "Count",
        "Cars",
        "People",
        "Seconds",
        "Seconds",
        "Seconds",
        "Seconds",
        "Percent",
        "Seconds",
    };
    return units[p_ndx];
}

float PeopleMoverData::getValue (int p_distNum) const
{
	return (float)((distList[p_distNum])->getRandomValue());
}

void PeopleMoverDataSet::readData (ArctermFile& p_file)
{
    clear();

    char str[SYSTEM_NAME_LEN];
    PeopleMoverSystem *system, tempSys;
    ProcessorIDList *idList;
    ProcessorID *procID;
    int flag;

    if (!p_file.seekToRecord ("SYSTEM", 0))
        return;

	assert( m_pInTerm );
    while (p_file.getLine())
    {
        system = new PeopleMoverSystem;
        addItem (system);

        p_file.getField (str, SYSTEM_NAME_LEN);
        system->setName (str);
        p_file.getInteger (flag);
        system->setBidirectional (flag);
        p_file.getInteger (flag);
        system->setOnDemand (flag);

        idList = system->getStations();
        while (!p_file.eol())
        {
            p_file.getField (str, 1024);
            procID = new ProcessorID;
			procID->SetStrDict( m_pInTerm->inStrDict );
            procID->setID (str);
            idList->addItem (procID);
        }
    }

    ElapsedTime time;
    PeopleMoverData *data;
    ProbabilityDistribution *prob;

    p_file.reset();
    p_file.seekToRecord ("NAME", 0);
	
	assert( m_pInTerm );
    while (p_file.getLine())
    {
        p_file.getField (str, SYSTEM_NAME_LEN);
        if (!strcmpi (str, "SYSTEM"))
            break;

        tempSys.setName (str);
        system = getItem (findItem (&tempSys));

        data = new PeopleMoverData;

        p_file.getField (str, 1024);
        time.SetTime (str);
        data->setTime (time);

        for (int i = 0; i < DIST_COUNT; i++)
        {
            prob = GetTerminalRelateProbDistribution (p_file,m_pInTerm);
            if (!prob)
                prob = new ConstantDistribution (0.0);
            data->setDist (prob, i);
        }
        system->addItem (data);
    }
}

void PeopleMoverDataSet::writeData (ArctermFile& p_file) const
{
    PeopleMoverSystem *system, tempSys;
    PeopleMoverData *data;
    const ProbabilityDistribution *prob;

    for (int i = 0; i < getCount(); i++)
    {
        system = getItem (i);

        for (int j = 0; j < system->getCount(); j++)
        {
            p_file.writeField (system->getName());
            data = system->getItem (j);
            p_file.writeTime (data->getTime(), 1);

            for (int k = 0; k < DIST_COUNT; k++)
            {
                prob = data->getDist (k);
                if (prob)
                    prob->writeDistribution (p_file);
                else
                    p_file.writeField ("0.0");
            }
            p_file.writeLine();
        }
    }

    int peakCount = 0;
    ProcessorIDList *idList;
    for (i = 0; i < getCount(); i++)
    {
        system = getItem (i);
        idList = system->getStations();
        if (idList->getCount() > peakCount)
            peakCount = idList->getCount();
    }

    p_file.writeField ("SYSTEM,BIDIRECTIONAL,ON DEMAND");
    char str[1024];
    for (i = 0; i < peakCount; i++)
    {
        strcpy (str, "STATION ");
        itoa (i+1, str + strlen (str), 10);
        p_file.writeField (str);
    }
    p_file.writeLine();

    ProcessorID *procID;
    for (i = 0; i < getCount(); i++)
    {
        system = getItem (i);
        p_file.writeField (system->getName());
        p_file.writeInt (system->isBidirectional());
        p_file.writeInt (system->isOnDemand());
        idList = system->getStations();
        for (int j = 0; j < idList->getCount(); j++)
        {
            procID = idList->getItem (j);
            procID->writeProcessorID (p_file);
        }
        p_file.writeLine();
    }
}

// removes all references to the passed ProcessorID
void PeopleMoverDataSet::removeAllEntries (const ProcessorID &p_id,InputTerminal *_pTerm)
{
    PeopleMoverSystem *system;
    ProcessorIDList *idList;

    for (int i = 0; i < getCount(); i++)
    {
        system = getItem (i);
        idList = system->getStations();
        idList->remove (p_id,_pTerm);
    }
}

bool PeopleMoverDataSet::getUsedProInfo(const ProcessorID& p_id,InputTerminal *_pTerm,std::vector<CString>& _strVec)
{
	bool bRet =false;
	PeopleMoverSystem *system;
    ProcessorIDList *idList;
	
    for (int i = 0; i < getCount(); i++)
    {
        system = getItem (i);
        idList = system->getStations();
        if(idList->IfUsedThisProcssor(p_id,_pTerm,_strVec))
		{
			CString str;
			str = p_id.GetIDString();
			str += "used in PeopleMoverSystem ;";
			_strVec.push_back(str);
			bRet = true;
		}
    }
	return bRet;
}
// replaces all references to the passed ProcessorID
void PeopleMoverDataSet::replaceAllEntries (const ProcessorID &p_old,
    const ProcessorID &p_new,InputTerminal*_pTerm)
{
    PeopleMoverSystem *system;
    ProcessorIDList *idList;

    for (int i = 0; i < getCount(); i++)
    {
        system = getItem (i);
        idList = system->getStations();
        idList->replace (p_old, p_new,_pTerm);
    }
}

