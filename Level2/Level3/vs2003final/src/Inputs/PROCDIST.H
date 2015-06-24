#ifndef PROCESSOR_DISTRIBUTION_H
#define PROCESSOR_DISTRIBUTION_H

#include "../common/ProbabilityDistribution.h"
#include "../common/template.h"
#include <CommonData/procid.h>

#define RANDOM      0
#define SEQUENTIAL  1

class InputTerminal;
class ProcessorDistribution : public EmpiricalDistribution
{
protected:

    ProcessorID *destinations;
	bool* m_pHaveDrawn;				// remember which destination has been picked.
    int destIndex;
	int m_nCurDestIndex;
	int m_nCurDestIDLength;
	//  [7/5/2004]int m_nDestCount;//destinations' count.
private:
	virtual void readDistribution (ArctermFile& p_file);
	ProcessorDistribution(const ProcessorDistribution& );
	ProcessorDistribution& operator=(const ProcessorDistribution&);
public:
    ProcessorDistribution() { destIndex = 0; destinations = NULL; /*m_nDestCount=0;*/ m_pHaveDrawn = NULL; }
    virtual ~ProcessorDistribution ();

    virtual void initDestinations (const ProcessorID *p_dests,
        const double *p_percents, int p_count);
	virtual void initDestinations (const ProcessorID *p_dests,int p_nDestProcsCount,
		const double *p_percents, int p_nPercentCount);
		
    // removes any group indexes within passed group
    void removeEntry (const ProcessorID& p_id,InputTerminal *_pTerm);
	virtual void DeletePipe( int _iIdx );
	virtual void DeleteOnePipe( int _iPipeIndex);

	void AddNewOneItem(const ProcessorID& p_id);

	bool getUsedProInfo(const ProcessorID& id,InputTerminal *_pTerm,std::vector<CString>& _strVec);
    // replaces all references to old ID with new ID
    void changeEntry (const ProcessorID& p_old, const ProcessorID& p_new,InputTerminal*_pTerm);

    const char *getProbabilityName (void) const { return "Processor"; }
    virtual int getProbabilityType (void) const { return PROCESSOR; };

    virtual void readDistribution (ArctermFile& p_file,InputTerminal * pInterm);
    virtual void readVersion1 (ArctermFile& p_file,InputTerminal * pInterm);
    void writeDistribution (ArctermFile& p_file) const;

    // returns a destination group index
    // if the flag RANDOM is passed the probability distribution is used
    // if the flag SEQUENTIAL is used the groups are returned in order
    // when using SEQUENTIAL selection, NULL is returned when all groups
	// in the tranistion row have been returned once
    const ProcessorID *getDestinationGroup (int p_flag);

    // simple accessor
    ProcessorID *getGroup (int p_ndx) const;

	int GetCurGroupIndex() const { return m_nCurDestIndex; }

	int GetCurDestIDLength() const { return m_nCurDestIDLength; }
	const ProcessorID* GetCurDestID() const
	{
		if(m_nCurDestIndex<0 || m_nCurDestIndex>=GetDestCount())return NULL;
		return &(destinations[m_nCurDestIndex]);
	}

	virtual bool IsCurOneToOne() const { return false; }
	int GetOneXOneState() const {return 0;}	

	bool IfProcessorExistInDist( const ProcessorID* _pProc ) const;
	int GetDestCount() const {/*[7/5/2004]return m_nDestCount;*/ return getCount();}
	const ProcessorID* getZeropercentDestGroup();

protected:
	// draw status related function

	// reset the draw status
	void ResetDrawStatus();

	// get index on draw number
	int GetIndexOnDrawNumber( int _nDrawNumber ) const;

	//get draw number
	int GetDrawNumber() const;

	virtual void ReClacDestinations();
};

#endif
