#ifndef BRIDGE_CONNECTOR_PAX_DATA_H
#define BRIDGE_CONNECTOR_PAX_DATA_H

#include "common\template.h"
#include "common\dataset.h"
#include "inputs\MobileElemConstraintDatabase.h"

#define BC_PAX_DATA_COUNT  1

// Bridge Connector passenger data type
enum BCPaxDataTypes
{
	EntryFlightTimeDatabase
};


class BridgeConnectorPaxData : public DataSet
{
protected:
    static char * far paxDataName[BC_PAX_DATA_COUNT+1];
    static char * far units[BC_PAX_DATA_COUNT+1];

    typedef CMobileElemConstraintDatabase * CMobileElemConstraintDatabasePointer;
    CMobileElemConstraintDatabasePointer paxData[BC_PAX_DATA_COUNT];

public:
    BridgeConnectorPaxData();
    virtual ~BridgeConnectorPaxData();

    CMobileElemConstraintDatabase *getPaxData(int ndx) const {  assert( ndx>=0 && ndx< BC_PAX_DATA_COUNT ); return paxData[ndx]; }
    const char *getDataName(int ndx) const { assert( ndx>=0 && ndx< BC_PAX_DATA_COUNT ); return paxDataName[ndx]; }
    char **getDataNames() const { return paxDataName; }
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
		return paxData[EntryFlightTimeDatabase];
	}
};

#endif //PASSENGER_DATA_H_2
