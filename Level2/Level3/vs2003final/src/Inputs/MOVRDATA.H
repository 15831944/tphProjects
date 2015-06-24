#ifndef PEOPLE_MOVER_DATA_H
#define PEOPLE_MOVER_DATA_H

#include "common\elaptime.h"
#include "common\containr.h"
#include "common\dataset.h"
#include <CommonData/procid.h>
#include "PROCIDList.h"

class ProbabilityDistribution;

#define SYSTEM_NAME_LEN 80
#define DIST_COUNT      12

class INPUTS_TRANSFER PeopleMoverData
{
protected:
    ElapsedTime startTime;
    ProbabilityDistribution **distList;

public:
    PeopleMoverData ();
    virtual ~PeopleMoverData ();

    ElapsedTime getTime (void) const { return startTime; }
    void setTime (ElapsedTime p_time) { startTime = p_time; }

    const ProbabilityDistribution *getDist (int p_distNum) const
        { return distList[p_distNum]; }
    float getValue (int p_distNum) const;
    void setDist (ProbabilityDistribution *p_dist, int p_distNum)
        { distList[p_distNum] = p_dist; }

    int operator == (const PeopleMoverData &p_data)
        { return startTime == p_data.startTime; }
    int operator < (const PeopleMoverData &p_data)
        { return startTime < p_data.startTime; }

    const char *getUnits (int p_ndx) const;

    enum
    {
        SPEED,
        ACCEL,
        DECEL,
        TRAINS,
        CARS_PER_TRAIN,
        CAR_CAPACITY,
        DWELL_TIME,
        HEADWAY_TIME,
        SEPARATION,
        LOAD_LAG,
        HOLD_PROB,
        HOLD_TIME
    };
};

class PeopleMoverSystem : public SortedContainer<PeopleMoverData>
{
protected:
    char systemName[SYSTEM_NAME_LEN];
    ProcessorIDList stationList;
    int bidirectional;
    int onDemand;

public:
    PeopleMoverSystem () : SortedContainer<PeopleMoverData> (8, 0, 8)
        { bidirectional = onDemand = 0; }
    virtual ~PeopleMoverSystem () {};

    void setName (const char *p_str) { strcpy (systemName, p_str); }
    const char *getName (void) const { return systemName; }
    ProcessorIDList *getStations (void) { return &stationList; }

    int isBidirectional (void) const { return bidirectional; }
    void setBidirectional (int p_bidir) { bidirectional = p_bidir; }

    int isOnDemand (void) const { return onDemand; }
    void setOnDemand (int p_onDemand) { onDemand = p_onDemand; }

    int operator == (const PeopleMoverSystem &p_system)
        { return !strcmpi (systemName, p_system.systemName); }
};

class PeopleMoverDataSet : public DataSet, public UnsortedContainer<PeopleMoverSystem>
{
public:
    PeopleMoverDataSet() : DataSet (PeopleMoverFile),
        UnsortedContainer<PeopleMoverSystem> (8, 0, 8) {};
    virtual ~PeopleMoverDataSet () {};

    virtual const char *getTitle (void) const
        { return "People mover systems and operational characteristics"; }
    virtual const char *getHeaders (void) const
        { return "Name,Time,Speed,Accel,Decel,Trains,Cars,"
            "Capacity,Dwell,Headway Time,Separation,LoadLag,"
            "Hold Prob,Hold Time"; }

    // reads and writes people mover data to the passed file in CSV
    virtual void readData (ArctermFile& p_file);
    virtual void writeData (ArctermFile& p_file) const;

    // empties the databases
    virtual void clear (void) { UnsortedContainer<PeopleMoverSystem>::clear(); }

    // removes all references to the passed ProcessorID
    void removeAllEntries (const ProcessorID &p_id,InputTerminal *_pTerm);

    // replaces all references to the passed ProcessorID
    void replaceAllEntries (const ProcessorID &p_old,
        const ProcessorID &p_new,InputTerminal*_pTerm);
	bool getUsedProInfo(const ProcessorID& _id,InputTerminal *_pTerm,std::vector<CString>& _strVec);
};

#endif
