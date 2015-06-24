// SideMoveWalkDataSet.h: interface for the CSideMoveWalkDataSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIDEMOVEWALKDATASET_H__C738939F_EFB4_4696_9ABB_6249ABA4A665__INCLUDED_)
#define AFX_SIDEMOVEWALKDATASET_H__C738939F_EFB4_4696_9ABB_6249ABA4A665__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common\dataset.h"
#include "PROCDB.H"
#include "SideMoveWalkProcData.h"


class CSideMoveWalkDataSet : public ProcessorDatabase, 	public DataSet  
{
private:
	CSideMoveWalkProcData * m_pDefaultData;
public:
	CSideMoveWalkDataSet();
	virtual ~CSideMoveWalkDataSet();

public:
	//ProcessorEntry to entryList if  one is not yet present
    virtual void addEntry (const ProcessorID& pID);
	CSideMoveWalkProcData * GetLinkedSideWalkProcData (const ProcessorID& pID);

    // if processor ID already has an entry in hierarchy, add the
    // ConstraintEntry to its PassengerConstraintDatabase
    // if it does not yet have one, create it
    int insert (const ProcessorID& id, CSideMoveWalkProcData * _pProcData);

    // returns PassengerConstraintDatabase of ProcessorEntry at passed index
    CSideMoveWalkProcData *getDatabase (int index) const
      { return (CSideMoveWalkProcData *) (getItem(index)); };

    virtual void clear (void) { SortedContainer<ProcessorDataElement>::clear(); }



    virtual void readData (ArctermFile& p_file);
    virtual void writeData (ArctermFile& p_file) const;

	virtual const char *getTitle (void) const { return "Moving side walk data";};
	virtual const char *getHeaders (void) const { return "all moving side walk data";};

};

#endif // !defined(AFX_SIDEMOVEWALKDATASET_H__C738939F_EFB4_4696_9ABB_6249ABA4A665__INCLUDED_)
