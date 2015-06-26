#pragma once
#include "common\template.h"
#include "common\dataset.h"
#include "Engine\PROCESS.H"
#include "MobileElemConstraint.h"
#include "MobileElemConstraintDatabase.h"

class BridgePaxData : public CMobileElemConstraint
{
public:
    BridgePaxData();
    ~BridgePaxData();
protected:
    Processor m_procID;
};

class BridgeConnectorPaxDatabase : public CMobileElemConstraintDatabase, DataSet
{
public:
    BridgeConnectorPaxDatabase();
    virtual ~BridgeConnectorPaxDatabase();
    virtual void initDefaultValues();
    virtual void clear();
    virtual void readData(ArctermFile& p_file);
    virtual void writeData(ArctermFile& p_file)const;
    virtual void readObsoleteData( ArctermFile& p_file );

    virtual const char *getTitle()const
    {
        return "Miscellaneous Passenger Data";
    }
    virtual const char *getHeaders()const
    {
        return "Variable,Pax Type,Units,Distribution,Parameters";
    }
};

