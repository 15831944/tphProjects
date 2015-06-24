#pragma once

#include <Common/containr.h>
#include <CommonData/procid.h>

class InputTerminal;
class ProcessorIDList : public UnsortedContainer<ProcessorID>
{
public:
	// Constructors and Destructors
	ProcessorIDList();
	virtual ~ProcessorIDList() {};

	//    ProcessorIDList (const ProcessorIDList& p_id) { *this = p_id; }
	//    const ProcessorIDList& operator = (const ProcessorIDList& p_id);
	int operator == (const ProcessorIDList& p_id) const;

	// returns 1 if ID list is blanked by removal
	int remove (const ProcessorID& id,InputTerminal *_pTerm);
	void replace (const ProcessorID& p_old, const ProcessorID& p_new,InputTerminal *_pTerm);
	const ProcessorID *getID (int p_ndx) const { return getItem (p_ndx); }
	void setID (int p_ndx, const ProcessorID& p_id);

	// simple accessors
	void initDefault (InputTerminal* _pInTerm);
	int areAllBlank (void) const;

	bool IfUsedThisProcssor( const ProcessorID& _procID ,InputTerminal*_pTerm,std::vector<CString>& _stringVec)const;

	// I/O routines
	virtual void readIDList (ArctermFile& p_file, InputTerminal* _pInTerm, bool _bWithOneToOne = true );
	virtual void writeIDList (ArctermFile& p_file) const;
	friend stdostream& operator << (stdostream& s, ProcessorIDList& list);
};



