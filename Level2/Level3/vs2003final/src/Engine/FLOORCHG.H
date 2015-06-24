#ifndef FLOOR_CHANGE_PROCESSOR_H
#define FLOOR_CHANGE_PROCESSOR_H

#include "common\template.h"
#include "common\containr.h"
#include "engine\process.h"
#include <CommonData/ExtraProcProp.h>

class ENGINE_TRANSFER FloorChangeProcessor : public Processor, public FloorChangeProcProp
{
protected:
public:

    FloorChangeProcessor () { connections = NULL; connectionCount = 0; };

    //copy constructor.
    FloorChangeProcessor (const FloorChangeProcessor& aProc) { *this = aProc; };

    virtual ~FloorChangeProcessor () 
	{ if (connectionCount)
            delete connections; connections = NULL; };

    //Returns processor type.
    virtual int getProcessorType (void) const { return FloorChangeProc; };

	virtual ExtraProcProp* GetExtraProp() { return this; }

	//Returns processor name.
    virtual const char *getProcessorName (void) const { return "FLOOR"; };

    void initSpecificMisc (const MiscData *miscData);

    int isConnected (const Processor *aProc) const;

	int getConnectionCount() { return connectionCount; }
	const GroupIndex& getConnection(int _idx) { return connections[_idx]; }
	Point GetEntryPoint(const CString& _strMobType,int _iCurFloor, const CString& _strTime);

	virtual Processor* CreateNewProc();

};
      
#endif
