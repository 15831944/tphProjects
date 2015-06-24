// MathFlowDataSet.h: interface for the CMathFlowDataSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATHFLOWDATASET_H__7075DEC0_11E4_48C2_AA04_3BCE93F3ED1A__INCLUDED_)
#define AFX_MATHFLOWDATASET_H__7075DEC0_11E4_48C2_AA04_3BCE93F3ED1A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <vector>
#include "../Common/dataset.h"
#include "MathFlow.h"


class CMathFlowDataSet : public DataSet
{
public:
	CMathFlowDataSet();
	virtual ~CMathFlowDataSet();
public:	
	virtual void clear();
	virtual void readData(ArctermFile& p_file);
	virtual void readObsoleteData(ArctermFile& p_file);
	virtual void writeData(ArctermFile& p_file) const;
	virtual const char* getTitle() const{return "Flow Data";}
	virtual const char* getHeaders() const{return "FlowName,ProcessName,ChildrenNO,ChildName,ChildPower,Distance";}

	
	int GetFlows(std::vector<CMathFlow>* pFlowVect);
	void SetFlows(std::vector<CMathFlow>* pFlowVect);
	int AddFlow(const CMathFlow& flow);
	
protected:
	std::vector<CMathFlow>* m_pFlowVect;

};

#endif // !defined(AFX_MATHFLOWDATASET_H__7075DEC0_11E4_48C2_AA04_3BCE93F3ED1A__INCLUDED_)
