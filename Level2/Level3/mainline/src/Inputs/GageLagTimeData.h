// GageLagTimeData.h: interface for the CGageLagTimeData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAGELAGTIMEDATA_H__E610934E_7A60_431D_AA67_396B5B843B94__INCLUDED_)
#define AFX_GAGELAGTIMEDATA_H__E610934E_7A60_431D_AA67_396B5B843B94__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
class ArctermFile;
class ProbabilityDistribution;
class InputTerminal; 
#include "PROCDB.H"

class CGageLagTimeData : public ProcessorDataElement  
{
	ProbabilityDistribution* m_pDist;
public:
	CGageLagTimeData();
	CGageLagTimeData(const ProcessorID& id);
	virtual ~CGageLagTimeData();
public:
	
	void SetDistribution( ProbabilityDistribution* _pDist ){ m_pDist = _pDist;	}
	ProbabilityDistribution* GetDistribution(){ return m_pDist;	}
	void WriteToFile( ArctermFile& _file )const;
	void ReadFromFile( ArctermFile& _file , InputTerminal* _pInTerm );

};

#endif // !defined(AFX_GAGELAGTIMEDATA_H__E610934E_7A60_431D_AA67_396B5B843B94__INCLUDED_)
