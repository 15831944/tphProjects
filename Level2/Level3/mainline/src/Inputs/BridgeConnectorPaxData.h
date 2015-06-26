#pragma once
#include "common\template.h"
#include "common\dataset.h"
#include "CommonData\PROCID.H"
#include "inputs\MobileElemConstraintDatabase.h"

class BridgeConnectorPaxData : public DataSet
{
protected:
    CMobileElemConstraintDatabase* m_pPaxData;

public:
    BridgeConnectorPaxData();
    virtual ~BridgeConnectorPaxData();

    void deletePaxType(int p_level, int p_index);
    virtual void initDefaultValues();
    virtual void clear();
    virtual void readData(ArctermFile& p_file);
    virtual void writeData(ArctermFile& p_file) const;
    virtual void readObsoleteData( ArctermFile& p_file );

    virtual const char *getTitle() const
    { 
        return "Miscellaneous Passenger Data"; 
    }
    virtual const char *getHeaders() const
    { 
        return "Variable,Pax Type,Units,Distribution,Parameters"; 
    }

    CMobileElemConstraintDatabase* getEntryFlightTimeDestribution() const
    {
        return m_pPaxData;
    }
};
