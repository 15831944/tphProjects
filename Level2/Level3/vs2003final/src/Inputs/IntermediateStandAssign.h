#pragma once
#include "InputsDll.h"
#include "common\dataset.h"

class INPUTS_TRANSFER IntermediateStandAssign :public DataSet
{
public:
	IntermediateStandAssign(void);
	virtual ~IntermediateStandAssign(void);

	virtual void readData (ArctermFile& p_file);

	virtual void writeData (ArctermFile& p_file) const;

	virtual const char *getTitle (void) const
	{ return "IntermediateStandAssign"; }
	virtual const char *getHeaders (void) const
	{ return "Assignment"; }

	virtual void clear();

public:
	bool m_bIntermediateStandAssign;
	bool m_bDifferentStand;
};
