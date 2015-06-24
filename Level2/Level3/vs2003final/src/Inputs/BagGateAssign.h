// BagGateAssign.h: interface for the BagGateAssign class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BAGGATEASSIGN_H__5E372DF8_9D94_4673_A7F5_D94008AFE528__INCLUDED_)
#define AFX_BAGGATEASSIGN_H__5E372DF8_9D94_4673_A7F5_D94008AFE528__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PROCDB.H"
#include "common\dataset.h"
#include "common\containr.h"
#include "inputs\procdb.h"
#include <CommonData/procid.h>
#include "inputs\assign.h"


class INPUTS_TRANSFER BagGateEntry : public ProcessorDataElement
{
protected:
    //CProbDistEntry *m_probDistEntry;
	ProcessorDistribution* m_probDistEntry;

public:
    virtual ~BagGateEntry ();

    BagGateEntry(const ProcessorID& id) : ProcessorDataElement (id)
        {m_probDistEntry = NULL;}

    ProcessorDistribution *getProbDist (void) const { return m_probDistEntry;}
    void setProbDist (ProcessorDistribution *_pProbDistEntry) {m_probDistEntry = _pProbDistEntry;}
	void Clear(void);
};


class INPUTS_TRANSFER BagGateAssign : public ProcessorDatabase , public DataSet
{
	bool m_bGateHasHigherPriorty;//Gate has higher priority when assign baggage device 

public:
	BagGateAssign();
	virtual ~BagGateAssign();

    // adds a ProcessorDataElement to the list if it does not already exist
    virtual void addEntry (const ProcessorID& pID);

    virtual void insert (const ProcessorID& pID, ProcessorDistribution* probDistEntry);

    virtual void readData (ArctermFile& p_file) ;
    virtual void writeData (ArctermFile& p_file) const;

    virtual void clear (void) { SortedContainer<ProcessorDataElement>::clear(); }

    virtual const char *getTitle (void) const
        { return "Bagage Device -- Gate"; }
    virtual const char *getHeaders (void) const
        { return "Gates"; }

	bool IfGateHasHigherPriority()const{ return m_bGateHasHigherPriorty;	};
	void SetPriorityFlag( bool _bFlag ){ m_bGateHasHigherPriorty = _bFlag;	};
	
	int removeAllEmptyEntry();

};

#endif // !defined(AFX_BAGGATEASSIGN_H__5E372DF8_9D94_4673_A7F5_D94008AFE528__INCLUDED_)
