#ifndef __LANDSIDE_PLACEMENT_DEF
#define __LANDSIDE_PLACEMENT_DEF

#pragma once
#include "common\dataset.h"
#include "engine\proclist.h"
#include "Processor2.h"

#include "placement.h"

class CLandsidePlacement :
	public CPlacement
{
public:
	CLandsidePlacement(ProcessorList* proclist, InputFiles fileType);
	virtual ~CLandsidePlacement(void);

	virtual void readData (ArctermFile& p_file);
	virtual void writeData (ArctermFile& p_file) const;

	virtual void BuildFromProcList(ProcessorList* _pProcList, CProcessor2::CProcDispProperties& _dispProp);

private:
	ProcessorList * m_pProcList;
};


#endif