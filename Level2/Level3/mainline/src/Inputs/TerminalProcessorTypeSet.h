// TerminalProcessorTypeSet.h: interface for the CTerminalProcessorTypeSet class.
#pragma once
#include "..\common\dataset.h"
#include "engine\Car.h"
class CTerminalProcessorTypeSet : public DataSet  
{
public:
	CTerminalProcessorTypeSet();
	virtual ~CTerminalProcessorTypeSet();

	virtual void readData (ArctermFile& p_file);
	virtual void writeData (ArctermFile& p_file) const;
	virtual const char *getTitle (void) const { return "Terminal Processor Type data file" ;};
	virtual const char *getHeaders (void) const {return "Terminal Processor Type data " ;};
	virtual void clear (void);
public:
	BOOL IsProcessorTypeVisiable(int index);
	void SetProcessorTypeVisiable(int index, BOOL bAble);
	BOOL m_bProcessorTypeVisiable[18]; 
};
