// ProcessorDistributionWithPipe.h: interface for the CProcessorDistributionWithPipe class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCESSORDISTRIBUTIONWITHPIPE_H__F731CF90_78B3_4DAC_9367_EAF90C853AAA__INCLUDED_)
#define AFX_PROCESSORDISTRIBUTIONWITHPIPE_H__F731CF90_78B3_4DAC_9367_EAF90C853AAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PROCDIST.H"
#include "FlowItemEx.h"

class CProcessorDistributionWithPipe : public ProcessorDistribution  
{
protected:
	std::vector<CFlowItemEx> m_vPipeIndex;
	bool m_bReadProcessorDistribution;		// the key word is: PROCESSOR
private:
	virtual void readDistribution( ArctermFile& p_file);
public:
	CProcessorDistributionWithPipe( bool _bReadProcessorDistribution );
	CProcessorDistributionWithPipe( const CProcessorDistributionWithPipe& _otherDistribution );
	virtual ~CProcessorDistributionWithPipe();

	
	virtual void initDestinations (const ProcessorID *p_dests,
										const double *p_percents, 
										int p_count);
	virtual void initDestinations (const ProcessorID *p_dests,int p_nDestProcsCount,
		const double *p_percents, int p_nPercentCount)
	{
		ProcessorDistribution::initDestinations(p_dests,p_nDestProcsCount,p_percents,p_nPercentCount);
	}

	//if function will initialize the destinations using the flow items(m_vPipeIndex),and call InitRules() automatically,
	
	//void InitDesinations();



	
	int GetPipeCount() const { return (int)m_vPipeIndex.size();};
	CFlowItemEx GetPipe(int nIndex) const
	{
		if (nIndex >=0 && nIndex < (int)m_vPipeIndex.size())
		{
			return m_vPipeIndex[nIndex];
		}
		return CFlowItemEx();
	 }

	std::vector<CFlowItemEx>& GetPipeVector() { return m_vPipeIndex;	};

    const char *getProbabilityName (void) const { return "PROCESSORWITHPIPE"; }
    virtual int getProbabilityType (void) const { return PROCESSORWITHPIPE; };

	void DeletePipe( int _iPipeIdx );
	bool IfUseThisPipe( int _iPipeIdx )const;

	void DeleteOnePipe( int _iPipeIndex);
	void AddNewOneItem(const ProcessorID& id, const CFlowItemEx& item);

    virtual void readDistribution( ArctermFile& p_file,InputTerminal * pInterm);
	void readObsoleteDistribution25 (ArctermFile& p_file,InputTerminal * pInterm);
		
    void writeDistribution (ArctermFile& p_file) const;

	virtual bool IsCurOneToOne() const;
	virtual int GetOneXOneState() const;
	bool GetFollowState();
	const CFlowItemEx* GetCurFlowItem() const{ return (m_nCurDestIndex==-1 ? NULL : &(m_vPipeIndex[m_nCurDestIndex]));}
	virtual void InitRules();

	virtual void ReClacDestinations();
};








#endif // !defined(AFX_PROCESSORDISTRIBUTIONWITHPIPE_H__F731CF90_78B3_4DAC_9367_EAF90C853AAA__INCLUDED_)
