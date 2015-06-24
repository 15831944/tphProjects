// ProcessorTags.h: interface for the CProcessorTags class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCESSORTAGS_H__510E604B_7D71_41BC_A495_E23B27A62BDD__INCLUDED_)
#define AFX_PROCESSORTAGS_H__510E604B_7D71_41BC_A495_E23B27A62BDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common\dataset.h"
#include <bitset>

#define PROCESSORTAGCOUNT 20

class CProcessorTags : public DataSet
{
public:
	static const int PROCTAGGEOGRAPHIC;
	static const int PROCTAGBEHAVIORAL;
	static const char* PROCTAGNAMES[];

	CProcessorTags();
	virtual ~CProcessorTags();

	virtual void clear();
	virtual void initDefaultValues();

	virtual void readData (ArctermFile& p_file);
	virtual void writeData (ArctermFile& p_file) const;

	virtual const char *getTitle (void) const { return "Processor Info Display Data"; }
    virtual const char *getHeaders (void) const{  return ".,..,..,,,"; }

	bool Get(int idx) const { return m_bsOn.at(idx); }
	void Set(int idx, bool b) { m_bsOn.set(idx, b); }
	void Flip(int idx) { m_bsOn.flip(idx); }

protected:
	std::bitset<PROCESSORTAGCOUNT> m_bsOn;
};

#endif // !defined(AFX_PROCESSORTAGS_H__510E604B_7D71_41BC_A495_E23B27A62BDD__INCLUDED_)
