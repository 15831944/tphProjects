// MathProcessDataSet.h: interface for the CMathProcessDataSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATHPROCESSDATASET_H__50636960_E058_40FE_A7E3_925664749CAC__INCLUDED_)
#define AFX_MATHPROCESSDATASET_H__50636960_E058_40FE_A7E3_925664749CAC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../Common/dataset.h"
#include "MathProcess.h"
#include <vector>

class CMathProcessDataSet : public DataSet
{
public:
	CMathProcessDataSet();
	virtual ~CMathProcessDataSet();
	
	virtual void clear();
	virtual void readData(ArctermFile& p_file);
	virtual void readObsoleteData(ArctermFile& p_file);
	virtual void writeData(ArctermFile& p_file) const;
	virtual const char* getTitle() const{return "Process Data";}
	virtual const char* getHeaders() const{return "Name, Number, Time, Discipline, Configuration, Capacity";}

	
	int GetProcesses(std::vector<CMathProcess>* pProces);
	void SetProcesses(std::vector<CMathProcess>* pProces);
	int AddProcess(const CMathProcess& proc);
	
protected:
	std::vector<CMathProcess>* m_pProcesses;
};

#endif // !defined(AFX_MATHPROCESSDATASET_H__50636960_E058_40FE_A7E3_925664749CAC__INCLUDED_)
