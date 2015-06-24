// HubbingData.h: interface for the CHubbingData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HUBBINGDATA_H__E83E8D2A_CA8E_44FA_B61B_B5433A939AAF__INCLUDED_)
#define AFX_HUBBINGDATA_H__E83E8D2A_CA8E_44FA_B61B_B5433A939AAF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "pax_cnst.h"
#include "fltdist.h"

class CHubbingData  
{
protected:
	CPassengerConstraint m_const;
    ProbabilityDistribution* m_pTransiting;
    ProbabilityDistribution* m_pTransferring;
   
	FlightTypeDistribution* m_pTransferDest;
public:

	CHubbingData();
	virtual ~CHubbingData();


	// fill the empty item with default value
	void FillEmptyWithDefault();


	// fill the data in the data memeber.	
	void SetConstraint( const CPassengerConstraint& _const ){ m_const = _const; }
    void SetTransiting( ProbabilityDistribution* _pTransiting ){ m_pTransiting = _pTransiting; }
    void SetTransferring( ProbabilityDistribution* _pTransferring ){ m_pTransferring = _pTransferring; }
  
	void SetTransferDest( FlightTypeDistribution* _pTransferDest ){ m_pTransferDest = _pTransferDest; }

	CPassengerConstraint& GetConstraint(){ return m_const; }
    ProbabilityDistribution* GetTransiting(){ return m_pTransiting; }
    ProbabilityDistribution* GetTransferring(){ return m_pTransferring; }
    
	FlightTypeDistribution* GetTransferDest(){ return m_pTransferDest; }

	//ProbabilityDistribution* GetMinTransferTime(){ return m_pTransferDest; }

	void WipeIntrinsic();
	int FindBestFlight(const FlightConstraint &_aFlight);


    int operator == (const CHubbingData& _data)
        { return m_const == _data.m_const; }
    int operator < (const CHubbingData& _data)
        { return m_const < _data.m_const; }

};

#endif // !defined(AFX_HUBBINGDATA_H__E83E8D2A_CA8E_44FA_B61B_B5433A939AAF__INCLUDED_)
